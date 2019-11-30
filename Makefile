.PHONY: all debug
.DEFAULT: all
CFLAGS = -Wall -pedantic -Werror -std=gnu99 # -pthread
all: lacec
debug: clean
debug: CFLAGS += -g
debug: DEFINES += -D LOGGING
debug: lacec

lacec: main.c main.h lexer.c lexer.h parse.c parse.h resolve.c resolve.h compile.c compile.h utils.c utils.h dictionary.c dictionary.h macros.h includes.h sb.c sb.h
	clang $(CFLAGS) main.c parse.c lexer.c resolve.c compile.c utils.c dictionary.c sb.c $(DEFINES) -o lacec
clean:
	rm -f *.o lacec
