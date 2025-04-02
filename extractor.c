#include "espresso_zip.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "This hasn't got filename(s)!\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
      FILE *f_essp = original_file_open(argv[i], "rb"); //파일 읽기

      if(f_essp == NULL) {
          fprintf(stderr, "Error opening espresso archive%s\n", argv[i]);
          if(f_essp != NULL) fclose(f_essp);
          continue;
      }

      coffee_recovery_magic(f_essp, argv[i]);
      fclose(f_essp);
    }
    return 0;
}
