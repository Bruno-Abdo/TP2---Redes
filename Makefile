CC = gcc
CFLAGS = -Wall -g
BIN_DIR = bin

all: client server

client: client.c common.c
	$(CC) $(CFLAGS) -o $@ $^

server: server.c common.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf bin
