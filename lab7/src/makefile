CC = gcc
CFLAGS = -I.
TARGETS = tcpserver tcpclient udpserver udpclient

TCP_PORT = 10050
TCP_BUFSIZE = 100
UDP_PORT = 20001
UDP_BUFSIZE = 1024
SERVER_IP = 127.0.0.1

all: $(TARGETS)

tcpserver: tcpserver.c
	$(CC) $(CFLAGS) -o tcpserver tcpserver.c

tcpclient: tcpclient.c
	$(CC) $(CFLAGS) -o tcpclient tcpclient.c

udpserver: udpserver.c
	$(CC) $(CFLAGS) -o udpserver udpserver.c

udpclient: udpclient.c
	$(CC) $(CFLAGS) -o udpclient udpclient.c

clean:
	rm -f $(TARGETS)


run-tcp-server:
	./tcpserver --port $(TCP_PORT) --bufsize $(TCP_BUFSIZE)


run-tcp-client:
	./tcpclient --ip $(SERVER_IP) --port $(TCP_PORT) --bufsize $(TCP_BUFSIZE)


run-udp-server:
	./udpserver --port $(UDP_PORT) --bufsize $(UDP_BUFSIZE)


run-udp-client:
	./udpclient --ip $(SERVER_IP) --port $(UDP_PORT) --bufsize $(UDP_BUFSIZE)

