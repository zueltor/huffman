#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "sort.h"

void encode(FILE *f1, FILE *f2) {
    unsigned int i,
            j,
            n,
            cur_pos,
            r;

    cur_pos = ftell(f1);
    n = 0;

    unsigned int frequencies[N] = {0};
    unsigned char buf[BUF_SIZE];

    while ((r = fread(buf, sizeof(char), BUF_SIZE, f1)) && (r > 0)) {
        for (i = 0; i < r; i++) {
            frequencies[buf[i]]++;
            n++;
        }
    }

    if (n == 0)
        return;

    buf[0] = (n >> 24) & 0xFF;
    buf[1] = (n >> 16) & 0xFF;
    buf[2] = (n >> 8) & 0xFF;
    buf[3] = n & 0xFF;

    fwrite(buf, sizeof(char), 4, f2);

    n = 0;

    for (i = 0; i < N; i++)
        if (frequencies[i] > 0)
            n++;

    node *tree = (node *) malloc(n * sizeof(node));

    codes sym_codes[N];

    j = 0;
    for (i = 0; i < N; i++)
        if (frequencies[i] > 0) {
            tree[j].freq = frequencies[i];
            tree[j].sym = i;
            tree[j].left = NULL;
            tree[j].right = NULL;
            j++;
        }

    psort(tree, n);
    build_tree(tree, n);


    unsigned char tree_code[N] = {0};
    unsigned char *alphabet = (unsigned char *) malloc(n * sizeof(char));
    n = 0;
    i = 0;

    memset(buf, 0, BUF_SIZE * sizeof(char));
    get_codes(sym_codes, tree, buf, 0, tree_code, &i, alphabet, &n);
    free(tree);
    fseek(f1, cur_pos, SEEK_SET);

    i = (i % 8) ? i / 8 + 1 : i / 8;

    fwrite(tree_code, sizeof(char), i, f2);
    fwrite(alphabet, sizeof(char), j, f2);
    free(alphabet);

    f_write_code(f1, f2, sym_codes, buf);
}

void build_tree(node *p, unsigned int n) {
    unsigned int i;
    int j;
    if (n < 2)
        return;
    i = n - 2;
    while (i < n) {
        p[i] = tree_merge(&p[i], &p[i + 1]);
        i--;
        j = i;
        while ((p[j].freq < p[j + 1].freq) && j >= 0) {
            node tmp = p[j];
            p[j] = p[j + 1];
            p[j + 1] = tmp;
            j--;
        }
    }
}

node tree_merge(node *p1, node *p2) {
    node p;
    p.left = (node *) malloc(sizeof(node));
    p.right = (node *) malloc(sizeof(node));
    *(p.left) = *p1;
    *(p.right) = *p2;
    p.freq = p1->freq + p2->freq;
    return p;
}

void get_codes(codes *sym_codes, node *p, unsigned char *buf, unsigned int bit_pos, unsigned char *tree_code,
               unsigned int *i, unsigned char *alphabet, unsigned int *j) {
    if (p->left == NULL) {
        tree_code[(*i) / 8] += (0x80 >> ((*i) % 8));
        *i += 1;
        memcpy(sym_codes[p->sym].code, buf, (bit_pos / 8 + 1) * sizeof(char));
        sym_codes[p->sym].length = bit_pos;
        alphabet[*j] = p->sym;
        *j += 1;
        return;
    }
    char c = buf[bit_pos / 8];
    *i += 1;
    get_codes(sym_codes, p->left, buf, bit_pos + 1, tree_code, i, alphabet, j);

    buf[bit_pos / 8] = c + (0x80 >> (bit_pos % 8));
    get_codes(sym_codes, p->right, buf, bit_pos + 1, tree_code, i, alphabet, j);
}

void f_write_code(FILE *f1, FILE *f2, codes *sym_codes, unsigned char *buf) {
    unsigned char c,
            c_out = 0,
            sym;
    unsigned int i,
            bit_pos = 0,
            bit_length,
            j = 0,
            r,
            k = 0,
            i_out_pos = 0;

    unsigned char buf_out[BUF_SIZE];

    while ((r = fread(buf, sizeof(char), BUF_SIZE, f1)) && (r > 0)) {
        for (i = 0; i < r; i++) {
            c = buf[i];
            bit_length = sym_codes[(unsigned int) c].length;
            while (bit_length > 0) {
                if (k >= bit_pos) {
                    sym = sym_codes[(unsigned int) c].code[j] << bit_pos;
                    c_out = c_out + (sym >> k);
                    if (bit_length < 8 - k) {
                        k = k + bit_length;
                        bit_length = 0;
                        bit_pos = 0;
                        j = 0;
                        continue;
                    } else {
                        bit_pos = (8 - k + bit_pos) % 8;
                        bit_length = bit_length - 8 + k;
                        k = 0;
                        if (bit_pos == 0)
                            j++;
                        if (bit_length == 0) {
                            j = 0;
                            bit_pos = 0;
                        }
                    }

                } else if (k < bit_pos) {
                    sym = sym_codes[(unsigned int) c].code[j] << bit_pos;
                    c_out = c_out + (sym >> k);
                    if (8 - bit_pos < bit_length) {
                        k = k + 8 - bit_pos;
                        bit_length = bit_length - (8 - bit_pos);
                        sym = sym_codes[(unsigned int) c].code[++j] >> (k);
                        c_out = c_out + (sym);
                        if (8 - k <= bit_length) {
                            bit_pos = 8 - k;
                            bit_length = bit_length - bit_pos;
                            k = 0;
                            if (bit_length == 0) {
                                j = 0;
                                bit_pos = 0;
                            }

                        } else {
                            k = k + bit_length;
                            bit_pos = 0;
                            bit_length = 0;
                            j = 0;
                            continue;
                        }
                    } else {
                        j = 0;
                        k = k + bit_length;
                        bit_pos = 0;
                        bit_length = 0;
                        continue;
                    }
                }
                buf_out[i_out_pos] = c_out;
                i_out_pos++;
                if (i_out_pos == BUF_SIZE) {
                    fwrite(buf_out, sizeof(char), BUF_SIZE, f2);
                    memset(buf_out, 0, BUF_SIZE);
                    i_out_pos = 0;
                }
                c_out = 0;
            }
        }
    }
    fwrite(buf_out, sizeof(char), i_out_pos, f2);
    if (k)
        fwrite(&c_out, sizeof(char), 1, f2);
}
