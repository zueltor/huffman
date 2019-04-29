#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

void decode(FILE *f1, FILE *f2) {
    node tree;
    unsigned int i = 0,
            n_text = 0,
            n_alphabet,
            j = 0,
            k,
            i_out_pos = 0;
    unsigned char c,
            bit_buf[8];

    unsigned char *buf = (unsigned char *) malloc(BUF_SIZE * sizeof(char));
    if (buf == NULL) {
        //printf("mem error");
        exit(1);
    }
    unsigned char *buf_out = (unsigned char *) malloc(BUF_SIZE * sizeof(char));
    if (buf_out == NULL) {
        //printf("mem error");
        exit(1);
    }
    if (fread(buf, sizeof(char), 4, f1) == 4)
        n_text = buf[3] + (buf[2] << 8u) + (buf[1] << 16u) + (buf[0] << 24u);
    //else
    //return;
    unsigned char *code = (unsigned char *) malloc(2 * N * sizeof(char));
    if (code == NULL) {
        //printf("mem error");
        exit(1);
    }
    fread(buf, sizeof(char), BUF_SIZE, f1);

    unsigned int k1 = 0,
            k0 = 0;
    do {
        c = buf[i];
        i++;
        char_to_bin(code + j, c);
        for (k = 0; k < 8; k++) {
            if (code[j + k] == '0')
                k0++;
            else k1++;
            if (k1 == k0 + 1)
                break;
        }
        j += 8;
    } while (k1 != k0 + 1);

    n_alphabet = k1;
    unsigned char *alphabet = (unsigned char *) malloc(N * sizeof(char));
    if (alphabet == NULL) {
        //printf("mem error");
        exit(1);
    }
    for (k = 0; k < n_alphabet; k++) {
        alphabet[k] = buf[i];
        i++;
    }

    k = 0;
    j = 0;
    code_to_tree(&tree, code, alphabet, &j, &k);
    free(code);
    free(alphabet);

    node *p;
    p = &tree;
    k = 0;
    j = 0;

    if (n_alphabet == 1) {
        c = p->sym;
        for (j = 0; j < BUF_SIZE; j++)
            buf_out[j] = c;
        while (n_text > BUF_SIZE) {
            fwrite(buf_out, sizeof(char), BUF_SIZE, f2);
            n_text -= BUF_SIZE;
        }
        fwrite(buf_out, sizeof(char), n_text, f2);
    } else
        while (k < n_text) {
            c = buf[i];
            i++;
            if (i == BUF_SIZE) {
                fread(buf, sizeof(char), BUF_SIZE, f1);
                i = 0;
            }
            char_to_bin(bit_buf, c);
            for (j = 0; j < 8; j++) {
                if (bit_buf[j] == '0') {
                    p = p->left;
                } else p = p->right;
                if (p->left == NULL) {
                    c = p->sym;
                    buf_out[i_out_pos] = c;
                    i_out_pos++;
                    if (i_out_pos == BUF_SIZE) {
                        fwrite(buf_out, sizeof(char), BUF_SIZE, f2);
                        i_out_pos = 0;
                    }
                    p = &tree;
                    k++;
                    if (k >= n_text)
                        break;
                }
            }
            j = 0;
        }
    fwrite(buf_out, sizeof(char), i_out_pos, f2);
    free(buf_out);
    free(buf);
}


void code_to_tree(node *p, unsigned char *code, unsigned char *a, unsigned int *posa, unsigned int *posc) {
    if (code[*posc] == '1') {
        p->left = NULL;
        p->right = NULL;
        p->sym = a[*posa];
        *posa += 1;;
        if (*posa == strlen(a))
            return;
    } else {
        p->left = (node *) malloc(sizeof(node));
        p->right = (node *) malloc(sizeof(node));
        *posc += 1;
        code_to_tree(p->left, code, a, posa, posc);
        *posc += 1;
        code_to_tree(p->right, code, a, posa, posc);
    }
}

unsigned int char_to_bin(unsigned char *buf, unsigned int c) {

    int i;
    unsigned int n = 0;
    for (i = 7; i >= 0; i--) {
        if (c % 2 == 0)
            buf[i] = '0';
        else {
            buf[i] = '1';
            n++;
        }
        c /= 2;
    }
    return n;
}



