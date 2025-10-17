#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/time.h>

#include <pthread.h>

#include "utils.h"
#include "sum.h"


struct Config {
  uint32_t threads_num;
  uint32_t array_size;
  uint32_t seed;
};

int parse_args(int argc, char **argv, struct Config *config) {
  static struct option options[] = {
      {"threads_num", required_argument, 0, 't'},
      {"array_size", required_argument, 0, 'a'},
      {"seed", required_argument, 0, 's'},
      {0, 0, 0, 0}
  };

  int option_index = 0;
  int c;
  
  while ((c = getopt_long(argc, argv, "t:a:s:", options, &option_index)) != -1) {
    switch (c) {
      case 't':
        config->threads_num = atoi(optarg);
        if (config->threads_num <= 0) {
          printf("threads_num must be positive\n");
          return -1;
        }
        break;
      case 'a':
        config->array_size = atoi(optarg);
        if (config->array_size <= 0) {
          printf("array_size must be positive\n");
          return -1;
        }
        break;
      case 's':
        config->seed = atoi(optarg);
        if (config->seed <= 0) {
          printf("seed must be positive\n");
          return -1;
        }
        break;
      default:
        printf("Usage: %s --threads_num num --array_size num --seed num\n", argv[0]);
        return -1;
    }
  }
  
  if (config->threads_num == 0 || config->array_size == 0 || config->seed == 0) {
    printf("Usage: %s --threads_num num --array_size num --seed num\n", argv[0]);
    return -1;
  }
  
  return 0;
}


void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

long long current_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main(int argc, char **argv) {
  struct Config config = {0, 0, 0};

  if (parse_args(argc, argv, &config) != 0) {
    return 1;
  }


  int *array = malloc(sizeof(int) * config.array_size);
  if (array == NULL) {
    printf("Memory allocation failed for array\n");
    return 1;
  }

  GenerateArray(array, config.array_size, config.seed);


  pthread_t threads[config.threads_num];
  struct SumArgs args[config.threads_num];


  int chunk_size = config.array_size / config.threads_num;
  int remainder = config.array_size % config.threads_num;


  int current_start = 0;
  for (uint32_t i = 0; i < config.threads_num; i++) {
    args[i].array = array;
    args[i].begin = current_start;
    
    int current_end = current_start + chunk_size + (i < remainder ? 1 : 0);
    args[i].end = current_end;
    
    current_start = current_end;
  }


  long long start_time = current_time_ms();

  for (uint32_t i = 0; i < config.threads_num; i++) {
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i]) != 0) {
      printf("Error: pthread_create failed for thread %u!\n", i);
      free(array);
      return 1;
    }
  }


  int total_sum = 0;
  for (uint32_t i = 0; i < config.threads_num; i++) {
    void *thread_result;
    if (pthread_join(threads[i], &thread_result) != 0) {
      printf("Error: pthread_join failed for thread %u!\n", i);
      free(array);
      return 1;
    }
    int sum = (int)(size_t)thread_result;
    total_sum += sum;
    printf("Thread %u sum: %d\n", i, sum);
  }


  long long end_time = current_time_ms();
  long long elapsed_time = end_time - start_time;

  free(array);


  printf("Calculation time: %lld ms\n", elapsed_time);
  

  return 0;
}
