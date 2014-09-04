P=sudoku
OBJECTS = array.o sudoku.o
CFLAGS = -std=gnu99 -O2 -Wall -g
LDLIBS=
CC=gcc

$(P): $(OBJECTS)
