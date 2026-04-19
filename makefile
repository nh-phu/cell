CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = cell
BUILTIN_SRCS := $(wildcard builtins/*.c)
SRCS = main.c input.c lexer.c run.c prompt.c util.c $(BUILTIN_SRCS)
HEADERS = headers.h

cell: $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

test_parse: test/test_parse.c input.c lexer.c util.c headers.h lexer.h
	$(CC) $(CFLAGS) -o test/test_parse test/test_parse.c input.c lexer.c util.c

test: test_parse
	@./test/test_parse
	rm -f test/test_parse

clean:
	rm -f $(TARGET)
	rm -f test/test_parse

run: $(TARGET)
	@./$(TARGET)
