CC := gcc
CFLAGS = -g -fno-plt -Wall -Wextra -Werror -Wpedantic -std=c99

OBJS = pakar.c.o serialize.c.o

pakar: $(OBJS) pakar.h serialize.h
	$(CC) -o $@ $(OBJS)

default: all
all: pakar

%.c.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -v $(OBJS) pakar
