
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
* rpsrc.c - program entry and argument handling                           *
*                                                                         *
* Updates:                                                                *
**************************************************************************/


#include "rpcalc.h"



long double get_random_ld( void )
{
	return (long double) drand48( );
}


// returns log
long double est_fact( uint64_t f )
{
	long double ef = 0;

	while( f > 1 )
	{
		ef += log( f );
		--f;
	}

	return ef;
}

uint64_t perms( STACK *s, uint64_t a, uint64_t b )
{
	if( b > a )
		return -1;

	return getfact( s, (int) a ) / getfact( s, (int) (a - b) );
}

// returns log
long double est_perms( uint64_t a, uint64_t b )
{
	if( b > a )
		return 0.0;

	return est_fact( a ) - est_fact( a - b );
}

uint64_t comb( STACK *s, uint64_t a, uint64_t b )
{
	if( b > a )
		return -1;

	return perms( s, a, b ) / getfact( s, (int) b );
}

// returns log
long double est_comb( uint64_t a, uint64_t b )
{
	if( b > a )
		return 0.0;

	return est_perms( a, b ) - est_fact( b );
}

long double fibonnaci( uint64_t a )
{
	long double b, c, d;

	if( a < 1 )
		return 0;

	// values needed for fib calc
	b = (long double) a;
	c = ( 1.0 + SQRT_FIVE ) / 2.0;
	d = 1 - c;

	return ( pow( c, b ) - pow( d, b ) ) / SQRT_FIVE;
}

long double nth_root( long double a, long double b )
{
	uint64_t c = (uint64_t) b;
	long double d, e;

	if( c < 1 )
		return 0;

	// now that we've converted that to int, and it's safe
	d = (long double) c;
	e = log( a );

	return exp( e / d );
}

int stack_compare( const void *p1, const void *p2 )
{
	long double d1 = *((long double *) p1);
	long double d2 = *((long double *) p2);

	return ( d1 > d2 ) ? 1 : ( d1 < d2 ) ? -1 : 0;
}

void stack_median( STACK *s )
{
	long double *tmpstack, val;
	int c;

	if( ( c = current( s ) ) == 0 )
		return;

	tmpstack = calloc( c, sizeof( long double ) );
	memcpy( tmpstack, s->vals, c * sizeof( long double ) );
	qsort( tmpstack, c, sizeof( long double ), &stack_compare );

	val = tmpstack[c >> 1];
	flatten( s );
	push( s, val );

	free( tmpstack );
}

void stack_mode( STACK *s )
{
	long double a, *spare_stack;
	uint64_t i, j = 0, c;
	int *counters, max;

	c = current( s );

	spare_stack = (long double *) calloc( c, sizeof( long double ) );
	counters = (int *) calloc( c, sizeof( int ) );

	while( c > 0 )
	{
		pop( s, &a, NULL );
		for( i = 0; i < j; ++i )
			if( spare_stack[i] == a )
			{
				counters[i]++;
				break;	
			}

		if( i == j )
		{
			spare_stack[j++] = a;
			counters[j]++;
		}
		--c;
	}

	max = 0;
	a = 0;
	for( i = 0; i < j; ++i )
		if( max < counters[i] )
		{
			max = counters[i];
			a = spare_stack[i];
		}

	free( spare_stack );
	free( counters );
	push( s, a );
}


void stack_unique( STACK *s )
{
	long double a, *spare_stack;
	uint64_t i, j = 0, c;

	c = current( s );

	spare_stack = (long double *) calloc( c, sizeof( long double ) );

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

uint64_t __gcd_of_two( uint64_t a, uint64_t b )
{
	uint64_t t;

	// if they are the same, return either
	if( a == b )
		return a;
	else if( a < b )	// otherwise arrange a > b
	{
		t = a;
		a = b;
		b = t;
	}

	// Euclidean algorithm - iterate modulo
	// always a is higher at the start
	while( b > 0 )
	{
		a = a % b;
		t = a;
		a = b;
		b = t;
	}

	return a;
}

// walk along the stack making GCD and stack product
void stack_lcm( STACK *s )
{
	uint64_t v, res;
	long double a;
	int i, c;

	// unique the stack
	stack_unique( s );

	// no stack?  nothing to do
	c = current( s );

	// trivial cases - stack already contains the answer, or nothing
	if( c < 2 )
		return;

	pop( s, &a, NULL );
	res = (uint64_t) fabsl( a );
	c--;

	// iterate along doing pairs
	for( i = 0; i < c; ++i )
	{
		pop( s, &a, NULL );
		v = (uint64_t) fabsl( a );

		// and use the gcd method
		// if we ever see 0, it's 0
		if( res > 0 && v > 0 )
			res = res * ( v / __gcd_of_two( res, v ) );
		else
			res = 0;
	}

	a = (long double) res;
	push( s, a );
}


// walk along the stack making GCD
void stack_gcd( STACK *s )
{
	uint64_t v, res;
	long double a;
	int c, i;

	// unique the stack
	stack_unique( s );

	c = current( s );

	// does the stack already contain the answer?
	if( c < 2 )
		return;

	pop( s, &a, NULL );
	res = (uint64_t) a;
	--c;

	for( i = 0; i < c; ++i )
	{
		pop( s, &a, NULL );
		v = (uint64_t) a;

		// and compare these two
		// if we ever see 0, it's 0
		if( res > 0 && v > 0 )
			res = __gcd_of_two( res, v );
		else
			res = 0;
	}

	a = (long double) res;
	push( s, a );
}


void stack_sd( STACK *s )
{
	long double m, q, a;
	int c, j;

	m = 0;
	c = current( s );
	j = c;

	while( j > 0 )
	{
		--j;
		m += peek( s, j );
	}

	m /= (long double) c;
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
	q /= (long double) c;
	q = sqrt( q );
	// s is now the root mean square diff

	// push both the mean and stddev

	push( s, m );
	push( s, q );
}





int handle_stdin( STACK *s )
{
	ssize_t ret;
	char *line;
	size_t lsz;

	lsz  = 262144;
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
	struct timespec ts;
	STACK *s;
	int i, t;

	// grab the nsec from the raw clock
	clock_gettime( CLOCK_MONOTONIC_RAW, &ts );
	// seed our random number generator
	srand48( (long) ts.tv_nsec );

	signal( SIGALRM, &alarm_handler );

	s = make_stack( 0 );

	// are we a terminal?
	t = isatty( fileno( stdin ) );

	// no args? Let's look at stdin
	if( ac == 1 )
	{
		// don't hang
		if( !t )
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

		// allow stdin even with args from a pipe
		// permits echo x y z | rpcalc oX
		if( t && hasinput( s, INTYPE_TERM ) )
			handle_stdin( s );
	}

	report( s );
	return 0;
}


