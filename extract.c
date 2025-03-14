#include "espresso_zip.h"
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>
#include<gperftools/tcmalloc.h>

int XOR_KEYS[97] = 
{
389, 379, 67, 347, 431, 491, 19, 499, 107, 113, 229, 307, 503, 71, 461, 179, 397, 2, 11, 509, 103, 281, 29, 373, 337, 353, 7, 317, 359, 479, 311, 421, 271, 443, 457, 157, 227, 61, 59, 41, 97, 139, 31, 109, 37, 43, 251, 199, 263, 3, 151, 409, 367, 191, 127, 463, 197, 269, 313, 17, 13, 163, 137, 181, 211, 167, 467, 487, 433, 331, 349, 101, 239, 383, 79, 53, 223, 73, 83, 5, 277, 193, 23, 47, 131, 401, 233, 149, 439, 449, 419, 89, 173, 241, 257, 293, 283
};



const char* MAGIC = "ESPL";

void *tc_malloc_alloc(void *opaque, unsigned items, unsigned size) {
    return tc_malloc(items * size);
}

void tc_malloc_free(void *opaque, void *address) {
    tc_free(address);
}

void coffee_recovery_magic(FILE *f_ezip, const char *fname) {
    fprintf(stderr, "==== START: Inflate Algorithm(zlib) ====\n");

    char *recovered_fname = tc_malloc(strlen(fname) + 6); // ".esso" 추가 고려
    sprintf(recovered_fname, "%s.esso", fname);

    FILE *f_tmp = tmpfile();
    if (!f_tmp) {
        fprintf(stderr, "==== ERROR: Failed to create temp file\n");
        tc_free(recovered_fname);
        return;
    }

    int ret;
    unsigned char in[INFLATE_CHUNK];
    unsigned char out[INFLATE_CHUNK];
    z_stream inflate_stream = {0};

    inflate_stream.zalloc = tc_malloc_alloc;
    inflate_stream.zfree = tc_malloc_free;
    inflate_stream.opaque = Z_NULL;

    ret = inflateInit(&inflate_stream);
    if (ret != Z_OK) {
        fprintf(stderr, "==== ERROR: Inflate(init) failed!\n");
        fclose(f_tmp);
        tc_free(recovered_fname);
        return;
    }

    do {
        inflate_stream.avail_in = fread(in, 1, INFLATE_CHUNK, f_ezip);
        if (inflate_stream.avail_in == 0) break;
        inflate_stream.next_in = in;

        do {
            inflate_stream.avail_out = INFLATE_CHUNK;
            inflate_stream.next_out = out;
            ret = inflate(&inflate_stream, Z_NO_FLUSH);

            if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
                fprintf(stderr, "==== ERROR: Inflate failed! ret = %d\n", ret);
                inflateEnd(&inflate_stream);
                fclose(f_tmp);
                tc_free(recovered_fname);
                return;
            }

            size_t have = INFLATE_CHUNK - inflate_stream.avail_out;
            fwrite(out, 1, have, f_tmp);

        } while (inflate_stream.avail_out == 0);

    } while (ret != Z_STREAM_END);

    inflateEnd(&inflate_stream);
    fprintf(stderr, "==== END: Inflate Algorithm(zlib) ====\n");

    // 압축 해제된 데이터를 원본 파일로 저장
    FILE *f_esso = fopen(recovered_fname, "wb");
    if (!f_esso) {
        fprintf(stderr, "==== ERROR: Failed to open %s for writing\n", recovered_fname);
        fclose(f_tmp);
        tc_free(recovered_fname);
        return;
    }

    // 중간 파일에서 다시 읽어 RLE 복원 수행
    rewind(f_tmp);

    // 헤더 확인
    char magic_buffer[MAGIC_LEN];
    if (fread(magic_buffer, sizeof(char), MAGIC_LEN, f_tmp) != MAGIC_LEN || memcmp(magic_buffer, MAGIC, MAGIC_LEN) != 0) {
        fprintf(stderr, "Error: Invalid MAGIC header\n");
        fclose(f_tmp);
        fclose(f_esso);
        tc_free(recovered_fname);
        return;
    }

    unsigned char data_byte, count_byte;
    _Bool byte_count = 1;

    while (fread(&data_byte, sizeof(data_byte), 1, f_tmp) == 1 &&
           fread(&count_byte, sizeof(count_byte), 1, f_tmp) == 1) {

        // 역연산 수행
        for(int rep = 3; rep < 97; rep++) {
            count_byte ^= XOR_KEYS[rep-3];
            count_byte ^= XOR_KEYS[rep];
            if (byte_count) count_byte = ~count_byte;
            count_byte = (count_byte >> SHIFT_BITS*2) | (count_byte << (8 - SHIFT_BITS*2));
            count_byte ^= XOR_KEYS[rep-2];
            count_byte ^= XOR_KEYS[rep-1];
        }


        for(int rep = 94; rep >= 0; rep--) {
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
                fwrite(&data_byte, sizeof(data_byte), 1, f_esso);
            }
        } else {
            fprintf(stderr, "==== WARN: Skipping invalid count_byte value: %d\n", count_byte);
        }
    }

    fclose(f_tmp);
    fclose(f_esso);
    char *original_name = tc_malloc(4096);
    memset(original_name,0,4096);
    if(strlen(recovered_fname) > 10 && strcmp(recovered_fname+strlen(recovered_fname)-10, ".ezip.esso") == 0) {
        strncpy(original_name,recovered_fname, strlen(recovered_fname) - 10);
    }
    if(rename(recovered_fname, original_name)) {
        fprintf(stderr, "==== LOG: filename recovery failed. see original_name.ezip.esso\n");
    }
    fprintf(stderr, "==== END: Coffee Recovery Magic function ====\n");
    tc_free(recovered_fname);
}

