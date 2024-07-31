#!/bin/bash


# Nifty colours.
#
# Colours for echo commands.
# For colour codes, see https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit
error=$(tput setaf 9)   # bright red.
success=$(tput setaf 2) # normal green.
#warning=$(tput setaf 214) # orange.
info=$(tput setaf 99)   # purple.
header=$(tput setaf 69) # light blue.
#debug=$(tput setaf 240) # grey.
reset=$(tput sgr0)

tfiles=("Rpcalc-Overall.tst" "Rpcalc-Trig.tst" "Rpcalc-Bin.tst")
bin=../rpcalc

TRIED=0
PASSED=0
FAILED=0

function handle_line( )
{
	line="$*"


	if [ ${#line} -eq 0 ]; then
		# empty line
		return
	fi

	first=${line:0:1}
	if [ $first == "#" ]; then
		# ignore comments
		return
	fi

	answer=${line%% =*}
	formula=${line##*= }
	#echo "$answer <- $formula"
	got=$($bin $formula)
	((TRIED++))

	if [ "$got" == "$answer" ]; then
		((PASSED++))
	else
		((FAILED++))
		echo "Failed: $line"
		echo "Expected $answer, got $got"
	fi
}

function handle_file( )
{
	tfile=$1

	TRIED=0
	PASSED=0
	FAILED=0

	while IFS= read -r line; do
		handle_line $line
	done < $tfile

	echo "${header}Test File:  $tfile$reset"
	echo "${info}Tests:      $TRIED$reset"
	echo "${success}  Passed:   $PASSED$reset"
	echo "${error}  Failed:   $FAILED$reset"

	return $FAILED
}

FTOTAL=0
for f in ${tfiles[*]}; do
	handle_file $f
	FTOTAL=$(($FTOTAL + $?))
done

exit $FTOTAL

