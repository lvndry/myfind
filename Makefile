CC= gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -std=c99 -g -fsanitize=address
LDFLAGS= -fsanitize=address
VPATH= src
SRC= myfind.c ast.c astlib.c errors.c parse.c parselib.c stack.c utils.c
OBJS= $(SRC:.c=.o)
BIN= myfind

.PHONY: all myfind debug

rebuild: clean all

all: myfind

$(BIN): $(OBJS)
	$(CC)  $(CFLAGS) $(LDFLAGS) -o $@ $^

debug: $(OBJS)
	$(CC) $(CFLAGS) -g -o debug $^

check: $(BIN)
	./tests/test.sh

clean:
	$(RM) $(OBJS) $(BIN) *.swp *.log
