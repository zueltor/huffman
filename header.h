#pragma once
#define BUF_SIZE 1024
#define N 256

typedef struct node node;
struct node {
    node *left;
    node *right;
    unsigned char sym;
    size_t freq;
};

typedef struct codes codes;
struct codes {
    unsigned char code[32];
    unsigned int length;
};

unsigned int huffman(char *mode, char *in, char *out);

void encode(FILE *f1, FILE *f2);

void build_tree(node *p, unsigned int n);

node tree_merge(node *p1, node *p2);

void get_codes(codes *sym_codes, node *p, unsigned char *buf, unsigned int bit_pos, unsigned char *tree_code,
               unsigned int *i, unsigned char *alphabet, unsigned int *k);

void f_write_code(FILE *f1, FILE *f2, codes *sym_codes, unsigned char *buf);

void decode(FILE *f1, FILE *f2);

void dfs(node *tree, unsigned char *buf, unsigned int *k);

void code_to_tree(node *tree, unsigned char *buf, unsigned int *k, unsigned int *j);

void print_help(void);