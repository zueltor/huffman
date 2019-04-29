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
    unsigned char *code;
    unsigned int length;
};

unsigned int huffman(char *mode, char *in, char *out);

void encode(FILE *f1, FILE *f2);

void int_to_str(unsigned int n, unsigned char *buf, unsigned int mode);

void build_tree(node *p, unsigned int n);

node tree_merge(node p1, node p2);

void get_codes(codes *syn_codes, node *p, unsigned char *buf, unsigned int pos, unsigned char *treecode, unsigned int *i,
               unsigned char *alphabet, unsigned int *k);

void f_write_code(FILE *f1, FILE *f2, codes *sym_codes, unsigned char *buf);

void bin_text_to_char(unsigned char *s, unsigned int n, unsigned char *res);

void decode(FILE *f1, FILE *f2);

void code_to_tree(node *p, unsigned char *code, unsigned char *a, unsigned int *posa, unsigned int *posc);

unsigned int char_to_bin(unsigned char *buf, unsigned int c);

void print_help(void);