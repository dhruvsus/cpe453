CC = gcc
CFLAGS = -Wall -lm
SOURCES = $(*.c)
TARGET = prog3

all: $(TARGET)

prog3:  prog3.c prog3.h
	$(CC) -o prog3 prog3.c $(CFLAGS)

clean:
	rm prog3