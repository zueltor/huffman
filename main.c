#include <stdio.h>
#include "header.h"

int main(int argc, char *argv[]) {

    if (argc == 1) {
        return huffman("test", "in.txt", "out.txt");
    }

    if (argc != 4) {
        print_help();
        return 1;
    }
    return huffman(argv[1], argv[2], argv[3]);
}
