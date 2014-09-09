# P=sudoku
OBJECTS = array.o sudoku.o
CFLAGS = -std=gnu99 -O2 -Wall -g
LDLIBS=
CC=gcc

# $(P): $(OBJECTS)

.PHONY: clean

all: sudoku array_test

clean:
	rm -f *.o sudoku array_test

sudoku: sudoku.o
	$(CC) $(CFLAGS) $^ -o $@

sudoku.o: sudoku.c
	$(CC) -c $(CFLAGS) $<

array.o: array.c array.h
	$(CC) -c $(CFLAGS) $<

array_test.o: array_test.c array.h
	$(CC) -c $(CFLAGS) $<

array_test: array_test.o array.o
	$(CC) $(CFLAGS) $^ -o $@

test: array_test
	./array_test

