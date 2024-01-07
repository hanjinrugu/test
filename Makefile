# Makefile for a simple C program

SRC_C=test.c
SRC_O=test.o
CC=gcc
CFLAGS=-Wall
TARGET=main

all: $(TARGET) run

$(TARGET): $(SRC_O)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC_O)

$(SRC_O): $(SRC_C)
	$(CC) $(CFLAGS) -c $(SRC_C)

clean:
	rm -f $(TARGET) $(SRC_O)

run:
	./$(TARGET)
