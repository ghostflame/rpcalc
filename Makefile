CC     = /usr/bin/gcc -std=c11 $(WFLAGS)

WFLAGS = -Wall -Wshadow -Wpedantic -Wextra -Wno-unused-parameter -Wno-implicit-fallthrough
CFLAGS = -O3 -I.

FILES  = rpsrc rpstack
OFILES = $(FILES:%=%.o)
BIN    = rpcalc

all:     $(BIN)


$(BIN): $(OFILES)
	$(CC) -o $(BIN) $(OFILES) -I. -lm


addtobin: $(BIN)
	cp $(BIN) ~/bin/

clean:
	rm -f $(BIN) *.o
