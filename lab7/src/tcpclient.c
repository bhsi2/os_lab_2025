#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SADDR struct sockaddr
#define SIZE sizeof(struct sockaddr_in)

void print_usage(const char *program_name) {
    printf("Usage: %s --ip <ip> --port <port> --bufsize <bufsize>\n", program_name);
    printf("Example: %s --ip 127.0.0.1 --port 10050 --bufsize 100\n", program_name);
}

int main(int argc, char *argv[]) {
    char *ip = NULL;
    int port = -1;
    int bufsize = -1;

    // Парсинг аргументов командной строки
    static struct option long_options[] = {
        {"ip", required_argument, 0, 'i'},
        {"port", required_argument, 0, 'p'},
        {"bufsize", required_argument, 0, 'b'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "i:p:b:", long_options, &option_index)) != -1) {
        switch (c) {
            case 'i':
                ip = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                if (port <= 0) {
                    fprintf(stderr, "Port must be positive\n");
                    exit(1);
                }
                break;
            case 'b':
                bufsize = atoi(optarg);
                if (bufsize <= 0) {
                    fprintf(stderr, "Buffer size must be positive\n");
                    exit(1);
                }
                break;
            default:
                print_usage(argv[0]);
                exit(1);
        }
    }

    if (ip == NULL || port == -1 || bufsize == -1) {
        print_usage(argv[0]);
        exit(1);
    }

    int fd;
    int nread;
    char *buf = malloc(bufsize);
    if (buf == NULL) {
        perror("malloc");
        exit(1);
    }
    
    struct sockaddr_in servaddr;
    
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creating");
        free(buf);
        exit(1);
    }

    memset(&servaddr, 0, SIZE);
    servaddr.sin_family = AF_INET;

    if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0) {
        perror("bad address");
        free(buf);
        close(fd);
        exit(1);
    }

    servaddr.sin_port = htons(port);

    if (connect(fd, (SADDR *)&servaddr, SIZE) < 0) {
        perror("connect");
        free(buf);
        close(fd);
        exit(1);
    }

    printf("Connected to server %s:%d\n", ip, port);
    printf("Input message to send (Ctrl+D to exit):\n");
    
    while ((nread = read(0, buf, bufsize)) > 0) {
        if (write(fd, buf, nread) < 0) {
            perror("write");
            break;
        }
    }

    free(buf);
    close(fd);
    printf("Disconnected\n");
    
    return 0;
}