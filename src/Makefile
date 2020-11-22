C=gcc
LIBFLAGS=-lm
CFLAGS=-I.

main: main.o hash_table.o prime.o
	$(CC) -o main  hash_table.o main.o prime.o $(LIBFLAGS)

clean: 
	-rm -f *.o core *.core
