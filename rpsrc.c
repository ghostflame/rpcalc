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

enum output_types
{
	OUTTYPE_DOUBLE = 0,
	OUTTYPE_INT,
	OUTTYPE_SCI
};


#define BROKEN( )		exit( fprintf( stderr, "Invalid formula.  Try a ?\n" ) )
#define STATE( )		printf( "a = %f, b = %f\n", a, b )
#define PLIM( _x, _m )	if( _x > _m ) exit( fprintf( stderr, "Internal value limit - max is %f, value %f\n", _m, _x ) )


typedef struct stacker STACK;
struct stacker
{
	long long int				size;
	long long int				curr;
	long long int				out;
	double					*	vals;
	long long int			*	fact;		// factorial record
};

STACK *make_stack( long long int max )
{
	STACK *s;

	if( !max )
		max = STACK_SIZE;

	s = (STACK *) calloc( 1, sizeof( STACK ) );
	s->size = max;
	s->out  = OUTTYPE_DOUBLE;
	s->vals = (double *) calloc( max, sizeof( double ) );
	s->fact = (long long int *) calloc( MAX_FACT, sizeof( long long int ) );
	return s;
}


void push( STACK *s, double v )
{
	if( s->curr < s->size )
	{
		s->vals[s->curr] = v;
		++(s->curr);
	}
	else
		fprintf( stderr, "Stack max size (%lld) reached.\n", s->size );
}

double _pop( STACK *s )
{
	double v = 0;

	if( s->curr > 0 )
	{
		v = s->vals[s->curr - 1];
		s->vals[s->curr - 1] = 0;
		--(s->curr);
	}
	else
		fprintf( stderr, "Nothing on the stack to pop :-(\n" );

	return v;
}

void pop( STACK *s, double *a, double *b )
{
	if( b )
		*b = _pop( s );

	if( a )
		*a  = _pop( s );
}

double peek( STACK *s, long long int idx )
{
	if( idx >= 0 && idx < s->curr )
		return s->vals[idx];

	return 0.0;
}

void flatten( STACK *s )
{
	if( s->curr > 0 )
	{
		memset( s->vals, 0, s->curr * sizeof( double ) );
		s->curr = 0;
	}
}

long long int empty( STACK *s )
{
	return ( s->curr == 0 ) ? 1 : 0;
}

long long int full( STACK *s )
{
	return ( s->curr < s->size ) ? 0 : 1;
}

long long int current( STACK *s )
{
	return s->curr;
}


void need( STACK *s, long long int count )
{
	if( s->curr < count )
		BROKEN( );
}



long long int factorials[MAX_FACT];

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


long long int factorial( STACK *s, long long int f )
{
	if( f < 2 )
		return 1;

	if( f >= MAX_FACT )
		return -1;

	if( !s->fact[f] )
		s->fact[f] = f * factorial( s, f - 1 );

	return s->fact[f];
}

// returns log
double est_fact( long long int f )
{
	double ef = 0;

	while( f > 1 )
	{
		ef += log( f );
		--f;
	}

	return ef;
}

long long int perms( STACK *s, long long int a, long long int b )
{
	if( b > a )
		return -1;

	return factorial( s, a ) / factorial( s, a - b );
}

// returns log
double est_perms( long long int a, long long int b )
{
	if( b > a )
		return 0.0;

	return est_fact( a ) - est_fact( a - b );
}

long long int comb( STACK *s, long long int a, long long int b )
{
	if( b > a )
		return -1;

	return perms( s, a, b ) / factorial( s, b );
}

// returns log
double est_comb( long long int a, long long int b )
{
	if( b > a )
		return 0.0;

	return est_perms( a, b ) - est_fact( b );
}

int stack_compare( const void *p1, const void *p2 )
{
	double d1 = *((double *) p1);
	double d2 = *((double *) p2);

	return ( d1 > d2 ) ? 1 : ( d1 < d2 ) ? -1 : 0;
}

void stack_median( STACK *s )
{
	double *tmpstack, val;
	int c;

	if( ( c = current( s ) ) == 0 )
		return;

	tmpstack = calloc( c, sizeof( double ) );
	memcpy( tmpstack, s->vals, c * sizeof( double ) );
	qsort( tmpstack, c, sizeof( double ), &stack_compare );

	val = tmpstack[c >> 1];
	flatten( s );
	push( s, val );

	free( tmpstack );
}


void stack_unique( STACK *s )
{
	double a, *spare_stack;
	long long int i, j = 0, c;

	c = current( s );

	spare_stack = (double *) calloc( c, sizeof( double ) );

	while( c > 0 )
	{
		pop( s, &a, NULL );
		for( i = 0; i < j; ++i )
			if( spare_stack[i] == a )
				break;

		if( i == j )
		{
			spare_stack[j++] = a;
		}
		--c;
	}

	for( i = j; i > 0; --i )
		push( s, spare_stack[i - 1] );

	free( spare_stack );
}



void stack_sd( STACK *s )
{
	double m, q, a;
	long long int c, j;

	m = 0;
	c = current( s );
	j = c;

	while( j > 0 )
	{
		--j;
		m += peek( s, j );
	}

	m /= (double) c;
	// m is now the mean
	a = m;

	j = c;
	q = 0;

	while( j-- > 0 )
	{
		pop( s, &a, NULL );
		a -= m;
		q += a * a;
	}
	q /= (double) c;
	q = sqrt( q );
	// s is now the root mean square diff

	// push both the mean and stddev

	push( s, m );
	push( s, q );
}



void report( STACK *s )
{
	long long int j;
	double a;

	pop( s, &a, NULL );

	switch( s->out )
	{
		case OUTTYPE_INT:
			j = (long long int) a;
			printf( "%lld\n", j );
			break;
		case OUTTYPE_SCI:
			printf( "%g\n", a );
			break;
		default:
			printf( "%f\n", a );
			break;
	}
}


void handle_arg( STACK *s, char *arg )
{
	long long int j, k;
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
					a = strtod( p, &q );
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
						if( a == 0.0 )
							BROKEN( );
						push( s, 1.0 / a );
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
						push( s, sqrt( a ) );
						break;

					case 'c':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, cbrt( a ) );
						break;

					case 'd':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, round( a ) );
						break;

					case 't':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, trunc( a ) );
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
						push( s, log( a ) );
						break;

					case 'e':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, exp( a ) );
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
						push( s, sin( a ) );
						break;

					case 'c':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, cos( a ) );
						break;

					case 't':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, tan( a ) );
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
						push( s, asin( a ) );
						break;

					case 'c':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, acos( a ) );
						break;

					case 't':
						need( s, 1 );
						pop( s, &a, NULL );
						push( s, atan( a ) );
						break;

					default:
						BROKEN( );
				}
				break;

			case 'R':
				need( s, 1 );
				pop( s, &a, NULL );
				push( s, ( a * M_PI ) / 180.0 );
				break;

			case 'D':
				need( s, 1 );
				pop( s, &a, NULL );
				push( s, ( a * 180.0 ) / M_PI );
				break;

			case 'a':
				need( s, 1  );
				pop( s, &a, NULL );
				push( s, fabs( a ) );
				break;

			case 'c':
				p++;
				switch( *p )
				{
					case 'E':
						push( s, M_E );
						break;

					case 'P':
						push( s, M_PI );
						break;

					default:
						BROKEN( );
				}
				break;

			case '^':
				need( s, 2 );
				pop( s, &a, &b );
				push( s, pow( a, b ) );
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
						push( s, (double) j );
						break;

					case 'r':
						need( s, 2 );
						pop( s, &a, &b );
						j = (long long int) a;
						j >>= (int) b;
						push( s, (double) j );
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
				push( s, (double) ( j % k ) );
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

			case 'N':
				p++;
				switch( *p )
				{
					case 'F':
						need( s, 1 );
						pop( s, &a, NULL );
						PLIM( a, 20.0 );
						j = (long long int) a;
						a = (double) factorial( s, j );
						push( s, a );
						break;

					case 'P':
						need( s, 2 );
						pop( s, &a, &b );
						PLIM( a, 20.0 );
						PLIM( b, 20.0 );
						j = (long long int) a;
						k = (long long int) b;
						a = (double) perms( s, j, k );
						push( s, a );
						break;

					case 'C':
						need( s, 2 );
						pop( s, &a, &b );
						PLIM( a, 20.0 );
						PLIM( b, 20.0 );
						j = (long long int) a;
						k = (long long int) b;
						a = (double) comb( s, j, k );
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
						PLIM( a, 170.0 );
						j = (long long int) a;
						a = est_fact( j );
						push( s, exp( a ) );
						break;

					case 'P':
						need( s, 2 );
						pop( s, &a, &b );
						PLIM( a, 170.0 );
						PLIM( b, 170.0 );
						j = (long long int) a;
						k = (long long int) b;
						a = est_perms( j, k );
						push( s, exp( a ) );
						break;

					case 'C':
						need( s, 2 );
						pop( s, &a, &b );
						PLIM( a, 170.0 );
						PLIM( b, 170.0 );
						j = (long long int) a;
						k = (long long int) b;
						a = est_comb( j, k );
						push( s, exp( a ) );
						break;

					default:
						BROKEN( );
				}

				s->out = OUTTYPE_SCI;
				break;

			case 'S':
				p++;
				switch( *p )
				{
					case 'C':
						a = (double) current( s );
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
						b = a / ( (double) j );
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
						a = sqrt( a / (double) j );
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
						a /= (double) j;
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

			case 'b':
				p++;
				switch( *p )
				{
					case 'n':
						need( s, 1 );
						pop( s, &b, NULL );
						a = (double) ~((int) b);
						push( s, a );
						break;

					case 'a':
						need( s, 2 );
						pop( s, &a, &b );
						j = (int) a;
						k = (int) b;
						a = (double) ( j & k );
						push( s, a );
						break;

					case 'o':
						need( s, 2 );
						pop( s, &a, &b );
						j = (int) a;
						k = (int) b;
						a = (double) ( j | k );
						push( s, a );
						break;

					case 'x':
						need( s, 2 );
						pop( s, &a, &b );
						j = (int) a;
						k = (int) b;
						a = (double) ( j ^ k );
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
				s->out = OUTTYPE_INT;
				break;

			case 'X':
				s->out = OUTTYPE_SCI;

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




int handle_stdin( STACK *s )
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

		handle_arg( s, line );
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
	STACK *s;
	int i;

	signal( SIGALRM, &alarm_handler );

	s = make_stack( 0 );

	// no args? Let's look at stdin
	if( ac == 1 )
	{
		// don't hang
		alarm( 3 );

		if( handle_stdin( s ) != 0 )
		{
			fprintf( stderr, "Usage: rpcalc <formula as args>\n" );
			return 0;
		}
	}
	else
	{
		for( i = 1; i < ac; i++ )
			handle_arg( s, av[i] );
	}

	report( s );
	return 0;
}


