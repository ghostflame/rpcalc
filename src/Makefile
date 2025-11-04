CC     = /usr/bin/gcc -std=c11 $(WFLAGS)

WFLAGS = -Wall -Wshadow -Wpedantic -Wextra -Wno-unused-parameter -Wno-implicit-fallthrough
#CFLAGS = -O3 -I.
CFLAGS = -g -pg -ggdb3

FILES  = args calc stack help helpers report main prime data
OFILES = $(FILES:%=%.o)
BIN    = ../rpcalc

all:     $(BIN)

$(BIN): $(OFILES)
	$(CC) -o $(BIN) $(OFILES) -I. -lm

clean:  ## Cleans all build files.
	rm -f $(BIN) $(OFILES)

