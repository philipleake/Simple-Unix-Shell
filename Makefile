CC = gcc
CFLAGS = -Wall
all: sh257

sh257: csapp.o shellex.o
	$(CC) $(CFLAGS) -pthread -o sh257 csapp.o shellex.o
csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c
shellex.o: shellex.c csapp.h
	$(CC) $(CFLAGS) -c shellex.c
clean:
	rm -v sh257
