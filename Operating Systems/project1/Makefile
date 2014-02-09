CC = gcc

default: all

all: doit shell shell2

doit: runstat.o doit.c
	$(CC) doit.c runstat.o -o doit

shell: runstat.o shell.c
	$(CC) shell.c runstat.o -o shell 

shell2: runstat.o shell2.c
	$(CC) shell2.c runstat.o -o shell2

runstat.o: runstat.c
	$(CC) -c runstat.c

clean:
	rm doit shell shell2 *.o *~


