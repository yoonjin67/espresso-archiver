#ifndef COFFEE_H
#define COFFEE_H
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *original_file_open(const char *fname, const char *mode);
void coffee_extract(const char *fname, FILE *fp);
void coffee_recovery_magic(FILE *f_essp, const char *fname);
void original_file_close(const char *fname, FILE *f);

#define SHIFT_BITS 3  // 비트 시프트 크기
#define XOR_LEN   256
#define MAGIC_LEN 4
// --암호화 키 관련 코드 구현 종료


//디플레이트 청크 크기
#define DEFLATE_CHUNK 16384
#define INFLATE_CHUNK DEFLATE_CHUNK
