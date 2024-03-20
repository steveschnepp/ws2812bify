/*
    STDIN = NUA format
    STDOUT = inverted binary stream for WS2812B
*/

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>

// Note : this has to be uint8_t
static inline
uint8_t revert_bits(uint8_t b)
{
	uint8_t result = 0;
	for (int i = 0; i < 8; i++) {
		result <<= 1;
		// Set the least significant bit of result to the current bit of b
		result |= (b & 1);
		b >>= 1;
	}
	return result;
}

void convert(uint32_t color, uint8_t * per8)
{
	// Expand the 24 bit into 8 UART bytes.
	// Every incoming bit is encoded as 3 bits in UART encoding
	// 0 --> 100
	// 1 --> 110
	// A UART byte is
	// 2 bits for 1st bit as the 1rst UART bit is the start bit
	// 3 bits for 2nd bit
	// 3 bits for 3rd bit
	// .. repeat for all the remaining 21 bits.

	// default = all zero incoming, so every 1/3 is set to 1.
	memset(per8, 0b00100100, 8);

	for (int i = 0; i < 24; i++) {
		int bit = (color >> (24 - i)) & 0x01;
		int idx = i / 3;
		int off = i % 3;

		if (!bit)
			continue;	// If 0 nothing to set
		switch (off) {
		case 0:
			per8[idx] |= 0b10000000;
			break;
		case 1:
			per8[idx] |= 0b00110000;
			break;
		case 2:
			per8[idx] |= 0b00000110;
			break;
		default:
			abort();	// Should never happen
		}
	}

	// UART is sending it LSB, WS2812B expects MSB
	for (int i = 0; i < 8; i++) {
		uint8_t c_lsb = per8[i];
		uint8_t c_msb = revert_bits(c_lsb);
		per8[i] = c_msb;
	}

	// We need to invert the UART as it is active high
	for (int i = 0; i < 8; i++) {
		uint8_t c_active_high = per8[i];
		uint8_t c_active_low = ~c_active_high;
		per8[i] = c_active_low;
	}

}

#define MAX_LINE_LENGTH 1 * 1024 * 1024 // 1MiB
uint32_t* scan_input_line(uint32_t* duration_ms, uint32_t *size)
{
	uint32_t* colors = NULL;
	uint32_t s = 0;

	char line[MAX_LINE_LENGTH];
	if (fgets(line , sizeof(line) , stdin) == NULL)
		goto END;

	// Parsing the whole line
    char* token = strtok(line, " ");

    while (token != NULL) {
		if (isspace(*token))  goto NEXT_TOKEN;

		uint32_t v = strtoul(token, NULL, 16);

		if (!s) {
			uint32_t duration_centisecs = v >> 24; // the first 8 bits of the 1rst token are the duration
			*duration_ms = duration_centisecs * 10;
		}
		v &= 0x00FFFFFF; // Ignore the duration byte

		s ++;
		colors = realloc(colors, (s) * sizeof(uint32_t));
		colors[s-1] = v;

NEXT_TOKEN:
        token = strtok(NULL, " "); // Next!
    }

END:
	*size = s;
	return colors;
}

void ussleep(unsigned int microseconds) {
	if (!microseconds) return;

    struct timespec req, rem;
    req.tv_sec = microseconds / 1000000; // Convert microseconds to seconds
    req.tv_nsec = (microseconds % 1000000) * 1000; // Convert remaining microseconds to nanoseconds

    // Call nanosleep
    while (nanosleep(&req, &rem) == -1) {
        // If interrupted, continue sleeping with the remaining time
        req = rem;
    }
}


int main(void)
{
	uint32_t* colors;
	uint32_t duration_ms;
	uint32_t size;
	uint8_t* buffer = NULL;
	while ((colors = scan_input_line(&duration_ms, &size))) {
		uint32_t buffer_size = size * 8 * sizeof(uint8_t);
		buffer = realloc(buffer, buffer_size);

		for (int i = 0; i < size; i ++) {
			uint32_t color = colors[i];
			convert(color, buffer + i*8);
		}

		// flush the whole buffer to UART in 1 GO.
		write(fileno(stdout), buffer, buffer_size);
		ussleep(50); // Reset

		ussleep(duration_ms * 1000);
	}

	if (buffer) free(buffer);
}
