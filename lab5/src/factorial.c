#include "factorial.h"
#include <stdio.h>

void* compute_factorial_range(void* args) {
    struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
    
    int start = thread_args->start;
    int end = thread_args->end;
    int mod = thread_args->mod;
    
    long long partial_result = 1;
    for (int i = start; i <= end; i++) {
        partial_result = (partial_result * i) % mod;
    }
    
    printf("Thread computed [%d-%d]: partial result = %lld\n", start, end, partial_result);
    
    pthread_mutex_lock(thread_args->mutex);
    
    *(thread_args->result) = (*(thread_args->result) * partial_result) % mod;
    
    printf("Thread updated global result to: %lld\n", *(thread_args->result));
    
    pthread_mutex_unlock(thread_args->mutex);
    
    return NULL;
}