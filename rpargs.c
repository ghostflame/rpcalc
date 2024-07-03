#include "rpcalc.h"

void usage( void )
{
	printf( "\
Usage:  rpcalc ?\n\
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
 * or x  (2)    Multiply the numbers together (note, shells often eat *)\n\
 /  (2)         Divide the first number by the second number\n\
rs  (1)         The square root of the argument\n\
rc  (1)         The cube root of the argument\n\
rn  (2) (i)     The n'th (second arg) root of the first argument\n\
nl  (1)         The natural log of the argument\n\
ne  (1)         The exponent of the argument\n\
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
Ts  (1)         Trigonometry - arc sine of the argument\n\
Tc  (1)         Trigonometry - arc cosine of the argument\n\
Tt  (1)         Trigonometry - arc tangent of the argument\n\
 R  (1)         Convert argument from degrees to radians\n\
 D  (1)         Convert argument from radians to degrees\n\
 g  (2)         Pushes the greater of the two arguments\n\
 l  (2)         Pushes the lesser of the two arguments\n\
 m  (2)         The first argument modulo the second\n\
 M  (2)         If the first %% the second is 0, push 0, else push 1\n\
 G  (2)         If the first number is greater push 1, else push 0\n\
 L  (2)         If the first number is lesser push 1, else push 0\n\
 =  (2)         If the two numbers are equal, push 1, else push 0\n\
 _  (1)         Converts 0 to 1, and non-zero to 0\n", STACK_SIZE );
	// we hit the max literal string limit
	printf( "\
bn  (1) (i)     Bitwise NOT\n\
ba  (2) (i)     Bitwise AND\n\
bo  (2) (i)     Bitwise OR\n\
bx  (2) (i)     Bitwise XOR\n\
 ,              Separator for numbers together in one argument\n\
fR              Push random floating point number [0, 1) (uses drand48)\n\
cE              Push mathematical constant: e (2.7818...)\n\
cP              Push mathematical constant: Pi (3.1416...)\n\
cG              Push mathematical constant: Phi (1.6180...)\n\
cg              Push physical constant: gravitation (6.6e-11)\n\
cc              Push physical constant: speed of light (2.99e8)\n\
ch              Push physical constant: Planck constant (6.62e-34)\n\
ce              Push physical constant: elementary charge (1.602e-19)\n\
cm              Push physical constant: Avagadro's (6.022e23)\n\
oZ              Display output in scientific (exponent) form\n\
oI              Convert output to integer and display in decimal\n\
oB              Convert output to integer and display in binary\n\
oO              Convert output to integer and display in octal\n\
oX              Convert output to integer and display in hexadecimal\n\
oR              Output options without prefixes (ie bare hex, bin, oct)\n\
ob              Output raw binary representation of the value\n\
 P  (1)         Set the output precision (0-12)\n\
IX              Disable hexadecimal input detection after this arg\n\
IO              Disable octal input detection after this arg\n\
IB              Disable binary input detection after this arg\n\
 %%              Wait for stdin even with arguments supplied\n\
 ?              Print this help.\n\n" );
}


void handle_arg( STACK *s, char *arg )
{
	long double a, b, c;
	long long int j, k;
	char *p, *q;
	int st_off;

	for( p = arg; *p; p++ )
	{
		// handle whitespace
		while( *p && isspace( *p ) )
			p++;

		// null is ignored
		switch( *p )
		{
			case '\0':
				break;

			case '0':
				// detect hex and octal
				switch( *(p+1) )
				{
					// hex, be generous and allow X
					case 'x':
					case 'X':
						// step over prefix
						p++;
						p++;
						if( hasinput( s, INTYPE_HEX ) )
						{
							j = strtoull( p, &q, 16 );
							p = q - 1;
							push( s, (long double) j );
						}
						else
						{
							a = strtold( p, &q );
							p = q - 1;
							push( s, a );
						}
						break;
					// binary - allow 0b and 0B
					case 'b':
					case 'B':
						// step over prefix
						p++;
						p++;
						if( hasinput( s, INTYPE_BIN ) )
						{
							j = strtoull( p, &q, 2 );
							p = q - 1;
							push( s, (long double) j );
						}
						else
						{
							a = strtold( p, &q );
							p = q - 1;
							push( s, a );
						}
						break;
					// octal - all 0123 or 0o123 or 0O123
					case 'o':
					case 'O':
						// step over prefix
						p++;
						p++;
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
						if( hasinput( s, INTYPE_OCT ) )
						{
							j = strtoull( p, &q, 8 );
							p = q - 1;
							push( s, (long double) j );
						}
						else
						{
							a = strtold( p, &q );
							p = q - 1;
							push( s, a );
						}
						break;
					// 8 and 9 are just regular numbers,
					// so allow long doubles, and handle decimals
					case '8':
					case '9':
					case '.':
						a = strtold( p, &q );
						p = q - 1;
						push( s, a );
						break;
					// else... it was input
					default:
						push( s, 0 );
						break;
				}
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				a = strtold( p, &q );
				p = q - 1;
				push( s, a );
				break;

			case '+':
				need( s, 2 );
				pop( s, &a, &b );
				a += b;
				push( s, a );
				break;

			case '-':
				if( *(p+1) >= '0' && *(p+1) <= '9' )
				{
					a = strtold( p, &q );
					p = q - 1;
					push( s, a );
					break;
				}

				need( s, 2 );
				pop( s, &a, &b );
				push( s, a - b );
				break;

			case 'x':	// to stop the shell grabbing *
			case '*':
				need( s, 2 );
				pop( s, &a, &b );
				push( s, a * b );
				break;

			case '/':
				need( s, 2 );
				pop( s, &a, &b );
				push( s, a / b );
				break;

			case 'i':
				p++;
				switch( *p )
				{
					case 'n':
						need( s, 1 );
						pop( s, &a, NULL );
						if( a == 0.0L )
							BROKEN( );
						push( s, 1.0L / a );
						break;

					default:
						BROKEN( );
				}

				break;

			case 'r':
				p++;
				switch( *p )
				{
					case 's':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, sqrtl( a ) );
						break;

					case 'c':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, cbrtl( a ) );
						break;

					case 'n':
						need( s, 2 );
						pop( s, &a, &b );
						push( s, nth_root( a, b ) );
						break;

					case 'd':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, roundl( a ) );
						break;

					case 't':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, truncl( a ) );
						break;

					default:
						BROKEN( );
				}
				break;

			case 'n':
				p++;
				switch( *p )
				{
					case 'l':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, logl( a ) );
						break;

					case 'e':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, expl( a ) );
						break;

					default:
						BROKEN( );
				}
				break;

			case 't':
				p++;
				switch( *p )
				{
					case 's':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, sinl( a ) );
						break;

					case 'c':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, cosl( a ) );
						break;

					case 't':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, tanl( a ) );
						break;

					default:
						BROKEN( );
				}
				break;

			case 'T':
				p++;
				switch( *p )
				{
					case 's':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, asinl( a ) );
						break;

					case 'c':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, acosl( a ) );
						break;

					case 't':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, atanl( a ) );
						break;

					default:
						BROKEN( );
				}
				break;

			case 'R':
				need( s, 1 );
				pop( s, &a, NULL );
				push( s, ( a * M_PIl ) / 180.0L );
				break;

			case 'D':
				need( s, 1 );
				pop( s, &a, NULL );
				push( s, ( a * 180.0L ) / M_PIl );
				break;

			case 'a':
				need( s, 1  );
				pop( s, &a, NULL );
				push( s, fabsl( a ) );
				break;

			case 'c':
				p++;
				switch( *p )
				{
					case 'E':
						push( s, M_El );
						break;

					case 'P':
						push( s, M_PIl );
						break;

					case 'g':
						push( s, RPCST_GRAV );
						break;

					case 'c':
						push( s, RPCST_LIGHT );
						break;

					case 'h':
						push( s, RPCST_PLANCK );
						break;

					case 'm':
						push( s, RPCST_AVAGADRO );
						break;

					case 'e':
						push( s, RPCST_CHARGE );
						break;

					default:
						BROKEN( );
				}
				break;

			case '^':
				need( s, 2 );
				pop( s, &a, &b );
				push( s, powl( a, b ) );
				break;

			case 's':
				p++;
				switch( *p )
				{
					case 'l':
						need( s, 2 );
						pop( s, &a, &b );
						j = (long long int) a;
						j <<= (int) b;
						push( s, (long double) j );
						break;

					case 'r':
						need( s, 2 );
						pop( s, &a, &b );
						j = (long long int) a;
						j >>= (int) b;
						push( s, (long double) j );
						break;

					default:
						BROKEN( );
				}
				break;

			case 'm':
				need( s, 2 );
				pop( s, &a, &b );
				j = (long long int) a;
				k = (long long int) b;
				push( s, (long double) ( j % k ) );
				break;

			case 'M':
				need( s, 2 );
				pop( s, &a, &b );
				j = (long long int) a;
				k = (long long int) b;
				if( ( j % k ) == 0 )
					a = 0.0;
				else
					a = 1.0;
				push( s, a );
				break;

			case 'Y':
				p++;
				switch( *p )
				{
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						st_off = *p - '0';
						if( get_mem( s, st_off, &a ) != 0 )
						{
							BROKEN( );
						}
						push( s, a );
						break;

					default:
						BROKEN( );
				}
				break;

			case 'y':
				p++;
				switch( *p )
				{
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						st_off = *p - '0';
						need( s, 1 );
						pop( s, &a, NULL );
						set_mem( s, st_off, a );
						break;

					default:
						BROKEN( );
				}
				break;

			case 'F':
				need( s, 1 );
				pop( s, &a, NULL);
				j = (long long int) a;
				push( s, fibonnaci( j ) );
				if( j < 93 )
					setoutput( s, OUTTYPE_INT );
				break; 

			case 'N':
				p++;
				switch( *p )
				{
					case 'F':
						need( s, 1 );
						pop( s, &a, NULL );
						PLIM( a, LIM_FACT_I );
						j = (long long int) a;
						a = (long double) getfact( s, (int) j );
						push( s, a );
						break;

					case 'P':
						need( s, 2 );
						pop( s, &a, &b );
						PLIM( a, LIM_FACT_I );
						PLIM( b, LIM_FACT_I );
						j = (long long int) a;
						k = (long long int) b;
						a = (long double) perms( s, j, k );
						push( s, a );
						break;

					case 'C':
						need( s, 2 );
						pop( s, &a, &b );
						PLIM( a, LIM_FACT_I );
						PLIM( b, LIM_FACT_I );
						j = (long long int) a;
						k = (long long int) b;
						a = (long double) comb( s, j, k );
						push( s, a );
						break;

					default:
						BROKEN( );
				}
				break;

			case 'E':
				p++;
				switch( *p )
				{
					case 'F':
						need( s, 1 );
						pop( s, &a, NULL );
						PLIM( a, LIM_FACT_E );
						j = (long long int) a;
						a = est_fact( j );
						push( s, expl( a ) );
						break;

					case 'P':
						need( s, 2 );
						pop( s, &a, &b );
						PLIM( a, LIM_FACT_E );
						PLIM( b, LIM_FACT_E );
						j = (long long int) a;
						k = (long long int) b;
						a = est_perms( j, k );
						push( s, expl( a ) );
						break;

					case 'C':
						need( s, 2 );
						pop( s, &a, &b );
						PLIM( a, LIM_FACT_E );
						PLIM( b, LIM_FACT_E );
						j = (long long int) a;
						k = (long long int) b;
						a = est_comb( j, k );
						push( s, expl( a ) );
						break;

					default:
						BROKEN( );
				}

				setoutput( s, OUTTYPE_SCI );
				break;

			case 'S':
				p++;
				switch( *p )
				{
					case 'C':
						a = (long double) current( s );
						flatten( s );
						push( s, a );
						break;

					case 'S':
						b = 0;
						while( !empty( s ) )
						{
							pop( s, &a, NULL );
							b += a;
						}
						push( s, b );
						break;

					case 'P':
						b = 1;
						while( !empty( s ) )
						{
							pop( s, &a, NULL );
							b *= a;
						}
						push( s, b );
						break;

					case 'G':
						a = 0;
						if( ( j = current( s ) ) == 0 )
							j = 1;
						while( !empty( s ) )
						{
							pop( s, &b, NULL );
							a += log( b );
						}
						b = a / ( (long double) j );
						push( s, exp( b ) );
						break;

					case 'Q':
						a = 0;
						if( ( j = current( s ) ) == 0 )
							j = 1;
						while( !empty( s ) )
						{
							pop( s, &b, NULL );
							a += b * b;
						}
						a = sqrt( a / (long double) j );
						push( s, a );
						break;

					case 'M':
						a = 0;
						if( ( j = current( s ) ) == 0 )
							j = 1;
						while( !empty( s ) )
						{
							pop( s, &b, NULL );
							a += b;
						}
						a /= (long double) j;
						push( s, a );
						break;

					case 'D':
						stack_sd( s );
						//report( s );
						break;

					case 'E':
						stack_median( s );
						break;

					case 'X':
						stack_unique( s );
						break;

					case 'g':
						stack_gcd( s );
						break;

					case 'l':
						stack_lcm( s );
						break;

					case 'U':
						need( s, 1 );
						pop( s, &b, NULL );
						a = b;
						while( !empty( s ) )
						{
							pop( s, &b, NULL );
							if( b > a )
								a = b;
						}
						push( s, a );
						break;

					case 'L':
						need( s, 1 );
						pop( s, &b, NULL );
						a = b;
						while( !empty( s ) )
						{
							pop( s, &b, NULL );
							if( b < a )
								a = b;
						}
						push( s, a );
						break;

					case 'B':
						need( s, 1 );
						pop( s, &b, NULL );
						a = c = b;
						while( !empty( s ) )
						{
							pop( s, &b, NULL );
							if( b < c )
							 	c = b;
							else if( b > a )
								a = b;
						}
						push( s, a - c );
						break;

					default:
						BROKEN( );
				}
				break;

			case '_':
				need( s, 1 );
				pop( s, &b, NULL );
				if( b < 0 || b > 0 )
					a = 0;
				else
					a = 1;
				push( s, a );
				break;

			// various functions
			case 'f':
				p++;
				switch( *p )
				{
					// random long double 0 <= ld < 1
					case 'R':
						a = get_random_ld( );
						push( s, a );
						break;

					default:
						BROKEN();
				}
				break;

			case 'b':
				p++;
				switch( *p )
				{
					case 'n':
						need( s, 1 );
						pop( s, &b, NULL );
						a = (long double) ~((int) b);
						push( s, a );
						break;

					case 'a':
						need( s, 2 );
						pop( s, &a, &b );
						j = (int) a;
						k = (int) b;
						a = (long double) ( j & k );
						push( s, a );
						break;

					case 'o':
						need( s, 2 );
						pop( s, &a, &b );
						j = (int) a;
						k = (int) b;
						a = (long double) ( j | k );
						push( s, a );
						break;

					case 'x':
						need( s, 2 );
						pop( s, &a, &b );
						j = (int) a;
						k = (int) b;
						a = (long double) ( j ^ k );
						push( s, a );
						break;

					default:
						BROKEN( );
				}
				break;

			case '=':
				need( s, 2 );
				pop( s, &a, &b );
				if( a == b )
					a = 1;
				else
					a = 0;
				push( s, a );
				break;

			case 'G':
				need( s, 2 );
				pop( s, &a, &b );
				if( a > b )
					a = 1;
				else
					a = 0;
				push( s, a );
				break;

			case 'L':
				need( s, 2 );
				pop( s, &a, &b );
				if( a < b )
					a = 1;
				else
					a = 0;
				push( s, a );
				break;

			case 'g':
				need( s, 2 );
				pop( s, &a, &b );
				if( b > a )
					a = b;
				push( s, a );
				break;

			case 'l':
				need( s, 2 );
				pop( s, &a, &b );
				if( b < a )
					a = b;
				push( s, a );
				break;

			case 'I':
				p++;
				for( ; *p && !isspace( *p ); p++ )
					switch( *p )
					{
						case 'O':
							setinput( s, INTYPE_OCT, 0 );
							break;

						case 'X':
							setinput( s, INTYPE_HEX, 0 );
							break;

						case 'B':
							setinput( s, INTYPE_BIN, 0 );
							break;

						default:
							BROKEN( );
					}

				// step back off the null
				--p;
				break;

			case 'o':
				p++;
				for( ; *p && !isspace( *p ); p++ )
					switch( *p )
					{
						case 'I':
							setoutput( s, OUTTYPE_INT );
							break;

						case 'Z':
							setoutput( s, OUTTYPE_SCI );
							break;

						case 'X':
							setoutput( s, OUTTYPE_HEX );
							break;

						case 'O':
							setoutput( s, OUTTYPE_OCT );
							break;

						case 'B':
							setoutput( s, OUTTYPE_BIN );
							break;

						case 'b':
							setoutput( s, OUTTYPE_DBIN );
							break;

						case 'R':
							setbare( s, 1 );
							break;

						default:
							BROKEN( );
					}

				// step back off the null
				--p;
				break;

			case 'P':
				need( s, 1 );
				pop( s, &a, NULL );
				setprecision( s, (int) a );
				break;

			case ',':
				// number separator
				break;

			case '%':
				// stdin, despite commandline args
				setinput( s, INTYPE_TERM, 1 );
				break;

			case '?':
				usage( );
				exit( 0 );
				break;

			default:
				BROKEN( );
				break;
		}
	}
}
