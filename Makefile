CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lyaml -ljansson

SERVER_TARGET = udp_server
CLIENT_TARGET = udp_client
HEADERS = udp_server.h config.h logger.h socket_utils.h udp_client.h
SERVER_SRCS = udp_server.c
CLIENT_SRCS = udp_client.c

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SERVER_SRCS) $(LDFLAGS)

$(CLIENT_TARGET): $(CLIENT_SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_SRCS) $(LDFLAGS)

clean:
	rm -f $(SERVER_TARGET) $(CLIENT_TARGET)

.PHONY: all clean
