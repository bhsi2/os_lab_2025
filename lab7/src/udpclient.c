#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SADDR struct sockaddr

void print_usage(const char *program_name) {
    printf("Usage: %s --ip <ip> --port <port> --bufsize <bufsize>\n", program_name);
    printf("Example: %s --ip 127.0.0.1 --port 20001 --bufsize 1024\n", program_name);
}

int main(int argc, char **argv) {
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

    int sockfd, n;
    char *sendline = malloc(bufsize);
    char *recvline = malloc(bufsize + 1);
    
    if (sendline == NULL || recvline == NULL) {
        perror("malloc");
        free(sendline);
        free(recvline);
        exit(1);
    }
    
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &servaddr.sin_addr) < 0) {
        perror("inet_pton problem");
        free(sendline);
        free(recvline);
        exit(1);
    }
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket problem");
        free(sendline);
        free(recvline);
        exit(1);
    }

    printf("UDP Client connected to %s:%d\n", ip, port);
    printf("Enter string (Ctrl+D to exit):\n");

    while ((n = read(0, sendline, bufsize)) > 0) {
        if (sendto(sockfd, sendline, n, 0, (SADDR *)&servaddr, sizeof(servaddr)) == -1) {
            perror("sendto problem");
            break;
        }

        if (recvfrom(sockfd, recvline, bufsize, 0, NULL, NULL) == -1) {
            perror("recvfrom problem");
            break;
        }

        recvline[n] = '\0';
        printf("REPLY FROM SERVER: %s\n", recvline);
    }
    
    free(sendline);
    free(recvline);
    close(sockfd);
    printf("Client stopped\n");
    
    return 0;
}