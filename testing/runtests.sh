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

tfiles=("Rpcalc-Overall.tst" "Rpcalc-Trig.tst" "Rpcalc-Bin.tst" "Rpcalc-Stack.tst")
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

function report( )
{
	t=$1
	p=$2
	f=$3

	echo "${info}Tests:      ${t}${reset}"
	echo "${success}  Passed:   ${p}${reset}"
	if [ $f -gt 0 ]; then
		echo "${error}  Failed:   ${f}${reset}"
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
	report $TRIED $PASSED $FAILED

	return $FAILED
}

function handle_pipes( )
{
	TRIED=0
	PASSED=0
	FAILED=0

	ret=$(echo "1 2 3 4 SS" | ${bin})
	((TRIED++))
	if [ "$ret" == "10.000000" ]; then
		((PASSED++))
	else
		((FAILED++))
	fi

	ret=$(echo "1 2 3 4 SS" | ${bin} . oI)
	((TRIED++))
	if [ "$ret" == "10" ]; then
		((PASSED++))
	else
		((FAILED++))
	fi

	echo "${header}Pipes Test${reset}"
	report $TRIED $PASSED $FAILED
}

FTOTAL=0
for f in ${tfiles[*]}; do
	handle_file $f
	FTOTAL=$(($FTOTAL + $?))
done
handle_pipes

exit $FTOTAL

