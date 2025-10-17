#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

#include "factorial.h"

void parse_arguments(int argc, char *argv[], int *k, int *pnum, int *mod) {
    static struct option long_options[] = {
        {"k", required_argument, 0, 'k'},
        {"pnum", required_argument, 0, 'p'},
        {"mod", required_argument, 0, 'm'},
        {0, 0, 0, 0}
    };
    
    *k = 0;
    *pnum = 1;
    *mod = 0;
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "k:p:m:", long_options, &option_index)) != -1) {
        switch (c) {
            case 'k':
                *k = atoi(optarg);
                if (*k < 0) {
                    fprintf(stderr, "Error: k must be non-negative\n");
                    exit(1);
                }
                break;
            case 'p':
                *pnum = atoi(optarg);
                if (*pnum <= 0) {
                    fprintf(stderr, "Error: pnum must be positive\n");
                    exit(1);
                }
                break;
            case 'm':
                *mod = atoi(optarg);
                if (*mod <= 0) {
                    fprintf(stderr, "Error: mod must be positive\n");
                    exit(1);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s -k <number> --pnum=<threads> --mod=<modulus>\n", argv[0]);
                fprintf(stderr, "Example: %s -k 10 --pnum=4 --mod=1000\n", argv[0]);
                exit(1);
        }
    }
    
    if (*k == 0 || *mod == 0) {
        fprintf(stderr, "Error: k and mod are required parameters\n");
        fprintf(stderr, "Usage: %s -k <number> --pnum=<threads> --mod=<modulus>\n", argv[0]);
        exit(1);
    }
}


int main(int argc char *argv[]) {
    int k, pnum, mod;
    
    parse_arguments(argc, argv, &k, &pnum, &mod);
    
    
    if (pnum > k) {
        pnum = k;
        printf("Adjusted threads to %d (cannot exceed k)\n", pnum);
    }
    
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    long long parallel_result = 1;
    
    pthread_t threads[pnum];
    struct ThreadArgs thread_args[pnum];
    
    int numbers_per_thread = k / pnum;
    int remainder = k % pnum;
        
    int current_start = 1;
    for (int i = 0; i < pnum; i++) {
        int range_size = numbers_per_thread;
        if (i < remainder) {
            range_size++; 
        }
        
        int current_end = current_start + range_size - 1;
        if (current_end > k) {
            current_end = k;
        }
        
        thread_args[i].start = current_start;
        thread_args[i].end = current_end;
        thread_args[i].mod = mod;
        thread_args[i].result = &parallel_result;
        thread_args[i].mutex = &mutex;
        
        printf("Thread %d: numbers from %d to %d\n", i, current_start, current_end);
        
        if (pthread_create(&threads[i], NULL, compute_factorial_range, &thread_args[i]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(1);
        }
        
        current_start = current_end + 1;
    }
    
    for (int i = 0; i < pnum; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Error joining thread %d\n", i);
            exit(1);
        }
    }
    
    pthread_mutex_destroy(&mutex);

    
    return 0;
}