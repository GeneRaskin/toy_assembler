CC = gcc
NAME = toyc
OBJS = bin_generator.o env.o lexer.o main.o parser.o sym_table.o
CFLAGS = -Wall -Wextra -Werror
INCLUDE = bin_generator.h env.h parser.h instruction_format.h \
	sym_table.h tokens.h

all: $(NAME)

%.o: %.c $(INCLUDE)
	$(CC) -c -o $@ $< $(CFLAGS)

$(NAME): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	/bin/rm -f $(OBJS)

fclean: clean
	/bin/rm -f $(NAME)

re: fclean all

.PHONY: all re clean fclean