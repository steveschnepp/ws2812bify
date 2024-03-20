#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 64 * 1024	// 64kiB

int main()
{
	ssize_t bytes_read;

	char *buffer = malloc(BUFFER_SIZE);
	if (!buffer) {
		exit(EXIT_FAILURE);
	}

	// Read from standard input to local buffer
	ssize_t buffer_len = 0;
	while ((bytes_read =
		read(STDIN_FILENO, buffer + buffer_len, BUFFER_SIZE)) > 0) {
		buffer_len += bytes_read;
		buffer = realloc(buffer, buffer_len + BUFFER_SIZE);
	}

	// Write the local buffer
	ssize_t bytes_written;
	char *out_buffer = buffer;
	while ((bytes_written = write(STDOUT_FILENO, buffer, buffer_len)) > 0) {
		out_buffer += bytes_written;
		buffer_len -= bytes_written;
	}

	return 0;
}
