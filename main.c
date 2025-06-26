#include "statdata.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <file1> <file2> <output_file>\n", argv[0]);
        return 1;
    }
    
    ProcessFiles(argv[1], argv[2], argv[3]);
    return 0;
}