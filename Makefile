.PHONY: all debug
.DEFAULT: all
CFLAGS = -Wall -pedantic -std=gnu99 -pthread
all: lacec

debug: CFLAGS += -g -O1
debug: DEFINES += -D LOGGING
debug: lacec

lacec: main.c main.h lexer.c lexer.h parse.c parse.h compile.c compile.h utils.c utils.h macros.h includes.h
	clang $(CFLAGS) main.c parse.c lexer.c compile.c utils.c $(DEFINES) -o lacec
clean:
	rm -f *.o
