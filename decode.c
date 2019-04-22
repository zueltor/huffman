#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

void decode(FILE *f1, FILE *f2)
{
	node tree;
	char alphabet[256] = { 0 };
	char code[512] = { 0 };
	unsigned char buf[9] = { 0 };
	unsigned int i,
		n_text,
		n_alphabet,
		k1,
		k0,
		j;
	unsigned char c;
	i = 0;

	if (fread(buf, sizeof(char), 4, f1)==4)
		n_text = buf[3] + (buf[2] << 8) + (buf[1] << 16) + (buf[0] << 24);
	else 
		return;
	k1 = 0;
	k0 = 0;
	j = 0;

	do
	{
		fread(&c, sizeof(char), 1, f1);
		char_to_bin(code + j, c);
		for (i = 0; i < 8; i++)
		{
			if (code[j + i] == '0')
				k0++;
			else k1++;
			if (k1 == k0 + 1)
				break;
		}
		j += 8;
	} while (k1 != k0 + 1);

	n_alphabet = k1;

	for (i = 0; i < n_alphabet; i++)
	{
		fread(&c, sizeof(char), 1, f1);
		alphabet[i] = c;
	}

	i = 0;
	j = 0;

	code_to_tree(&tree, code, alphabet, &j, &i);
	node *p;
	p = &tree;
	i = 0;

	if (n_alphabet == 1)
	{
		c = p->sym;
		for (i = 0; i < n_text; i++)
		{
			fwrite(&c, sizeof(char), 1, f2);
		}
	}
	else
		while (i < n_text)
		{
			fread(&c, sizeof(char), 1, f1);
			char_to_bin(buf, c);
			for (j = 0; j < 8; j++)
			{
				if (buf[j] == '0')
				{
					p = p->left;
				}
				else p = p->right;
				if (p->left == NULL)
				{
					c = p->sym;
					fwrite(&c, sizeof(char), 1, f2);
					p = &tree;
					i++;
					if (i >= n_text)
						break;
				}
			}
			j = 0;
		}
}


void code_to_tree(node *p, unsigned char *code, unsigned char *a, unsigned int *posa, unsigned int *posc)
{
	if (*(code + *posc) == '1')
	{
		p->left = NULL;
		p->right = NULL;
		p->sym = a[*posa];
		*posa = *posa + 1;;
		if (*posa == strlen(a))
			return;
	}
	else
	{
		p->left = (node*)malloc(sizeof(node));
		p->right = (node*)malloc(sizeof(node));
		*posc = *posc + 1;
		code_to_tree(p->left, code, a, posa, posc);
		*posc = *posc + 1;
		code_to_tree(p->right, code, a, posa, posc);
	}
	return;
}

unsigned int char_to_bin(unsigned char *buf, unsigned int c)
{

	int i;
	unsigned int n = 0;
	for (i = 7; i >= 0; i--)
	{
		if (c % 2 == 0)
			buf[i] = '0';
		else
		{
			buf[i] = '1';
			n++;
		}
		c /= 2;
	}
	return n;
}



