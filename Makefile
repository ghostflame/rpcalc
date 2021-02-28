CC     = /usr/bin/gcc -std=c11 $(WFLAGS)

WFLAGS = -Wall -Wshadow -Wpedantic -Wextra -Wno-unused-parameter
CFLAGS = -O3

FILES  = rpsrc
OFILES = $(FILES:%=%.o)
BIN    = rpcalc

all:     $(BIN)


$(BIN): $(OFILES)
	$(CC) -o $(BIN) $(OFILES) -lm


clean:
	rm -f $(BIN) *.o
