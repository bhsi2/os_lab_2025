#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SADDR struct sockaddr

void print_usage(const char *program_name) {
    printf("Usage: %s --port <port> --bufsize <bufsize>\n", program_name);
    printf("Example: %s --port 10050 --bufsize 100\n", program_name);
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

    const size_t kSize = sizeof(struct sockaddr_in);

    int lfd, cfd;
    int nread;
    char *buf = malloc(bufsize);
    if (buf == NULL) {
        perror("malloc");
        exit(1);
    }
    
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        free(buf);
        exit(1);
    }

    memset(&servaddr, 0, kSize);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(lfd, (SADDR *)&servaddr, kSize) < 0) {
        perror("bind");
        free(buf);
        close(lfd);
        exit(1);
    }

    if (listen(lfd, 5) < 0) {
        perror("listen");
        free(buf);
        close(lfd);
        exit(1);
    }

    printf("TCP Server started on port %d with buffer size %d\n", port, bufsize);

    while (1) {
        unsigned int clilen = kSize;

        if ((cfd = accept(lfd, (SADDR *)&cliaddr, &clilen)) < 0) {
            perror("accept");
            continue;
        }
        printf("Connection established\n");

        while ((nread = read(cfd, buf, bufsize)) > 0) {
            write(1, buf, nread);
        }

        if (nread == -1) {
            perror("read");
        }
        
        close(cfd);
        printf("Connection closed\n");
    }

    free(buf);
    close(lfd);
    return 0;
}