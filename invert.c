#include <stdio.h>

int main(void)
{
	int c;
	while ((c = getchar()) != EOF) {
		c = ~c;
		putchar(c);
	}
}
