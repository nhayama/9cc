CC=gcc
CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

C_DEBUG_FLAGS=-g3

all: 9cc

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
	./9cc -t
	./test.sh

.PHONY: debug
debug: CFLAGS+=$(C_DEBUG_FLAGS)
debug: all

.PHONY: clean
clean:
	rm -f 9cc *.o *~ tmp*
