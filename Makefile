CC     = /usr/bin/gcc -std=c11 $(WFLAGS)
BIN    = rpcalc

all:     bin test doc


bin:
	@cd src && make

addtobin: $(BIN)  ## Installs to ~/bin, not configurable yet.
	cp $(BIN) ~/bin/

clean:  ## Cleans all build files.
	@cd src && make clean
	@rm -f doc/*.info

test:   ## Runs the tests
	@cd testing && ./runtests.sh

doc:  ## Creates documentation.
	@cd doc && makeinfo rpcalc.texi

.PHONY: doc

help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
