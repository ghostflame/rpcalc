CC     = /usr/bin/gcc -std=c11 $(WFLAGS)

WFLAGS = -Wall -Wshadow -Wpedantic -Wextra -Wno-unused-parameter -Wno-implicit-fallthrough
CFLAGS = -O3 -I.

FILES  = rpsrc rpargs rpstack
OFILES = $(FILES:%=%.o)
BIN    = rpcalc

all:     $(BIN) test doc


$(BIN): $(OFILES)
	$(CC) -o $(BIN) $(OFILES) -I. -lm


addtobin: $(BIN)  ## Installs to ~/bin, not configurable yet.
	cp $(BIN) ~/bin/

clean:  ## Cleans all build files.
	rm -f $(BIN) *.o doc/*.info

test:   ## Runs the tests
	cd testing && ./runtests.sh

doc:  ## Creates documentation.
	cd doc && makeinfo rpcalc.texi

.PHONY: doc

help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
