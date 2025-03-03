#include "coffee.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "This hasn't got filename(s)!\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        char bean_fname[4096];
        char water_fname[4096];

        snprintf(bean_fname, sizeof(bean_fname), "%s.bean", argv[i]);
        snprintf(water_fname, sizeof(water_fname), "%s.water", argv[i]);

        FILE *fbean = original_file_open(bean_fname, "rb");
        FILE *fwater = original_file_open(water_fname, "rb");

        if(fbean == NULL || fwater == NULL) {
            fprintf(stderr, "Error opening bean or water file for %s\n", argv[i]);
            if(fbean != NULL) fclose(fbean);
            if(fwater != NULL) fclose(fwater);
            continue;
        }

        coffee_recovery_magic(fbean, fwater, argv[i]);
        fclose(fwater);
        fclose(fbean);
    }
    return 0;
}
