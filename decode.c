#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

void decode(FILE *f1, FILE *f2) {
    node tree;
    unsigned int i = 0,
            n_text = 0,
            j = 0,
            k;
    unsigned char buf[BUF_SIZE];
    unsigned char buf_out[BUF_SIZE];

    if (fread(buf, sizeof(char), 4, f1) == 4)
        n_text = buf[3] + (buf[2] << 8) + (buf[1] << 16) + (buf[0] << 24);

    fread(buf, sizeof(char), BUF_SIZE, f1);

    k = 0;
    j = 0;
    code_to_tree(&tree, buf, &k, &j);
    k++;
    dfs(&tree, buf, &k);

    node *p;
    p = &tree;
    j = 0;
    i = 0;
    unsigned int l = 0;
    while (i < n_text) {
        while (p->left != NULL) {
            if (buf[k] & (0x80 >> j)) {
                p = p->right;
            } else p = p->left;
            j++;
            if (j == 8) {
                j = 0;
                k++;
            }
            if (k == BUF_SIZE) {
                fread(buf, sizeof(char), BUF_SIZE, f1);
                k = 0;
            }
        }
        buf_out[l] = p->sym;
        l++;
        p = &tree;
        i++;
        if (l == BUF_SIZE) {
            fwrite(buf_out, sizeof(char), BUF_SIZE, f2);
            l = 0;
        }
    }
    fwrite(buf_out, sizeof(char), l, f2);
}

void code_to_tree(node *tree, unsigned char *buf, unsigned int *k, unsigned int *j) {
    if ((buf[*k] & (0x80 >> (*j))) == 0) {
        *j += 1;
        if (*j == 8) {
            *j = 0;
            *k += 1;
        }
        tree->left = malloc(sizeof(node));
        tree->right = malloc(sizeof(node));
        code_to_tree(tree->left, buf, k, j);
    } else {
        tree->left = NULL;
        tree->right = NULL;
        return;
    }
    *j += 1;
    if (*j == 8) {
        *j = 0;
        *k += 1;
    }
    code_to_tree(tree->right, buf, k, j);
}

void dfs(node *tree, unsigned char *buf, unsigned int *k) {
    if (tree->left == NULL) {
        tree->sym = buf[*k];
        *k += 1;
        return;
    } else {
        dfs(tree->left, buf, k);
        dfs(tree->right, buf, k);

    }

}




