#define _GNU_SOURCE

#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STACK_SIZE		262144
#define MAX_FACT		21

#define PUSH( )			stack_vals[st_pos++] = a; if( st_pos >= STACK_SIZE ) exit( fprintf( stderr, "Stack limit reached.\n" ) )
#define POP( x )		x = stack_vals[--st_pos]
#define PEEK( x )		if( st_pos < x ) exit( fprintf( stderr, "Cannot peek beyond stack size.\n" ) ); b = stack_vals[--x]

#define NEED( x )		if( st_pos < x ) exit( fprintf( stderr, "Invalid formula.  Try a ?\n" ) ); POP( b ); if( x == 2 ) POP( a )
#define BROKEN( )		exit( fprintf( stderr, "Invalid formula.  Try a ?\n" ) )
#define STATE( )		printf( "a = %f, b = %f\n", a, b )

static double stack_vals[STACK_SIZE];
int st_pos = 0;

int64_t factorials[MAX_FACT];

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
All numeric arguments are treated as type double.\n\
A (i) denotes integer conversion is applied before operations.\n\
\n\
 <Numbers>      These are consumed and pushed onto the stack.\n\
 +  (2)         Add the numbers together\n\
 -  (2)         Subtract the second number from the first\n\
 * or x  (2)    Multiply the numbers together (note, shells often eat *)\n\
 /  (2)         Divide the first number by the second number\n\
rs  (1)         The square root of the argument\n\
rc  (1)         The cube root of the argument\n\
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
 m  (2)         The first argument module the second\n\
 M  (2)         If the first %% the second is 0, push 0, else push 1\n\
 G  (2)         If the first number is greater push 1, else push 0\n\
 L  (2)         If the first number is lesser push 1, else push 0\n\
 =  (2)         If the two numbers are equal, push 1, else push 0\n\
 _  (1)         Converts 0 to 1, and non-zero to 0\n\
bn  (1) (i)     Bitwise NOT\n\
ba  (2) (i)     Bitwise AND\n\
bo  (2) (i)     Bitwise OR\n\
bx  (2) (i)     Bitwise XOR\n\
 ,  (0)         Separator for numbers together in one argument\n\
cE              Push mathematical constant e (2.7818...)\n\
cP              Push mathematical constant Pi (3.1416...)\n\
 I  (0)         Convert output to integer first\n\
 ?  (0)         Print this help.\n\n" );
}


int64_t factorial( int64_t f )
{
	if( f < 2 )
		return 1;

	if( f >= MAX_FACT )
		return -1;

	if( !factorials[f] )
		factorials[f] = f * factorial( f - 1 );

	return factorials[f];
}

int64_t perms( int64_t a, int64_t b )
{
	if( b > a )
		return -1;

	return factorial( a ) / factorial( b );
}

int64_t comb( int64_t a, int64_t b )
{
	if( b > a )
		return -1;

	return perms( a, b ) / factorial( a - b );
}

int stack_compare( const void *p1, const void *p2 )
{
	double d1 = *((double *) p1);
	double d2 = *((double *) p2);

	return ( d1 > d2 ) ? 1 : ( d1 < d2 ) ? -1 : 0;
}

void stack_median( void )
{
	double *tmpstack;

	if( st_pos == 0 )
		return;

	tmpstack = calloc( st_pos, sizeof( double ) );
	memcpy( tmpstack, stack_vals, st_pos * sizeof( double ) );
	qsort( tmpstack, st_pos, sizeof( double ), &stack_compare );

	stack_vals[0] = tmpstack[st_pos >> 1];
	st_pos = 1;

	free( tmpstack );
}


void stack_unique( void )
{
	double a, spare_stack[STACK_SIZE];
	int i, j = 0;

	while( st_pos > 0 )
	{
		POP( a );
		for( i = 0; i < j; ++i )
			if( spare_stack[i] == a )
				break;

		if( i == j )
		{
			spare_stack[j++] = a;
		}
	}

	for( i = j; i > 0; --i )
	{
		a = spare_stack[i - 1];
		PUSH( );
	}
}



void stack_sd( void )
{
	double m, s, a, b;
	int j;

	m = 0;
	j = st_pos;

	while( j > 0 )
	{
		PEEK( j );
		m += b;
	}
	m /= (double) st_pos;
	// m is now the mean
	a = m;

	j = st_pos;
	s = 0;

	while( st_pos > 0 )
	{
		POP( b );
		b -= m;
		s += b * b;
	}
	s /= (double) j;
	s = sqrt( s );
	// s is now the root mean square diff

	// push both the mean and stddev

	a = m;
	PUSH( );
	a = s;
	PUSH( );
}



int int_out = 0;

void report( void )
{
	double a, b;
	int64_t j;

	NEED( 1 );
	a = b;

	if( int_out )
	{
		j = (int64_t) a;
		printf( "%ld\n", j );
	}
	else
		printf( "%f\n", a );
}


void handle_arg( char *arg )
{
	int64_t j, k;
	double a, b, c;
	char *p, *q;

	for( p = arg; *p; p++ )
	{
		// handle whitespace
		while( *p && isspace( *p ) )
			p++;

		// null is ignored
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
				a = strtod( p, &q );
				p = q - 1;
				PUSH( );
				break;

			case '+':
				NEED( 2 );
				a += b;
				PUSH( );
				break;

			case '-':
				if( *(p+1) >= '0' && *(p+1) <= '9' )
				{
					a = strtod( p, &q );
					p = q - 1;
					PUSH( );
					break;
				}

				NEED( 2 );
				a -= b;
				PUSH( );
				break;

			case 'x':	// to stop the shell grabbing *
			case '*':
				NEED( 2 );
				a *= b;
				PUSH( );
				break;

			case '/':
				NEED( 2 );
				a /= b;
				PUSH( );
				break;

			case 'i':
				p++;
				switch( *p )
				{
					case 'n':
						NEED( 1 );
						if( b == 0.0 )
							BROKEN( );
						a = 1.0/b;
						PUSH( );
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
						NEED( 1 );
						a = sqrt( b );
						PUSH( );
						break;

					case 'c':
						NEED( 1 );
						a = cbrt( b );
						PUSH( );
						break;

					case 'd':
						NEED( 1 );
						a = round( b );
						PUSH( );
						break;

					case 't':
						NEED( 1 );
						a = trunc( b );
						PUSH( );
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
						NEED( 1 );
						a = log( b );
						PUSH( );
						break;

					case 'e':
						NEED( 1 );
						a = exp( b );
						PUSH( );
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
						NEED( 1 );
						a = sin( b );
						PUSH( );
						break;

					case 'c':
						NEED( 1 );
						a = cos( b );
						PUSH( );
						break;

					case 't':
						NEED( 1 );
						a = tan( b );
						PUSH( );
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
						NEED( 1 );
						a = asin( b );
						PUSH( );
						break;

					case 'c':
						NEED( 1 );
						a = acos( b );
						PUSH( );
						break;

					case 't':
						NEED( 1 );
						a = atan( b );
						PUSH( );
						break;

					default:
						BROKEN( );
				}
				break;

			case 'R':
				NEED( 1 );
				a = ( b * M_PI ) / 180.0;
				PUSH( );
				break;

			case 'D':
				NEED( 1 );
				a = ( b * 180.0 ) / M_PI;
				PUSH( );
				break;

			case 'a':
				NEED( 1 );
				a = fabs( b );
				PUSH( );
				break;

			case 'c':
				p++;
				switch( *p )
				{
					case 'E':
						a = M_E;
						PUSH( );
						break;

					case 'P':
						a = M_PI;
						PUSH( );
						break;

					default:
						BROKEN( );
				}
				break;

			case '^':
				NEED( 2 );
				a = pow( a, b );
				PUSH( );
				break;

			case 's':
				p++;
				switch( *p )
				{
					case 'l':
						NEED( 2 );
						j = (int64_t) a;
						STATE( );
						j <<= (int) b;
						a = (double) j;
						PUSH( );
						break;

					case 'r':
						NEED( 2 );
						j = (int64_t) a;
						j >>= (int) b;
						a = (double) j;
						PUSH( );
						break;

					default:
						BROKEN( );
				}
				break;

			case 'm':
				NEED( 2 );
				j = (int64_t) a;
				k = (int64_t) b;
				a = (double) ( j % k );
				PUSH( );
				break;

			case 'M':
				NEED( 2 );
				j = (int64_t) a;
				k = (int64_t) b;
				if( ( j % k ) == 0 )
					a = 0.0;
				else
					a = 1.0;
				PUSH( );
				break;

			case 'N':
				p++;
				switch( *p )
				{
					case 'F':
						NEED( 1 );
						j = (int64_t) b;
						a = (double) factorial( j );
						PUSH( );
						break;

					case 'P':
						NEED( 2 );
						j = (int64_t) a;
						k = (int64_t) b;
						a = (double) perms( j, k );
						PUSH( );
						break;

					case 'C':
						NEED( 2 );
						j = (int64_t) a;
						k = (int64_t) b;
						a = (double) comb( j, k );
						PUSH( );
						break;

					default:
						BROKEN( );
				}
				break;

			case 'S':
				p++;
				switch( *p )
				{
					case 'C':
						a = (double) st_pos;
						st_pos = 0;
						PUSH( );
						break;

					case 'S':
						a = 0;
						while( st_pos > 0 )
						{
							POP( b );
							a += b;
						}
						PUSH( );
						break;

					case 'P':
						a = 1;
						while( st_pos > 0 )
						{
							POP( b );
							a *= b;
						}
						PUSH( );
						break;

					case 'G':
						a = 0;
						if( !( j = st_pos ) )
							j = 1;
						while( st_pos > 0 )
						{
							POP( b );
							a += log( b );
						}
						b = a / ( (double) j );
						a = exp( b );
						PUSH( );
						break;

					case 'Q':
						a = 0;
						if( !( j = st_pos ) )
							j = 1;
						while( st_pos > 0 )
						{
							POP( b );
							a += b * b;
						}
						a = sqrt( a / (double) j );
						PUSH( );
						break;

					case 'M':
						a = 0;
						if( !( j = st_pos ) )
							j = 1;
						while( st_pos > 0 )
						{
							POP( b );
							a += b;
						}
						a /= (double) j;
						PUSH( );
						break;

					case 'D':
						stack_sd( );
						report( );
						break;

					case 'E':
						stack_median( );
						break;

					case 'X':
						stack_unique( );
						break;

					case 'U':
						NEED( 1 );
						a = b;
						while( st_pos > 0 )
						{
							POP( b );
							if( b > a )
								a = b;
						}
						PUSH( );
						break;

					case 'L':
						NEED( 1 );
						a = b;
						while( st_pos > 0 )
						{
							POP( b );
							if( b < a )
								a = b;
						}
						PUSH( );
						break;

					case 'B':
						NEED( 1 );
						a = c = b;
						while( st_pos > 0 )
						{
							POP( b );
							if( b < c )
							 	c = b;
							else if( b > a )
								a = b;
						}
						a -= c;
						PUSH( );
						break;

					default:
						BROKEN( );
				}
				break;

			case '_':
				NEED( 1 );
				if( b < 0 || b > 0 )
					a = 0;
				else
					a = 1;
				PUSH( );
				break;

			case 'b':
				p++;
				switch( *p )
				{
					case 'n':
						NEED( 1 );
						a = (double) ~((int) b);
						PUSH( );
						break;

					case 'a':
						NEED( 2 );
						j = (int) a;
						k = (int) b;
						a = (double) ( j & k );
						PUSH( );
						break;

					case 'o':
						NEED( 2 );
						j = (int) a;
						k = (int) b;
						a = (double) ( j | k );
						PUSH( );
						break;

					case 'x':
						NEED( 2 );
						j = (int) a;
						k = (int) b;
						a = (double) ( j ^ k );
						PUSH( );
						break;

					default:
						BROKEN( );
				}
				break;

			case '=':
				NEED( 2 );
				if( a == b )
					a = 1;
				else
					a = 0;
				PUSH( );
				break;

			case 'G':
				NEED( 2 );
				if( a > b )
					a = 1;
				else
					a = 0;
				PUSH( );
				break;

			case 'L':
				NEED( 2 );
				if( a < b )
					a = 1;
				else
					a = 0;
				PUSH( );
				break;

			case 'g':
				NEED( 2 );
				if( b > a )
					a = b;
				PUSH( );
				break;

			case 'l':
				NEED( 2 );
				if( b < a )
					a = b;
				PUSH( );
				break;

			case 'I':
				int_out = 1;
				break;

			case ',':
				// number separator
				break;

			case '?':
				usage( );
				exit( 0 );
				break;
		}
	}
}




int handle_stdin( void )
{
	ssize_t ret;
	char *line;
	size_t lsz;

	lsz  = 8192;
	line = (char *) malloc( lsz );

	while( ( ret = getline( &line, &lsz, stdin ) ) != -1 )
	{
		// stomp on the newline if there is one
		if( line[ret-1] == '\n' )
			line[--ret] = '\0';
		else if( ret < (ssize_t) lsz )
		{
			line[ret] = '\0';
		}
		else
		{
			// oh ffs.
			char *l = (char *) malloc( lsz + 2 );

			lsz += 2;
			memcpy( l, line, ret );
			l[ret] = '\0';
			free( line );
			line = l;
		}

		handle_arg( line );
	}

	free( line );

	if( feof( stdin ) )
		return 0;

	return 1;
}

void alarm_handler( int sig )
{
	usage( );
	exit( 1 );
}


int main( int ac, char **av )
{
	int i;

	signal( SIGALRM, &alarm_handler );

	memset( factorials, 0, MAX_FACT * sizeof( int64_t ) );

	// no args? Let's look at stdin
	if( ac == 1 )
	{
		// don't hang
		alarm( 3 );

		if( handle_stdin( ) != 0 )
		{
			fprintf( stderr, "Usage: rpcalc <formula as args>\n" );
			return 0;
		}
	}
	else
	{
		for( i = 1; i < ac; i++ )
			handle_arg( av[i] );
	}

	report( );
	return 0;
}


