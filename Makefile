CC= gcc
CFLAGS= -Wall -Wextra -Werror -pedantic -std=c99
VPATH= src
SRC= myfind.c
OBJS= $(SRC:.c=.o)

.PHONY: all myfind

all: myfind

myfind: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
clean:
	$(RM) $(OBJ) *.swp
