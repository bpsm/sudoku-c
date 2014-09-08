P=array_test
OBJECTS = array.o array_test.o
CFLAGS = -std=gnu99 -O0 -Wall -g
LDLIBS=
CC=gcc

$(P): $(OBJECTS)
