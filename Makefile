FLAGS = -g -std=gnu11 -Wall -lpthread

CC = gcc
RM=rm -f

all:  main.o queue.o list.o md5.o
	$(CC) $(FLAGS) -o hash main.o list.o queue.o md5.o -lm -lpthread

list.o: list.c
	$(CC) $(FLAGS) -c list.c

main.o: main.c
	$(CC) $(FLAGS) -c main.c

queue.o: queue.c
	$(CC) $(FLAGS) -c queue.c

md5.o: md5.c
	$(CC) $(FLAGS) -c md5.c

clean:
	$(RM) hash *.o

sha256gen: 
	$(CC) $(FLAGS) sha256gen.c sha256.c -o sha256gen