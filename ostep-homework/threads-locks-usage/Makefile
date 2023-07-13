
FLAGS = -Wall -pthread -g

all: simple-counter approx-counter

clean:
	rm -f simple-counter approx-counter

simple-counter: simple-counter.c common_threads.h
	gcc -o simple-counter simple-counter.c $(FLAGS)

approx-counter: approx-counter.c common_threads.h
	gcc -o approx-counter approx-counter.c $(FLAGS)
