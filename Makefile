FLAGS = -g -std=gnu11 -Wall -lpthread

CC = gcc
RM=rm -f

main:  main.o queue.o list.o sha256.o
	$(CC) $(FLAGS) -o hash main.o list.o queue.o sha256.o -lm -lpthread

list.o: list.c
	$(CC) $(FLAGS) -c list.c -lpthread

main.o: main.c
	$(CC) $(FLAGS) -c main.c -lpthread

queue.o: queue.c
	$(CC) $(FLAGS) -c queue.c -lpthread

sha256.o: sha256.c
	$(CC) $(FLAGS) -c sha256.c -lpthread

clean:
	$(RM) hash *.o
