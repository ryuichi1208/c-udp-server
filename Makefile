CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lyaml -ljansson

TARGET = udp_server
SRCS = udp_server.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
