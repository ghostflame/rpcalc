#!/bin/bash

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

function handle_file( )
{
	tfile=$1

	TRIED=0
	PASSED=0
	FAILED=0

	while IFS= read -r line; do
		handle_line $line
	done < $tfile

	echo "Test File:  $tfile"
	echo "Tests:      $TRIED"
	echo "  Passed:   $PASSED"
	echo "  Failed:   $FAILED"

	return $FAILED
}

FTOTAL=0
for f in ${tfiles[*]}; do
	echo $f
	handle_file $f
	FTOTAL=$(($FTOTAL + $?))
done

exit $FTOTAL

