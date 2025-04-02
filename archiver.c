#include "espresso_zip.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "This hasn't got filename(s)!\n");
        return 1; // Indicate an error
    }
    for (int i = 1; i < argc; i++) {
      FILE *input = fopen(argv[i], "rb");
      if(input == NULL) {
          fprintf(stderr, "Error opening file: %s\n", argv[i]);
          continue;
      }
      coffee_extract(argv[i], input);
      fclose(input);
    }
    return 0; // Indicate success
}
