CC=gcc
CFLAGS=-I.
DIR=../../lab3/src/
vpath %.h $(DIR)

all: process_memory parallel_min_max

process_memory : process_memory.c
	$(CC) process_memory.c -o process_memory $(CFLAGS)

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o $(DIR)parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c $(DIR)utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c $(DIR)find_min_max.c $(CFLAGS)

parallel_sum : sum.h
	$(CC) -pthread -o parallel_sum parallel_sum.c

clean : 
	rm process_memory parallel_min_max utils.o find_min_max.o parallel_sum