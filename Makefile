CC= gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -std=c99
LDFLAGS= -fsanitize=address -g
VPATH= src
SRC= myfind.c parse.c ast.c utils.c
OBJS= $(SRC:.c=.o)
BIN= myfind

.PHONY: all myfind

all: myfind

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
clean:
	$(RM) $(OBJS) $(BIN) *.swp *.log
