CFLAGS += -Og -Wall -Werror -g
LDLIBS =

all: ws2812bify reverse flushonce invert

ws2812bify: ws2812bify.o

clean:
		rm -f ws2812bify reverse flushonce invert *.o

%.bin: %.xxd
	xxd
