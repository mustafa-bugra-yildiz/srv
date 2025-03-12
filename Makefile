CFLAGS += -std=c89
CFLAGS += -Wall -Wextra -Werror

bin/srv: src/srv.c
	mkdir -p bin
	clang $(CFLAGS) -o bin/srv src/srv.c
