#include "coffee.h"
#include<sys/stat.h>
#include<stdio.h>


void coffee_extract(const char *fname, FILE *fp) {
    fprintf(stderr, "==== START: Coffee Extract function (file splitter) ====\n");

    char bean_fname[4096];
    char water_fname[4096];

    snprintf(bean_fname, sizeof(bean_fname), "%s.bean", fname);
    snprintf(water_fname, sizeof(water_fname), "%s.water", fname);

    FILE *fbean = original_file_open(bean_fname, "wb");
    FILE *fwater = original_file_open(water_fname, "wb");

    if (fbean == NULL || fwater == NULL || fp == NULL) {
        if(fbean != NULL) original_file_close(bean_fname, fbean);
        if(fwater != NULL) original_file_close(water_fname, fwater);
        fprintf(stderr, "==== END: Coffee Extract function (failed due to file open errors) ====\n");
        return;
    }

    int c;
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

        fwrite(&b, sizeof(b), 1, fwater);
        fwrite(&count, sizeof(count), 1, fbean);
    }

    original_file_close(water_fname, fwater);
    original_file_close(bean_fname, fbean);
    fprintf(stderr, "==== END: Coffee Extract function ====\n");
}

void coffee_recovery_magic(FILE *fbean, FILE *fwater, const char *fname) {
    fprintf(stderr, " ==== START: Coffee Recovery Magic function(recover file into original state) ====\n");

    struct stat buffer;
    char recovered_fname[4096];
    snprintf(recovered_fname, sizeof(recovered_fname), "%s.recover", fname);

    FILE *recovered_file = original_file_open(recovered_fname, "wb");
    if (recovered_file == NULL || fbean == NULL || fwater == NULL) {
        fprintf(stderr, "==== END: Coffee Recovery Magic function (failed due to file open errors) ====\n");
        return;
    }

    unsigned char t, count;
    while (fread(&t, sizeof(t), 1, fwater) == 1 && fread(&count, sizeof(count), 1, fbean) == 1) {
        for (int i = 0; i < count; i++) {
            fwrite(&t, sizeof(t), 1, recovered_file);
        }
    }

    original_file_close(recovered_fname, recovered_file);
    fprintf(stderr, "==== END: Coffee Recovery Magic function ====\n");
}
