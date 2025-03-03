#include "espresso_zip.h"
#include <sys/stat.h>

FILE *original_file_open(const char *fname, const char *mode) {
    FILE *ret = fopen(fname, mode);
    if (ret == NULL) {
        fprintf(stderr, "LOG: Failed to open file. filename: %s, mode: %s\n", fname, mode);
    } else {
        fprintf(stderr, "LOG: the file has opened. filename: %s, mode: %s\n", fname, mode);
    }
    return ret;
}

void original_file_close(const char *fname, FILE *f) {
    if (f != NULL) {
        fclose(f);
        fprintf(stderr, "LOG: the file \"%s\" has closed.\n", fname);
    }
}
