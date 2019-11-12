CC= gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -std=c99
VPATH= src
SRC= myfind.c parse.c
OBJS= $(SRC:.c=.o)
BIN= myfind
.PHONY: all myfind

all: myfind

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
clean:
	$(RM) $(OBJS) $(BIN) *.swp *.log
