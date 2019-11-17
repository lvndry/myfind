CC= gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -std=c99 -O3
LDFLAGS= -lasan -fsanitize=address -g
VPATH= src
SRC= myfind.c parse.c ast.c utils.c errors.c
OBJS= $(SRC:.c=.o)
BIN= myfind

.PHONY: all myfind debug

rebuild: clean all

all: myfind

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

debug: $(OBJS)
	$(CC) $(CFLAGS) -g -o debug $^

check: ./tests/test.sh

clean:
	$(RM) $(OBJS) $(BIN) *.swp *.log
