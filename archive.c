#include "espresso_zip.h"
#include <sys/stat.h>
#include <stdio.h>
#include <zlib.h>
#include <gperftools/tcmalloc.h>

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
        b ^= XOR_KEY;
        b = (b << SHIFT_BITS) | (b >> (8 - SHIFT_BITS));
        if (byte_count) b = ~b;
        b ^= SUB_XOR_KEY;
        // byte 암호화

        count ^= XOR_KEY;
        count = (count << SHIFT_BITS) | (count >> (8 - SHIFT_BITS));
        if (byte_count) count = ~count;
        count ^= SUB_XOR_KEY;

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
