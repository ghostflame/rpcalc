#!/bin/bash

tfile=Rpcalc-Tests.txt
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

while IFS= read -r line; do
	handle_line $line
done < $tfile

echo "Tests:      $TRIED"
echo "  Passed:   $PASSED"
echo "  Failed:   $FAILED"

exit $FAILED


