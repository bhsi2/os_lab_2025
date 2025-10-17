#ifndef FACTORIAL_H
#define FACTORIAL_H

#include <pthread.h>


struct ThreadArgs {
    int start;          
    int end;            
    int mod;            
    long long *result;  
    pthread_mutex_t *mutex; 
};

void* compute_factorial_range(void* args);

#endif