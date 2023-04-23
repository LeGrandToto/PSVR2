CC=gcc
CFLAGS=-Wall -Werror -fpic
LIBS=-lusb-1.0

# Get all .c files in current directory
SRCS=$(wildcard *.c)
# Get all corresponding .o files
OBJS=$(SRCS:.c=.o)

all: main

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main: $(OBJS)
	$(CC) $(LIBS) $(OBJS) -o $@ 

clean:
	rm -f *.o *.so

