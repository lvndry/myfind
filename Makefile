CC= gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -std=c99 -g -fsanitize=address -fsanitize=leak
LDFLAGS= -fsanitize=address -fsanitize=leak
VPATH= src
SRC= actionexp.c ast.c errors.c memory.c myfind.c parse.c parseand.c parseor.c stack.c testexp.c utils.c
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
