/**************************************************************************
* Copyright 2024 John Denholm                                             *
*                                                                         *
* Licensed under the Apache License, Version 2.0 (the "License");         *
* you may not use this file except in compliance with the License.        *
* You may obtain a copy of the License at                                 *
*                                                                         *
*     http://www.apache.org/licenses/LICENSE-2.0                          *
*                                                                         *
* Unless required by applicable law or agreed to in writing, software     *
* distributed under the License is distributed on an "AS IS" BASIS,       *
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
* See the License for the specific language governing permissions and     *
* limitations under the License.                                          *
*                                                                         *
*                                                                         *
* help.c - help text                                                      *
*                                                                         *
* Updates:                                                                *
**************************************************************************/

#include "rpcalc.h"

void usage( void )
{
	printf( "\
Usage:  rpcalc ?\n\
        rpcalc @\n\
        rpcalc <args> ...\n\
        <cmd> | rpcalc\n\
\n\
Rpcalc is a simple reverse-polish calculator.  It is stack based.\n\
The arguments are listed below.  The numbers in brackets after the\n\
operators are the number of arguments they consume.  In each case\n\
the result is pushed back onto the stack.  When all arguments have\n\
been handled, the top stack element is reported.\n\
\n\
Maximum stack size is %d.\n\
\n\
All numeric arguments are treated as type long double.\n\
A (i) denotes integer conversion is applied before operations.\n\
\n\
 <Numbers>      These are consumed and pushed onto the stack.\n\
 +  (2)         Add the numbers together\n\
 -  (2)         Subtract the second number from the first\n\
 x  (2)         Multiply the numbers together (note, shells often eat *)\n\
 /  (2)         Divide the first number by the second number\n\
 %%  (2) (i)     First argument modulo the second argument\n\
rs  (1)         The square root of the argument\n\
rc  (1)         The cube root of the argument\n\
rn  (2) (i)     The n'th (second arg) root of the first argument\n\
nl  (1)         The natural log of the argument\n\
ne  (1)         The exponent of the argument (equiv to cE <x> ^)\n\
nL  (1)         Log with base 10 of the argument\n\
nE  (1)         Exponent base 10 of the argument\n\
nY  (2)         Log of arbitrary base (second argument)\n\
in  (1)         The inverse of the argument 1/X\n\
rd  (1)         Round the argument to the nearest integer\n\
rt  (1)         Round (truncate downwards) the argument\n\
 ^  (2)         Raise the first number to the power of the second\n\
 a  (1)         Absolute value of the argument\n\
sl  (2) (i)     The first argument left-shifted by the second\n\
sr  (2) (i)     The first argument right-shifted by the second\n\
NF  (1) (i)     Factorial of the number\n\
NP  (2) (i)     Permutations of the second number of choices from the first\n\
NC  (2) (i)     Combinations of the second number of choices from the first\n\
 F  (1) (i)     The n'th Fibonnaci sequence number\n\
EF  (1) (i)     Estimate the factorial of the number (for large numbers)\n\
EP  (2) (i)     Estimate of permutations (see NP) (for large numbers)\n\
EC  (2) (i)     Estimate of combinations (see NC) (for large numbers)\n\
SS  (stack)     The sum of all numbers currently on the stack\n\
SP  (stack)     The product of all numbers currently on the stack\n\
SG  (stack)     The geometric mean of all numbers currently on the stack\n\
SQ  (stack)     The quadratic mean (root mean square) of the entire stack\n\
SM  (stack)     The mean of all numbers currently on the stack\n\
SE  (stack)     The median value of all numbers currently on the stack\n\
SO  (stack)     The modal value of all the numebrs currently on the stack\n\
SD  (stack)     The std-dev of all numbers currently on the stack\n\
SU  (stack)     The highest of all numbers currently on the stack\n\
SL  (stack)     The lowest of all numbers currently on the stack\n\
SB  (stack)     The span of all numbers currently on the stack\n\
SX  (stack)     Unique-ify the stack (eliminate duplicates)\n\
SC  (stack)     Count the numbers on the stack\n\
Sg  (stack)     The greatest common divisor of the stack\n\
Sl  (stack)     The least common multiple of the stack\n\
y0-9 (1)        Store top of stack in non-stack memory\n\
Y0-9            Retrieve non-stack memory element and push to the stack\n\
ts  (1)         Trigonometry - sine of the argument\n\
tc  (1)         Trigonometry - cosine of the argument\n\
tt  (1)         Trigonometry - tangent of the argument\n\
tS  (1)         Trigonometry - arc sine of the argument\n\
tC  (1)         Trigonometry - arc cosine of the argument\n\
tT  (1)         Trigonometry - arc tangent of the argument\n", STACK_SIZE );
	// we hit the max literal string limit
	printf( "\
 R  (1)         Convert argument from degrees to radians\n\
 D  (1)         Convert argument from radians to degrees\n\
 g  (2)         Pushes the greater of the two arguments\n\
 l  (2)         Pushes the lesser of the two arguments\n\
 M  (2)         If the first %% the second is 0, push 0, else push 1\n\
 G  (2)         If the first number is greater push 1, else push 0\n\
 L  (2)         If the first number is lesser push 1, else push 0\n\
 =  (2)         If the two numbers are equal, push 1, else push 0\n\
 _  (1)         Converts 0 to 1, and non-zero to 0\n\
bn  (1) (i)     Bitwise NOT\n\
bN  (1) (i)     Bitwise NOT (32 bit)\n\
ba  (2) (i)     Bitwise AND\n\
bo  (2) (i)     Bitwise OR\n\
bx  (2) (i)     Bitwise XOR\n\
bm  (1) (i)     Mask to just argument lower bits\n\
bu  (2) (i)     Upshift first number by second number bits\n\
bd  (2) (i)     Downshift first number by second number bits\n\
 ,              Separator for numbers together in one argument\n\
fR              Push random floating point number [0, 1) (uses drand48)\n\
fS  (1) (i)     Set random number generator seed (uses srand48)\n\
ft              Push double-precision current timestamp (nsec/billion + sec)\n\
fT              Push current unix timestamp (sec)\n\
cE              Push mathematical constant: e (2.7818...)\n\
cP              Push mathematical constant: Pi (3.1416...)\n\
cG              Push mathematical constant: Phi (1.6180...)\n\
cM              Push mathematical constant: magic angle (0.9553...)\n\
cL              Push mathematical constant: lemniscate constant (2.622...)\n\
cD              Push mathematical constant: Dottie number (0.7390...)\n\
cQ              Push mathematical constant: Parabolic (2.2955...)\n\
cg              Push physical constant: gravitation (6.6e-11)\n\
cc              Push physical constant: speed of light (2.99e8)\n\
ch              Push physical constant: Planck constant (6.62e-34)\n\
ce              Push physical constant: elementary charge (1.602e-19)\n\
cm              Push physical constant: Avagadro's (6.022e23)\n\
cf              Push physical constant: fine structure (0.0072...)\n\
cb              Push physical constant: Boltzmann (1.3806e-23)\n\
cs              Push physical constant: Stefan-Boltzmann (5.6703e-8)\n\
oZ              Display output in scientific (exponent) form\n\
oI              Convert output to integer and display in decimal\n\
oU              Convert output to unsigned integer and display in decimal\n\
oB              Convert output to integer and display in binary\n\
oO              Convert output to unsigned integer and display in octal\n\
oX              Convert output to integer and display in hexadecimal\n\
ox              Convert output to unsigned integer and display in hexadecimal\n\
oR              Output options without prefixes (ie bare hex, bin, oct)\n\
ob              Output raw binary representation of the value\n\
 P  (1)         Set the output precision (0-24)\n\
IX              Disable hexadecimal input detection after this arg\n\
IO              Disable octal input detection after this arg\n\
IB              Disable binary input detection after this arg\n\
 .              Wait for stdin even with arguments supplied\n\
 ? or @         Print this help. (version %s)\n\n", RPCALC_VERSION );
}
