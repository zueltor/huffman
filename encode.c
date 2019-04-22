#define _CRT_SECURE_NO_WARNINGS
#define BUF_SIZE 1024
#define N 256
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "sort.h"

void encode(FILE *f1, FILE *f2)
{
	unsigned int i,
		j,
		n,
		cur_pos,
		r;

	cur_pos = ftell(f1);
	n = 0;

	unsigned int *a = (int*)malloc(N * sizeof(int));
	unsigned char *buf = (char*)malloc(BUF_SIZE * sizeof(char));
	memset(a, 0, N * sizeof(int));
	while ((r = fread(buf, sizeof(char), BUF_SIZE, f1)) && (r > 0))
	{
		for (i = 0; i < r; i++)
		{
			a[buf[i]]++;
			n++;
		}
	}

	if (n == 0)
		return;

	int_to_str(n, buf, 1);
	fwrite(buf, sizeof(char), 4, f2);
	n = 0;

	for (i = 0; i < N; i++)
	{
		if (a[i] > 0)
			n++;
	}


	node *tree = (node*)malloc(n * sizeof(node));
	codes *leaf = (codes*)malloc(N * sizeof(codes));
	j = 0;
	for (i = 0; i < N; i++)
	{
		leaf[i].code = (char*)malloc(sizeof(char) * 32);
		if (a[i] > 0)
		{
			tree[j].freq = a[i];
			tree[j].sym = i;
			tree[j].left = NULL;
			tree[j].right = NULL;
			j++;
		}
	}
	char *alphabet = (char*)malloc(n * sizeof(char));
	psort(tree, n);
	build_tree(tree, n);


	unsigned char treecode[2*N] = { 0 };

	n = 0;
	i = 0;
	get_codes(leaf, tree, buf, 0, treecode, &i, alphabet, &n);

	free(tree);
	fseek(f1, cur_pos, SEEK_SET);

	bin_to_char(treecode, i, buf);
	i = (i % 8) ? i / 8 + 1 : i / 8;

	fwrite(buf, sizeof(char), i, f2);
	fwrite(alphabet, sizeof(char), j, f2);
	free(alphabet);

	fout_code(f1, f2, leaf, buf);
	free(buf);
	free(leaf);
}

void int_to_str(unsigned int n, unsigned char *buf, unsigned int mode)
{
	if (mode)
	{
		buf[0] = (n >> 24) & 0xFF;
		buf[1] = (n >> 16) & 0xFF;
		buf[2] = (n >> 8) & 0xFF;
		buf[3] = n & 0xFF;
	}
	else buf[0] = n & 0xFF;
}

void build_tree(node *p, unsigned int n)
{
	unsigned int i;
	int j;
	i = n - 2;
	while (i < n)
	{
		p[i] = tree_merge(p[i], p[i + 1]);
		i--;
		j = i;
		while ((p[j].freq < p[j + 1].freq) && j >= 0)
		{
			node tmp = p[j];
			p[j] = p[j + 1];
			p[j + 1] = tmp;
			j--;

		}
	}
}

node tree_merge(node p1, node p2)
{
	node p;
	p.left = (node*)malloc(sizeof(node));
	p.right = (node*)malloc(sizeof(node));
	*p.left = p1;
	*p.right = p2;
	p.freq = p1.freq + p2.freq;
	return p;
}

void get_codes(codes *leaf, node *p, unsigned char *buf, unsigned int pos, unsigned char *treecode, unsigned int *i, unsigned char *alphabet, unsigned int *k)
{
	if (p->left == NULL)
	{
		treecode[*i] = '1';
		*i += 1;
		unsigned char *s = (char*)malloc(sizeof(char) * 32);
		bin_to_char(buf, pos, s);
		for (unsigned int i = 0; i <= pos / 8; i++)
		{
			leaf[p->sym].code[i] = s[i];
		}
		leaf[p->sym].length = pos;
		alphabet[*k] = p->sym;
		*k += 1;
		return;
	}
	buf[pos] = '0';
	treecode[*i] = '0';
	*i = *i + 1;
	get_codes(leaf, p->left, buf, pos + 1, treecode, i, alphabet, k);
	buf[pos] = '1';
	get_codes(leaf, p->right, buf, pos + 1, treecode, i, alphabet, k);
}

void fout_code(FILE *f1, FILE *f2, codes *leaf, unsigned char *buf)
{
	unsigned char c,
		c_out,
		sym;

	unsigned int i, 
				bit_pos, 
				bit_length,
				j,
				r,
				k,
				i_out_pos;
	i_out_pos = 0;
	bit_pos = 0;
	j = 0;
	k = 0;
	c_out = 0;
	unsigned char* buf_out = (char*)malloc(BUF_SIZE * sizeof(char));

	while ((r = fread(buf, sizeof(char), BUF_SIZE, f1)) && (r > 0))
	{
		for (i = 0; i < r; i++)
		{
			c = buf[i];
			bit_length = leaf[(unsigned int)c].length;
			while (bit_length > 0)
			{
				if (k >= bit_pos)
				{
					sym = leaf[(unsigned int)c].code[j] << bit_pos;
					c_out = c_out + (sym >> k);
					if (bit_length < 8 - k)
					{
						k = k + bit_length;
						bit_length = 0;
						bit_pos = 0;
						j = 0;
						continue;
					}
					else
					{

						bit_pos = (8 - k + bit_pos) % 8;
						bit_length = bit_length - 8 + k;
						k = 0;
						if (bit_pos == 0)
							j++;
						if (bit_length == 0)
						{
							j = 0;
							bit_pos = 0;
						}
					}

				}
				else if (k < bit_pos)
				{
					sym = leaf[(unsigned int)c].code[j] << bit_pos;
					c_out = c_out + (sym >> k);
					if (8 - bit_pos < bit_length)
					{
						k = k + 8 - bit_pos;
						bit_length = bit_length - (8 - bit_pos);
						bit_pos = 0;
						sym = leaf[(unsigned int)c].code[++j] >> (k);
						c_out = c_out + (sym);

						if (8 - k <= bit_length)
						{
							bit_pos = 8 - k;
							bit_length = bit_length - bit_pos;
							k = 0;
							if (bit_length == 0)
							{
								j = 0;
								bit_pos = 0;
							}

						}
						else
						{
							k = k + bit_length;
							bit_pos = 0;
							bit_length = 0;
							j = 0;
							continue;
						}
					}
					else
					{
						j = 0;
						k = k + bit_length;
						bit_pos = 0;
						bit_length = 0;
						continue;
					}
				}
				buf_out[i_out_pos] = c_out;
				i_out_pos++;
				if (i_out_pos == BUF_SIZE)
				{
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
	free(buf_out);

}
