#include "espresso_zip.h"
#include <sys/stat.h>
#include <stdio.h>
#include <zlib.h>
#include <gperftools/tcmalloc.h>

int XOR_KEYS[97] = 
{
389, 379, 67, 347, 431, 491, 19, 499, 107, 113, 229, 307, 503, 71, 461, 179, 397, 2, 11, 509, 103, 281, 29, 373, 337, 353, 7, 317, 359, 479, 311, 421, 271, 443, 457, 157, 227, 61, 59, 41, 97, 139, 31, 109, 37, 43, 251, 199, 263, 3, 151, 409, 367, 191, 127, 463, 197, 269, 313, 17, 13, 163, 137, 181, 211, 167, 467, 487, 433, 331, 349, 101, 239, 383, 79, 53, 223, 73, 83, 5, 277, 193, 23, 47, 131, 401, 233, 149, 439, 449, 419, 89, 173, 241, 257, 293, 283
};


void *tcmalloc_alloc(void *opaque, unsigned items, unsigned size) {
    return tc_malloc(items * size);
}

void tcmalloc_free(void *opaque, void *address) {
    tc_free(address);
}

const char *MAGIC = "ESPL";

void coffee_extract(const char *fname, FILE *fp) {
    fprintf(stderr, "==== START: Coffee Extract function (file splitter) ====\n");

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
        for(int rep = 0; rep < 96; rep++) {
            b ^= XOR_KEYS[rep];
            b = (b << SHIFT_BITS) | (b >> (8 - SHIFT_BITS));
            b^=XOR_KEYS[rep+1];
            if (!byte_count) b = ~b;
        }
        // byte 암호화
        for(int rep = 96; rep >= 1; rep--) {
            count ^= XOR_KEYS[rep-1];
            count = (count << SHIFT_BITS*2) | (count >> (8 - SHIFT_BITS*2));
            if (byte_count) count = ~count;
            count ^= XOR_KEYS[rep];
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
