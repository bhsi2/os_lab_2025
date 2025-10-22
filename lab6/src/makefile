CC = gcc
CFLAGS = -pthread

all: client server

client: client.c common.o common.h
	$(CC) $(CFLAGS) -o client common.o client.c

server: server.c common.o common.h
	$(CC) $(CFLAGS) -o server common.o server.c

common.o: common.c common.h
	$(CC) $(CFLAGS) -o common.o -c common.c

clean:
	rm -f client server common.o

run-server1:
	./server --port 20001 --tnum 4

run-server2:
	./server --port 20002 --tnum 4

run-server3:
	./server --port 20003 --tnum 4

run-client:
	./client --k 100 --mod 1000000007 --servers servers.txt

