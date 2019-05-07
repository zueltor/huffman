#include <stdio.h>
#include <string.h>
#include "header.h"

unsigned int huffman(char *mode, char *in, char *out) {
    FILE *f1 = fopen(in, "rb");
    FILE *f2 = fopen(out, "wb");

    if (f1 == NULL) {
        printf("error: %s cannot be opened", in);
        return 1;
    }
    if (f2 == NULL) {
        printf("error: %s cannot be opened", out);
        return 1;
    }

    if (strcmp(mode, "test") == 0) {
        fseek(f1, 0, SEEK_END);
        int size = ftell(f1);
        fseek(f1, 0, SEEK_SET);
        if (size == 3)
            return 0;
        unsigned char buf[4] = {0};

        fread(buf, sizeof(char), 3, f1);
        if (buf[0] == 'c')
            encode(f1, f2);
        if (buf[0] == 'd')
            decode(f1, f2);
    } else if (strcmp(mode, "-e") == 0) {
        encode(f1, f2);
        //printf("encoded");
    } else if (strcmp(mode, "-d") == 0) {
        decode(f1, f2);
        //printf("decoded");
    } else return 1;

    fclose(f1);
    fclose(f2);
    return 0;
}

void print_help(void) {
    printf("Huffman: -e | -d <inputfile> <outputfile>\n");
}

