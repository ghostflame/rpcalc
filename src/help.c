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

char *help_text_base[] = {
	"\
Usage:  rpcalc ?\n\
        rpcalc @\n\
        rpcalc <args> ...\n\
        <cmd> | rpcalc\n\
\n",
"Rpcalc is a simple reverse-polish calculator.  It is stack based.\n\
The arguments are listed below.  The numbers in brackets after the\n\
operators are the number of arguments they consume.  In each case\n\
the result is pushed back onto the stack.  When all arguments have\n\
been handled, the top stack element is reported.\n\n",
	NULL
};



char *help_text_ops[] = {
"All numeric arguments are treated as type long double\n\
A (i) denotes integer conversion is applied before operations.\n\
\n",
	NULL
};



HEGR help_groups[] =
{
	{
		.key = '+',
		.texts = {
"<Numbers>      These are consumed and pushed onto the stack.",
" +  (2)         Add the numbers together",
" -  (2)         Subtract the second number from the first",
" x  (2)         Multiply the numbers together (note, shells often eat *)",
" /  (2)         Divide the first number by the second number",
" %  (2) (i)     First argument modulo the second argument",
" ^  (2)         Raise the first number to the power of the second",
" =  (2)         If the two numbers are equal, push 1, else push 0",
" _  (1)         Converts 0 to 1, and non-zero to 0",
NULL
		}
	},
	{
		.key = 'r',
		.texts = {
"rs  (1)         The square root of the argument",
"rc  (1)         The cube root of the argument",
"rn  (2) (i)     The n'th (second arg) root of the first argument",
"rd  (1)         Round the argument to the nearest integer",
"rt  (1)         Round (truncate downwards) the argument",
NULL
		}
	},
	{
		.key = 'n',
		.texts = {
"nl  (1)         The natural log of the argument",
"ne  (1)         The exponent of the argument (equiv to cE <x> ^)",
"nL  (1)         Log with base 10 of the argument",
"nE  (1)         Exponent base 10 of the argument",
"nY  (2)         Log of arbitrary base (second argument)",
NULL
		}
	},
	{
		.key = 'i',
		.texts = {
"in  (1)         The inverse of the argument 1/X",
NULL
		}
	},
	{
		.key = 'a',
		.texts = {
" a  (1)         Absolute value of the argument",
NULL
		}
	},
	{
		.key = 's',
		.texts = {
"sl  (2) (i)     The first argument left-shifted by the second",
"sr  (2) (i)     The first argument right-shifted by the second",
NULL
		}
	},
	{
		.key = 'N',
		.texts = {
"NF  (1) (i)     Factorial of the number",
"NP  (2) (i)     Permutations of the second number of choices from the first",
"NC  (2) (i)     Combinations of the second number of choices from the first",
NULL
		}
	},
	{
		.key = 'F',
		.texts = {
" F  (1) (i)     The n'th Fibonnaci sequence number",
NULL
		}
	},
	{
		.key = 'E',
		.texts = {
"EF  (1) (i)     Estimate the factorial of the number (for large numbers)",
"EP  (2) (i)     Estimate of permutations (see NP) (for large numbers)",
"EC  (2) (i)     Estimate of combinations (see NC) (for large numbers)",
NULL
		}
	},
	{
		.key = 'S',
		.texts = {
"SS  (stack)     The sum of all numbers currently on the stack",
"SP  (stack)     The product of all numbers currently on the stack",
"SG  (stack)     The geometric mean of all numbers currently on the stack",
"SQ  (stack)     The quadratic mean (root mean square) of the entire stack",
"SM  (stack)     The mean of all numbers currently on the stack",
"SE  (stack)     The median value of all numbers currently on the stack",
"SO  (stack)     The modal value of all the numebrs currently on the stack",
"SD  (stack)     The std-dev of all numbers currently on the stack",
"SU  (stack)     The highest of all numbers currently on the stack",
"SL  (stack)     The lowest of all numbers currently on the stack",
"SB  (stack)     The span of all numbers currently on the stack",
"SX  (stack)     Unique-ify the stack (eliminate duplicates)",
"SC  (stack)     Count the numbers on the stack",
"Sg  (stack)     The greatest common divisor of the stack",
"Sl  (stack)     The least common multiple of the stack",
NULL
		}
	},
	{
		.key = 'y',
		.texts = {
"y0-9 (1)        Store top of stack in non-stack memory",
"Y0-9            Retrieve non-stack memory element and push to the stack",
NULL
		}
	},
	{
		.key = 't',
		.texts = {
"ts  (1)         Trigonometry - sine of the argument",
"tc  (1)         Trigonometry - cosine of the argument",
"tt  (1)         Trigonometry - tangent of the argument",
"tS  (1)         Trigonometry - arc sine of the argument",
"tC  (1)         Trigonometry - arc cosine of the argument",
"tT  (1)         Trigonometry - arc tangent of the argument",
NULL
		}
	},
	{
		.key = 'R',
		.texts = {
" R  (1)         Convert argument from degrees to radians",
NULL
		}
	},
	{
		.key = 'D',
		.texts = {
" D  (1)         Convert argument from radians to degrees",
NULL
		}
	},
	{
		.key = 'g',
		.texts = {
" g  (2)         Pushes the greater of the two arguments",
NULL
		}
	},
	{
		.key = 'l',
		.texts = {
" l  (2)         Pushes the lesser of the two arguments",
NULL
		}
	},
	{
		.key = 'M',
		.texts = {
" M  (2)         If the first %% the second is 0, push 0, else push 1",
NULL
		}
	},
	{
		.key = 'G',
		.texts = {
" G  (2)         If the first number is greater push 1, else push 0",
NULL
		}
	},
	{
		.key = 'L',
		.texts = {
" L  (2)         If the first number is lesser push 1, else push 0",
NULL
		}
	},
	{
		.key = 'b',
		.texts = {
"bn  (1) (i)     Bitwise NOT",
"bN  (1) (i)     Bitwise NOT (32 bit)",
"ba  (2) (i)     Bitwise AND",
"bo  (2) (i)     Bitwise OR",
"bx  (2) (i)     Bitwise XOR",
"bm  (1) (i)     Mask to just argument lower bits",
"bu  (2) (i)     Upshift first number by second number bits",
"bd  (2) (i)     Downshift first number by second number bits",
NULL
		}
	},
	{
		.key = 'p',
		.texts = {
"pT  (1) (i)     If the number is prime, push 1, else 0",
"pR  (2) (i)     If the two numbers are relatively prime, push 1, else 0",
"pn  (1) (i)     Push the highest prime below the number to the stack",
"pN  (1) (i)     Push the next highest prime onto the stack",
NULL
		}
	},
	{
		.key = 'f',
		.texts = {
"fR              Push random floating point number [0, 1) (uses drand48)",
"fS  (1) (i)     Set random number generator seed (uses srand48)",
"ft              Push double-precision current timestamp (nsec/billion + sec)",
"fT              Push current unix timestamp (sec)",
NULL
		}
	},
	{
		.key = 'c',
		.texts = {
"cE              Push mathematical constant: e (2.7818...)",
"cP              Push mathematical constant: Pi (3.1416...)",
"cG              Push mathematical constant: Phi (1.6180...)",
"cM              Push mathematical constant: magic angle (0.9553...)",
"cL              Push mathematical constant: lemniscate constant (2.622...)",
"cD              Push mathematical constant: Dottie number (0.7390...)",
"cQ              Push mathematical constant: Parabolic (2.2955...)",
"cg              Push physical constant: gravitation (6.6e-11)",
"cc              Push physical constant: speed of light (2.99e8)",
"ch              Push physical constant: Planck constant (6.62e-34)",
"ce              Push physical constant: elementary charge (1.602e-19)",
"cm              Push physical constant: Avagadro's (6.022e23)",
"cf              Push physical constant: fine structure (0.0072...)",
"cb              Push physical constant: Boltzmann (1.3806e-23)",
"cs              Push physical constant: Stefan-Boltzmann (5.6703e-8)",
NULL
		}
	},
	{
		.key = 'o',
		.texts = {
"oZ              Display output in scientific (exponent) form",
"oI              Convert output to integer and display in decimal",
"oU              Convert output to unsigned integer and display in decimal",
"oB              Convert output to integer and display in binary",
"oO              Convert output to unsigned integer and display in octal",
"oX              Convert output to integer and display in hexadecimal",
"ox              Convert output to unsigned integer and display in hexadecimal",
"oR              Output options without prefixes (ie bare hex, bin, oct)",
"ob              Output raw binary representation of the value",
NULL
		}
	},
	{
		.key = 'P',
		.texts = {
" P  (1)         Set the output precision (0-24)",
NULL
		}
	},
	{
		.key = 'I',
		.texts = {
"IX              Disable hexadecimal input detection after this arg",
"IO              Disable octal input detection after this arg",
"IB              Disable binary input detection after this arg",
NULL
		}
	},
	{
		.key = ',',
		.texts = {
" ,              Separator for numbers together in one argument",
NULL
		}
	},
	{
		.key = '.',
		.texts = {
" .              Wait for stdin even with arguments supplied",
NULL
		}
	},
	{
		.key = '\0',
		.texts = { NULL }
	}
};

void show_help_group( HEGR *hg )
{
	char **p;

	p = &(hg->texts[0]);

	for( ; *p; p++ )
		printf( "%s\n", *p );
}


void usage( char grp, int exval )
{
	HEGR *h;
	char **p;

	for( p = help_text_base; *p; p++ )
		printf( "%s", *p );

	printf( "Max stack size is %d.\n\n", STACK_SIZE );

	for( p = help_text_ops; *p; p++ )
		printf( "%s", *p );

	if( grp )
	{
		for( h = &(help_groups[0]); h->key > '\0'; ++h )
			if( h->key == grp )
			{
				show_help_group( h );
				break;
			}
	}
	else
		for( h = &(help_groups[0]); h->key > '\0'; ++h )
			show_help_group( h );

	printf( " ?              Print this help\n" );
	printf( " @<key>         Print help filtered by first character is <key>\n" );
	printf( "\n(Rpcalc version %s)\n\n", RPCALC_VERSION );

	exit( exval );
}
