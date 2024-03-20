#include <stdio.h>
#include <stdint.h>

static
uint8_t reverse(uint8_t b)
{
	uint8_t result = 0;
	for (int i = 0; i < 8; i++) {
		result <<= 1;	// Shift result left by 1 bit
		result |= (b & 1);	// Set the least significant bit of result to the current bit of b
		b >>= 1;	// Shift b right by 1 bit to move to the next bit
	}
	return result;
}

int main(void)
{
	int c;
	while ((c = getchar()) != EOF) {
		c = reverse(c);
		putchar(c);
	}
}
