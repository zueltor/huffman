#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include "header.h"

unsigned int huffman(char *mode, char *in, char *out) {
    FILE *f1 = fopen(in, "rb");
    FILE *f2 = fopen(out, "wb");

    if (f1 == NULL) {
        printf("error %s", in);
        return 1;
    }
    if (f2 == NULL) {
        printf("error %s", out);
        return 1;
    }

    if (strcmp(mode, "-e") == 0) {
        encode(f1, f2);
        printf("encoded");
    } else if (strcmp(mode, "-d") == 0) {
        decode(f1, f2);
        printf("decoded");
    } else return 1;

    fclose(f1);
    fclose(f2);
    return 0;
}


void bin_to_char(unsigned char *s, unsigned int n, unsigned char *res) {
    unsigned char c = 0;
    unsigned int i,
            k = 0;
    while (n > 7) {
        for (i = 0; i < 8; i++)
            c = (c << 1) + s[k * 8 + i] - 48;
        res[k] = c;
        k++;
        n = n - 8;
    }
    res[k] = last_char(s + k * 8, n);
}

unsigned char last_char(unsigned char *s, unsigned int n) {
    unsigned int i;
    unsigned char c = 0;
    for (i = 0; i < n; i++)
        c = (c << 1) + s[i] - 48;
    for (i = n; i < 8; i++)
        c = (c << 1);
    return c;
}

void print_help(void) {
    printf("Huffman: -e | -d <inputfile> <outputfile>\n");
}

