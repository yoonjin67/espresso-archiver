#include "espresso_zip.h"
#include <sys/stat.h>
#include <stdio.h>
#include <zlib.h>
#include <gperftools/tcmalloc.h>

unsigned char XOR_KEYS[256] = {102, 2, 176, 55, 42, 22, 250, 196, 85, 206, 197, 121, 66, 61, 86, 134, 211, 178, 243, 161, 23, 244, 49, 48, 39, 62, 242, 180, 109, 229, 248, 247, 182, 64, 145, 150, 114, 100, 174, 117, 98, 99, 154, 120, 149, 217, 52, 225, 70, 190, 94, 132, 27, 90, 124, 222, 183, 74, 88, 164, 152, 202, 10, 253, 53, 194, 224, 158, 103, 82, 6, 240, 28, 93, 40, 13, 30, 146, 8, 14, 34, 32, 168, 131, 219, 162, 157, 56, 21, 104, 122, 33, 234, 67, 5, 143, 175, 12, 16, 58, 148, 17, 133, 187, 128, 91, 186, 110, 106, 79, 127, 113, 24, 201, 129, 170, 76, 3, 118, 216, 220, 111, 185, 155, 84, 199, 160, 204, 1, 38, 51, 147, 144, 75, 105, 214, 81, 254, 235, 172, 78, 41, 83, 0, 46, 59, 230, 173, 4, 195, 212, 25, 112, 29, 57, 163, 179, 89, 19, 95, 92, 97, 228, 71, 7, 123, 237, 26, 68, 210, 44, 251, 226, 54, 45, 73, 184, 171, 159, 137, 236, 188, 140, 65, 136, 192, 166, 193, 119, 125, 36, 18, 107, 50, 37, 246, 9, 96, 141, 208, 31, 181, 165, 80, 231, 252, 177, 215, 135, 156, 169, 151, 200, 203, 77, 255, 142, 69, 241, 191, 205, 153, 138, 15, 198, 63, 167, 227, 207, 115, 245, 218, 101, 87, 233, 238, 239, 72, 232, 20, 126, 223, 189, 139, 130, 249, 60, 11, 43, 35, 116, 221, 47, 209, 213, 108};



void *tcmalloc_alloc(void *opaque, unsigned items, unsigned size) {
    return tc_malloc(items * size);
}

void tcmalloc_free(void *opaque, void *address) {
    tc_free(address);
}

const char *MAGIC = "ESPL";

void coffee_extract(const char *fname, FILE *fp) {
    fprintf(stderr, "==== START: Coffee Extract function ====\n");

    char esso_fname[4096];
    char ezip_fname[4096];

    // 파일 확장자 붙이기
    snprintf(esso_fname, sizeof(esso_fname), "%s.esso", fname);
    snprintf(ezip_fname, sizeof(ezip_fname), "%s.ezip", fname);

    FILE *f_esso = original_file_open(esso_fname, "wb");
    FILE *f_ezip = original_file_open(ezip_fname, "wb");

    if (f_esso == NULL || fp == NULL || f_ezip == NULL) {
        if (f_esso != NULL) original_file_close(esso_fname, f_esso);
        fprintf(stderr, "==== END: Extract function (failed due to file open errors) ====\n");
        return;
    }

    fwrite(MAGIC, sizeof(char), MAGIC_LEN, f_esso);

    int c;
    _Bool byte_count = 1;
    // 바이트 카운트가 1일때만 NOT 연산.
    while ((c = fgetc(fp)) != EOF) {
        unsigned char b = (unsigned char)c;
        unsigned char count = 1;

        int next_c;
        while ((next_c = fgetc(fp)) != EOF && (unsigned char)next_c == b) {
            count++;
        }
        if (next_c != EOF) {
            ungetc(next_c, fp);
        }

        // 간단한 RLE 구현을 위해 count 횟수 계산
        for(int rep = 0; rep < XOR_LEN-3; rep++) {
            b ^= XOR_KEYS[rep];
            b ^= XOR_KEYS[rep+2];
            b = (b << SHIFT_BITS) | (b >> (8 - SHIFT_BITS));
            b^=XOR_KEYS[rep+1];
            if (!byte_count) b = ~b;
            b=XOR_KEYS[b];
        }
        // byte 암호화
        for(int rep = XOR_LEN - 1; rep >= 3; rep--) {
            count ^= XOR_KEYS[rep-1];
            count ^= XOR_KEYS[rep-2];
            count = (count << SHIFT_BITS*2) | (count >> (8 - SHIFT_BITS*2));
            if (byte_count) count = ~count;
            count ^= XOR_KEYS[rep];
            count ^= XOR_KEYS[rep-3];
            count=XOR_KEYS[count];
        }

        byte_count = !byte_count;

        fwrite(&b, sizeof(b), 1, f_esso);
        fwrite(&count, sizeof(count), 1, f_esso);
    }


    // --디플레이트 함수 불러오기 (zlib 필요)
    fprintf(stderr, "==== START: Deflate Algorithm(zlib) ====\n");
    z_stream deflate_stream = {0};
    deflate_stream.zalloc = tcmalloc_alloc;
    deflate_stream.zfree  = tcmalloc_free;
    deflate_stream.opaque = Z_NULL;
    
    int ret = deflateInit(&deflate_stream, Z_BEST_COMPRESSION);
    if (ret != Z_OK) {
        fprintf(stderr, "==== ERROR: Deflate(init) failed! ret = %d\n", ret);
        return;
    }
    
    // 파일 다시 열기
    original_file_close(esso_fname, f_esso);
    f_esso = original_file_open(esso_fname, "rb");
    if (f_esso == NULL) {
        fprintf(stderr, "==== ERROR: Failed to reopen file for deflate ====\n");
        fclose(f_ezip);
        return;
    }
    
    // 파일 포인터를 0으로 되돌림
    fflush(f_esso);
    fseek(f_esso, 0, SEEK_SET);
    
    unsigned char in[DEFLATE_CHUNK];
    unsigned char out[DEFLATE_CHUNK];
    
    int flush;
    do {
        deflate_stream.avail_in = fread(in, 1, DEFLATE_CHUNK, f_esso);
        if (deflate_stream.avail_in == 0) {
            if (feof(f_esso)) {
                flush = Z_FINISH;
            } else {
                fprintf(stderr, "==== ERROR: fread failed during deflate ====\n");
                break;
            }
        } else {
            flush = feof(f_esso) ? Z_FINISH : Z_NO_FLUSH;
        }
        deflate_stream.next_in = in;
    
        do {
            deflate_stream.avail_out = DEFLATE_CHUNK;
            deflate_stream.next_out = out;
            ret = deflate(&deflate_stream, flush);
    
            size_t have = DEFLATE_CHUNK - deflate_stream.avail_out;
            if (have > 0) {
                if (fwrite(out, 1, have, f_ezip) != have) {
                    fprintf(stderr, "==== ERROR: fwrite failed during deflate ====\n");
                    deflateEnd(&deflate_stream);
                    fclose(f_ezip);
                    fclose(f_esso);
                    return;
                }
            }
        } while (deflate_stream.avail_out == 0);
    
    } while (flush != Z_FINISH);
    
    original_file_close(esso_fname, f_esso);
    deflateEnd(&deflate_stream);
    fclose(f_ezip);
    fprintf(stderr, "==== END: Deflate Algorithm(zlib) ====\n");
    
    // 임시 파일 삭제
    if (unlink(esso_fname) == 0) {
        fprintf(stderr, "==== INFO: temporary file (*.esso) has cleared. ====\n");
    } else {
        fprintf(stderr, "==== WARN: temporary file (*.esso) remove failed. ====\n");
    }
    
        fprintf(stderr, "==== END: Archive function ====\n");
    }
