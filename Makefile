# Which compler
CC = gcc

# Options for development
CFLAGS = -Wall

9cc: 9cc.c

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o tmp*
