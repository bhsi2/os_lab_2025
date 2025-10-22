#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "common.h"

struct Server {
    char ip[255];
    int port;
};

// Функция для чтения серверов из файла
int ReadServersFromFile(const char *filename, struct Server **servers) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Cannot open servers file: %s\n", filename);
        return -1;
    }

    int capacity = 10;
    int count = 0;
    *servers = malloc(sizeof(struct Server) * capacity);

    char line[255];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        
        if (strlen(line) == 0) continue;

        char *colon = strchr(line, ':');
        if (colon == NULL) {
            fprintf(stderr, "Invalid server format: %s (expected ip:port)\n", line);
            continue;
        }

        *colon = '\0';
        int port = atoi(colon + 1);

        if (count >= capacity) {
            capacity *= 2;
            *servers = realloc(*servers, sizeof(struct Server) * capacity);
        }

        strncpy((*servers)[count].ip, line, sizeof((*servers)[count].ip) - 1);
        (*servers)[count].ip[sizeof((*servers)[count].ip) - 1] = '\0';
        (*servers)[count].port = port;
        count++;
    }

    fclose(file);
    return count;
}

int main(int argc, char **argv) {
    uint64_t k = -1;
    uint64_t mod = -1;
    char servers_file[255] = {'\0'};

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {{"k", required_argument, 0, 0},
                                        {"mod", required_argument, 0, 0},
                                        {"servers", required_argument, 0, 0},
                                        {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index);

        if (c == -1)
            break;

        switch (c) {
        case 0: {
            switch (option_index) {
            case 0:
                if (!ConvertStringToUI64(optarg, &k)) {
                    fprintf(stderr, "Invalid k value: %s\n", optarg);
                    return 1;
                }
                break;
            case 1:
                if (!ConvertStringToUI64(optarg, &mod)) {
                    fprintf(stderr, "Invalid mod value: %s\n", optarg);
                    return 1;
                }
                break;
            case 2:
                strncpy(servers_file, optarg, sizeof(servers_file) - 1);
                servers_file[sizeof(servers_file) - 1] = '\0';
                break;
            default:
                printf("Index %d is out of options\n", option_index);
            }
        } break;

        case '?':
            printf("Arguments error\n");
            break;
        default:
            fprintf(stderr, "getopt returned character code 0%o?\n", c);
        }
    }

    if (k == -1 || mod == -1 || !strlen(servers_file)) {
        fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
                argv[0]);
        return 1;
    }

    struct Server *servers = NULL;
    int servers_num = ReadServersFromFile(servers_file, &servers);
    if (servers_num <= 0) {
        fprintf(stderr, "No valid servers found in file: %s\n", servers_file);
        return 1;
    }

    printf("Found %d servers, k = %llu, mod = %llu\n", servers_num, k, mod);

    uint64_t total_result = 1;
    
    for (int i = 0; i < servers_num; i++) {
        struct hostent *hostname = gethostbyname(servers[i].ip);
        if (hostname == NULL) {
            fprintf(stderr, "gethostbyname failed with %s\n", servers[i].ip);
            continue;
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(servers[i].port);
        server_addr.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            fprintf(stderr, "Socket creation failed for server %s:%d!\n", 
                    servers[i].ip, servers[i].port);
            continue;
        }

        if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            fprintf(stderr, "Connection failed to %s:%d\n", 
                    servers[i].ip, servers[i].port);
            close(sockfd);
            continue;
        }

        uint64_t numbers_per_server = k / servers_num;
        uint64_t remainder = k % servers_num;
        
        uint64_t begin = i * numbers_per_server + 1;
        uint64_t end = (i + 1) * numbers_per_server;
        
        if (i < remainder) {
            begin += i;
            end += i + 1;
        } else {
            begin += remainder;
            end += remainder;
        }

        if (i == servers_num - 1) {
            end = k;
        }

        printf("Server %d (%s:%d): calculating range %llu-%llu\n", 
               i, servers[i].ip, servers[i].port, begin, end);

        char task[sizeof(uint64_t) * 3];
        memcpy(task, &begin, sizeof(uint64_t));
        memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
        memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

        if (send(sockfd, task, sizeof(task), 0) < 0) {
            fprintf(stderr, "Send failed to server %s:%d\n", 
                    servers[i].ip, servers[i].port);
            close(sockfd);
            continue;
        }

        char response[sizeof(uint64_t)];
        ssize_t bytes_received = recv(sockfd, response, sizeof(response), 0);
        if (bytes_received < 0) {
            fprintf(stderr, "Receive failed from server %s:%d\n", 
                    servers[i].ip, servers[i].port);
            close(sockfd);
            continue;
        }

        if (bytes_received != sizeof(uint64_t)) {
            fprintf(stderr, "Invalid response size from server %s:%d\n", 
                    servers[i].ip, servers[i].port);
            close(sockfd);
            continue;
        }

        uint64_t partial_result = 0;
        memcpy(&partial_result, response, sizeof(uint64_t));
        
        printf("Server %d result: %llu\n", i, partial_result);
        
        total_result = MultModulo(total_result, partial_result, mod);
        
        close(sockfd);
    }

    printf("\nFinal result: %llu! mod %llu = %llu\n", k, mod, total_result);
    
    free(servers);
    return 0;
}