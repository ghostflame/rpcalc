CC     = /usr/bin/gcc -std=c11 $(WFLAGS)

WFLAGS = -Wall -Wshadow -Wpedantic -Wextra -Wno-unused-parameter -Wno-implicit-fallthrough
CFLAGS = -O3 -I.

FILES  = args stack help helpers main
OFILES = $(FILES:%=%.o)
BIN    = ../rpcalc

all:     $(BIN)

$(BIN): $(OFILES)
	$(CC) -o $(BIN) $(OFILES) -I. -lm


addtobin: $(BIN)  ## Installs to ~/bin, not configurable yet.
	cp $(BIN) ~/bin/

clean:  ## Cleans all build files.
	rm -f $(BIN) $(OFILES)

