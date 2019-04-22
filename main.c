#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "header.h"




/*
int main(int argc, char *argv[])
{

	if (argc != 4)
	{
		print_help();
		return 1;
	}
	if (huffman(argv[1], argv[2], argv[3]))
		return 1;

	return 0;

}
*/


int main(void)
{
	FILE *f1 = fopen("in.txt", "rb");
	FILE *f2 = fopen("out.txt", "wb");
	if (f1 == NULL)
	{
		return 1;
	}
	if (f2 == NULL)
	{
		return 1;
	}
	fseek(f1, 0, SEEK_END);
	int size = ftell(f1);
	fseek(f1, 0, SEEK_SET);
	if (size == 3)
		return 0;
	unsigned char buf[4] = { 0 };

	fread(buf, sizeof(char), 3, f1);
	if (buf[0] == 'c')
		encode(f1, f2);
	if (buf[0] == 'd')
		decode(f1, f2);
	fclose(f1);
	fclose(f2);
	return 0;

}
