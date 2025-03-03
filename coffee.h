#ifndef COFFEE_H
#define COFFEE_H
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *original_file_open(const char *fname, const char *mode);
void coffee_extract(const char *fname, FILE *fp);
void coffee_recovery_magic(FILE *fbean, FILE *fwater, const char *fname);
void original_file_close(const char *fname, FILE *f);
