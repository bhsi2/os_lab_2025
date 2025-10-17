#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

int thread1_has_mutex1 = 0;
int thread1_has_mutex2 = 0;
int thread2_has_mutex1 = 0;
int thread2_has_mutex2 = 0;

void* thread1_function(void* arg) {
    printf("Thread 1: Started\n");
    
    pthread_mutex_lock(&mutex1);
    thread1_has_mutex1 = 1;
    printf("Thread 1: Locked mutex1\n");
    
    sleep(1);
    
    printf("Thread 1: Trying to lock mutex2...\n");
    pthread_mutex_lock(&mutex2);
    thread1_has_mutex2 = 1;
    printf("Thread 1: Locked mutex2\n");
       
    return NULL;
}

void* thread2_function(void* arg) {
    printf("Thread 2: Started\n");
    
    pthread_mutex_lock(&mutex2);
    thread2_has_mutex2 = 1;
    printf("Thread 2: Locked mutex2\n");
    
    sleep(1);
    
    printf("Thread 2: Trying to lock mutex1...\n");
    pthread_mutex_lock(&mutex1);
    thread2_has_mutex1 = 1;
    printf("Thread 2: Locked mutex1\n");
    
    return NULL;
}

void* monitor_function(void* arg) {
    printf("Monitor: Started\n");
    
    int deadlock_detected = 0;
    
    while (!deadlock_detected) {
        sleep(2);
        
        printf("\n=== MONITOR REPORT ===\n");
        printf("Thread 1: has mutex1=%d, has mutex2=%d\n", 
               thread1_has_mutex1, thread1_has_mutex2);
        printf("Thread 2: has mutex1=%d, has mutex2=%d\n", 
               thread2_has_mutex1, thread2_has_mutex2);
        
        if (thread1_has_mutex1 && !thread1_has_mutex2 && 
            thread2_has_mutex2 && !thread2_has_mutex1) {
            printf("Deadlock!\n");
            printf("Thread 1 is waiting for mutex2 (held by Thread 2)\n");
            printf("Thread 2 is waiting for mutex1 (held by Thread 1)\n");
            deadlock_detected = 1;
        } else if (thread1_has_mutex1 && thread1_has_mutex2) {
            printf("Thread 1 has both mutexes - no deadlock\n");
        } else if (thread2_has_mutex1 && thread2_has_mutex2) {
            printf("Thread 2 has both mutexes - no deadlock\n");
        } else {
            printf("Still working...\n");
        }
        printf("=====================\n\n");
        
        if (deadlock_detected) {
            printf("Program is in deadlock state. Press Ctrl+C to terminate.\n");
        }
    }
    
    return NULL;
}


int main(int argc, char* argv[]) {
    printf("=== DEADLOCK DEMONSTRATION PROGRAM ===\n\n");
    
    pthread_t thread1, thread2, monitor;


    printf("Creating threads that will cause deadlock...\n");
    
    pthread_create(&monitor, NULL, monitor_function, NULL);
    sleep(1);
    
    pthread_create(&thread1, NULL, thread1_function, NULL);
    pthread_create(&thread2, NULL, thread2_function, NULL);
    
    pthread_join(monitor, NULL);

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    
    return 0;
}