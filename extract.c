#include "espresso_zip.h"
#include <sys/stat.h>
#include <stdio.h>
#include <zlib.h>
#include <gperftools/tcmalloc.h>
#include <string.h>

unsigned char XOR_KEYS[256] = {102, 2, 176, 55, 42, 22, 250, 196, 85, 206, 197, 121, 66, 61, 86, 134, 211, 178, 243, 161, 23, 244, 49, 48, 39, 62, 242, 180, 109, 229, 248, 247, 182, 64, 145, 150, 114, 100, 174, 117, 98, 99, 154, 120, 149, 217, 52, 225, 70, 190, 94, 132, 27, 90, 124, 222, 183, 74, 88, 164, 152, 202, 10, 253, 53, 194, 224, 158, 103, 82, 6, 240, 28, 93, 40, 13, 30, 146, 8, 14, 34, 32, 168, 131, 219, 162, 157, 56, 21, 104, 122, 33, 234, 67, 5, 143, 175, 12, 16, 58, 148, 17, 133, 187, 128, 91, 186, 110, 106, 79, 127, 113, 24, 201, 129, 170, 76, 3, 118, 216, 220, 111, 185, 155, 84, 199, 160, 204, 1, 38, 51, 147, 144, 75, 105, 214, 81, 254, 235, 172, 78, 41, 83, 0, 46, 59, 230, 173, 4, 195, 212, 25, 112, 29, 57, 163, 179, 89, 19, 95, 92, 97, 228, 71, 7, 123, 237, 26, 68, 210, 44, 251, 226, 54, 45, 73, 184, 171, 159, 137, 236, 188, 140, 65, 136, 192, 166, 193, 119, 125, 36, 18, 107, 50, 37, 246, 9, 96, 141, 208, 31, 181, 165, 80, 231, 252, 177, 215, 135, 156, 169, 151, 200, 203, 77, 255, 142, 69, 241, 191, 205, 153, 138, 15, 198, 63, 167, 227, 207, 115, 245, 218, 101, 87, 233, 238, 239, 72, 232, 20, 126, 223, 189, 139, 130, 249, 60, 11, 43, 35, 116, 221, 47, 209, 213, 108};

const char *MAGIC = "ESPL";

void *tcmalloc_alloc(void *opaque, unsigned items, unsigned size) {
    return tc_malloc(items * size);
}

void tcmalloc_free(void *opaque, void *address) {
    tc_free(address);
}

void coffee_recovery_magic(FILE *f_ezip, const char *fname) {
    fprintf(stderr, "==== START: Inflate Algorithm(zlib) ====\n");

    char esso_fname[4096];
    snprintf(esso_fname, sizeof(esso_fname), "%s.esso", fname);

    FILE *f_esso = original_file_open(esso_fname, "wb");
    if (f_esso == NULL) {
        fprintf(stderr, "==== ERROR: Failed to open %s for writing\n", esso_fname);
        return;
    }

    int ret;
    unsigned char in[DEFLATE_CHUNK];
    unsigned char out[DEFLATE_CHUNK];
    z_stream inflate_stream = {0};

    inflate_stream.zalloc = tcmalloc_alloc;
    inflate_stream.zfree = tcmalloc_free;
    inflate_stream.opaque = Z_NULL;

    ret = inflateInit(&inflate_stream);
    if (ret != Z_OK) {
        fprintf(stderr, "==== ERROR: Inflate(init) failed!\n");
        original_file_close(esso_fname, f_esso);
        return;
    }

    do {
        inflate_stream.avail_in = fread(in, 1, DEFLATE_CHUNK, f_ezip);
        if (inflate_stream.avail_in == 0) break;
        inflate_stream.next_in = in;

        do {
            inflate_stream.avail_out = DEFLATE_CHUNK;
            inflate_stream.next_out = out;
            ret = inflate(&inflate_stream, Z_NO_FLUSH);

            if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
                fprintf(stderr, "==== ERROR: Inflate failed! ret = %d\n", ret);
                inflateEnd(&inflate_stream);
                original_file_close(esso_fname, f_esso);
                return;
            }

            size_t have = DEFLATE_CHUNK - inflate_stream.avail_out;
            fwrite(out, 1, have, f_esso);

        } while (inflate_stream.avail_out == 0);

    } while (ret != Z_STREAM_END);

    inflateEnd(&inflate_stream);
    original_file_close(esso_fname, f_esso);
    fprintf(stderr, "==== END: Inflate Algorithm(zlib) ====\n");

    // 압축 해제된 데이터를 원본 파일로 저장
    f_esso = original_file_open(esso_fname, "rb");
    if (!f_esso) {
        fprintf(stderr, "==== ERROR: Failed to open %s for reading\n",esso_fname);
        return;
    }

    FILE *f_original = original_file_open(fname, "wb");
    if (!f_original) {
        fprintf(stderr, "==== ERROR: Failed to open %s for writing\n", fname);
        original_file_close(esso_fname,f_esso);
        return;
    }

    // 헤더 확인
    char magic_buffer[MAGIC_LEN];
    if (fread(magic_buffer, sizeof(char), MAGIC_LEN, f_esso) != MAGIC_LEN || memcmp(magic_buffer, MAGIC, MAGIC_LEN) != 0) {
        fprintf(stderr, "Error: Invalid MAGIC header\n");
        original_file_close(esso_fname,f_esso);
        original_file_close(fname, f_original);
        return;
    }

    unsigned char data_byte, count_byte;
    _Bool byte_count = 1;

    while (fread(&data_byte, sizeof(data_byte), 1, f_esso) == 1 &&
           fread(&count_byte, sizeof(count_byte), 1, f_esso) == 1) {

        // 역연산 수행
        for(int rep = 3; rep < XOR_LEN; rep++) {
            count_byte = XOR_KEYS[count_byte];
            count_byte ^= XOR_KEYS[rep-3];
            count_byte ^= XOR_KEYS[rep];
            if (byte_count) count_byte = ~count_byte;
            count_byte = (count_byte >> SHIFT_BITS*2) | (count_byte << (8 - SHIFT_BITS*2));
            count_byte ^= XOR_KEYS[rep-2];
            count_byte ^= XOR_KEYS[rep-1];
        }

        for(int rep = XOR_LEN-2; rep >= 0; rep--) {
            data_byte = XOR_KEYS[data_byte];
            if (!byte_count) data_byte = ~data_byte;
            data_byte ^= XOR_KEYS[rep+1];
            data_byte = (data_byte >> SHIFT_BITS) | (data_byte << (8 - SHIFT_BITS));
            data_byte ^= XOR_KEYS[rep+2];
            data_byte ^= XOR_KEYS[rep];
        }

        byte_count = !byte_count;

        // count_byte 값 검증
        if (count_byte > 0) {
            for (int i = 0; i < count_byte; i++) {
                fwrite(&data_byte, sizeof(data_byte), 1, f_original);
            }
        } else {
            fprintf(stderr, "==== WARN: Skipping invalid count_byte value: %d\n", count_byte);
        }
    }

    original_file_close(esso_fname, f_esso);
    original_file_close(fname, f_original);

    if (unlink(esso_fname) == 0) {
        fprintf(stderr, "==== INFO: temporary file (*.esso) has cleared. ====\n");
    } else {
        fprintf(stderr, "==== WARN: temporary file (*.esso) remove failed. ====\n");
    }

    fprintf(stderr, "==== END: Coffee Recovery Magic function ====\n");
}
