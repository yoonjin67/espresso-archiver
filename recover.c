#include "coffee.h"
#include <sys/stat.h>

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
