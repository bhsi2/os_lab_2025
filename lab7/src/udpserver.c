#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SADDR struct sockaddr

void print_usage(const char *program_name) {
    printf("Usage: %s --port <port> --bufsize <bufsize>\n", program_name);
    printf("Example: %s --port 20001 --bufsize 1024\n", program_name);
}

int main(int argc, char *argv[]) {
    int port = -1;
    int bufsize = -1;

    // Парсинг аргументов командной строки
    static struct option long_options[] = {
        {"port", required_argument, 0, 'p'},
        {"bufsize", required_argument, 0, 'b'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "p:b:", long_options, &option_index)) != -1) {
        switch (c) {
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

    if (port == -1 || bufsize == -1) {
        print_usage(argv[0]);
        exit(1);
    }

    int sockfd, n;
    char *mesg = malloc(bufsize);
    char ipadr[16];
    
    if (mesg == NULL) {
        perror("malloc");
        exit(1);
    }
    
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket problem");
        free(mesg);
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (SADDR *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind problem");
        free(mesg);
        close(sockfd);
        exit(1);
    }
    
    printf("UDP Server starts on port %d with buffer size %d\n", port, bufsize);

    while (1) {
        unsigned int len = sizeof(cliaddr);

        if ((n = recvfrom(sockfd, mesg, bufsize, 0, (SADDR *)&cliaddr, &len)) < 0) {
            perror("recvfrom");
            continue;
        }
        mesg[n] = 0;

        printf("REQUEST: %s FROM %s:%d\n", mesg,
               inet_ntop(AF_INET, (void *)&cliaddr.sin_addr.s_addr, ipadr, 16),
               ntohs(cliaddr.sin_port));

        if (sendto(sockfd, mesg, n, 0, (SADDR *)&cliaddr, len) < 0) {
            perror("sendto");
            continue;
        }
    }

    free(mesg);
    close(sockfd);
    return 0;
}