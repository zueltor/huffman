#include <stdio.h>
#include "header.h"
#include "sort.h"

void sift(node *p, unsigned int i, unsigned int n)
{
	unsigned int l,
		r,
		k;
	i++;
	while ((l = 2 * i) <= n)
	{
		r = (l + 1 <= n) ? l + 1 : i;
		if ((p[i - 1].freq <= p[l - 1].freq) && (p[i - 1].freq <= p[r - 1].freq))
			return;
		k = (p[l - 1].freq <= p[r - 1].freq) ? l : r;
		node tmp = p[i - 1];
		p[i - 1] = p[k - 1];
		p[k - 1] = tmp;
		i = k;
	}
}
void psort(node *p, unsigned int n)
{
	int i;

	for (i = n / 2; i >= 0; i--)
	{
		sift(p, i, n);
	}
	for (i = n - 1; i > 0; i--)
	{

		node tmp = p[0];
		p[0] = p[i];
		p[i] = tmp;
		sift(p, 0, i);
	}
}