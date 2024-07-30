# RPCALC

Like `dc`, but better…

## Installation

Run `make help` to see the available commands.

**TL;DR** Run `make -j `nproc` && make addtobin` which will install in `$HOME/bin`.

## How to use this

Examples of some commands:

```bash
√ ; ./rpcalc 9 rs
3.000000
√ ; ./rpcalc 1 1 +
2.000000
```
Rpcalc will take options on the command line, or to stdin.  If you wish to provide both,
the `.` command will cause stdin processing to occur after all command line args have been
consumed.


## Get full help

Run `rpcalc ?` (or `rpcalc "?"` in `zsh`) to get full help. You can use `rpcalc @` for all shells.
