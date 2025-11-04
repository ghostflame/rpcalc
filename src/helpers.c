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
* helpers.c - sub-op parsing, lots of switches                            *
*                                                                         *
* Updates:                                                                *
**************************************************************************/

#include "rpcalc.h"

// keep these in alphabetical order, it makes avoiding
// duplicates easier
const rpcst_opt const_opts[] =
{
	// maths - upper case
	{ 'D', 0.739085133215160641655L },	// Dottie number
	{ 'E', M_El },				// e (in maths.h)
	{ 'P', M_PIl },				// PI (in maths.h)
	{ 'G', 1.61803398874989484820L },	// Phi, golden ratio
	{ 'L', 2.62205755429211981046L },	// Leminscate constant
	{ 'M', 0.955316618124509278163L },	// Magic angle
	{ 'Q', 2.29558714939263807403L },	// Parabolic constant

	// physics - lower case
	{ 'b', 1.380649e-23L },			// Boltzmann constant
	{ 'c', 299792458L },			// speed of light
	{ 'e', 1.602176634e-19L },		// charge on the electron
	{ 'f', 0.0072973525643L },		// fine structure constant
	{ 'g', 6.674301e-11L },			// Gravitation constant
	{ 'h', 6.62607015e-34L },		// Planck's constant
	{ 'm', 6.02214076e23L },		// mole, Avagadro's number
	{ 's', 5.670374419e-8L },		// Stefan-Boltzmann constant

	// terminator - always last
	{ '\0', 0.0 },
};


void helper_const( STACK *s, char cst )
{
	const rpcst_opt *rpo;

	for( rpo = const_opts; rpo->opt; rpo++ )
		if( rpo->opt == cst )
		{
			push( s, rpo->val );
			return;
		}

	BROKEN( );
}


void helper_fetch( STACK *s, char which )
{
	long double a;
	int st_off;

	switch( which )
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
			st_off = which - '0';
			if( get_mem( s, st_off, &a ) != 0 )
			{
				BROKEN( );
			}
			push( s, a );
			break;

		default:
			BROKEN( );
	}
}


void helper_store( STACK *s, char which )
{
	long double a;
	int st_off;

	switch( which )
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
			st_off = which - '0';
			need( s, 1 );
			pop( s, &a, NULL );
			set_mem( s, st_off, a );
			break;

		default:
			BROKEN( );
	}
}


// bitwise, convert to uint, operate, convert back
void helper_bitwise( STACK *s, char op )
{
	long double a, b;
	uint64_t m, n;
	int64_t j;

	switch( op )
	{
		case 'n':
			need( s, 1 );
			pop( s, &b, NULL );
			m = (uint64_t) b;
			n = ~m;
			a = (long double) n;
			push( s, a );
			break;

		case 'N':
			need( s, 1 );
			pop( s, &b, NULL );
			m = (uint64_t) b;
			n = ~m;
			a = (long double) ( n & 0xffffffff );
			push( s, a );
			break;

		case 'a':
			need( s, 2 );
			pop( s, &a, &b );
			m = (uint64_t) a;
			n = (uint64_t) b;
			a = (long double) ( m & n );
			push( s, a );
			break;

		case 'o':
			need( s, 2 );
			pop( s, &a, &b );
			m = (uint64_t) a;
			n = (uint64_t) b;
			a = (long double) ( m | n );
			push( s, a );
			break;

		case 'x':
			need( s, 2 );
			pop( s, &a, &b );
			m = (uint64_t) a;
			n = (uint64_t) b;
			a = (long double) ( m ^ n );
			push( s, a );
			break;

		case 'm':
			need( s, 2 );
			pop( s, &a, &b );
			m = (uint64_t) a;
			j = (long long int) b;
			n = 0;
			if( j > 63 )
			{
				fprintf( stderr, "Cannot mask lower %ld bits, limit is 63.\n", j );
				BROKEN( );
			}
			//for( ; j > 0; --j )
			//	n = ( n << 1 ) + 1;
			n = ( 1 << j ) - 1;
			a = (long double) ( m & n );
			push( s, a );
			break;

		case 'u':
			need( s, 2 );
			pop( s, &a, &b );
			m = (uint64_t) a;
			j = (long long int) b;
			if( j > 63 )
			{
				fprintf( stderr, "Cannot upshift %ld bits, limit is 63.\n", j );
				BROKEN( );
			}
			a = (long double) ( m << j );
			push( s, a );
			break;

		case 'd':
			need( s, 2 );
			pop( s, &a, &b );
			m = (uint64_t) a;
			j = (long long int) b;
			if( j > 63 )
			{
				fprintf( stderr, "Cannot downshift %ld bits, limit is 63.\n", j );
				BROKEN( );
			}
			a = (long double) ( m >> j );
			push( s, a );
			break;

		default:
			BROKEN( );
	}
}

void helper_bitshift( STACK *s, char op )
{
	long double a, b;
	int64_t j;

	switch( op )
	{
		case 'l':
			need( s, 2 );
			pop( s, &a, &b );
			j = (int64_t) a;
			j <<= (int) b;
			push( s, (long double) j );
			break;

		case 'r':
			need( s, 2 );
			pop( s, &a, &b );
			j = (int64_t) a;
			j >>= (int) b;
			push( s, (long double) j );
			break;

		default:
			BROKEN( );
	}
}


// whole-stack operations
void helper_stack_ops( STACK *s, char op )
{
	long double a, b, c;
	int64_t j;

	switch( op )
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

		case 'O':
			stack_mode( s );
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

		// just remove something from the stack
		case '-':
			pop( s, &b, NULL );
			break;

		default:
			BROKEN( );
	}
}


void helper_trig( STACK *s, char op )
{
	long double a;

	switch( op )
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

		case 'S':
			need( s, 1 );
			pop( s, &a, NULL );
			push( s, asinl( a ) );
			break;

		case 'C':
			need( s, 1 );
			pop( s, &a, NULL );
			push( s, acosl( a ) );
			break;

		case 'T':
			need( s, 1 );
			pop( s, &a, NULL );
			push( s, atanl( a ) );
			break;

		default:
			BROKEN( );
	}
}


void helper_logs( STACK *s, char op )
{
	long double a, b;

	switch( op )
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

		case 'L':
			need( s, 1 );
			pop( s, &a, NULL );
			push( s, log10l( a ) );
			break;

		case 'E':
			need( s, 1 );
			pop( s, &a, NULL );
			push( s, exp10l( a ) );
			break;

		case 'Y':
			need( s, 2 );
			pop( s, &a, &b );
			if( b > 1 )
				push( s, logl( a ) / logl( b ) );
			else
				BROKEN( );
			break;

		default:
			BROKEN( );
	}
}


void helper_fact( STACK *s, char op )
{
	long double a, b;
	int64_t j, k;

	switch( op )
	{
		case 'F':
			need( s, 1 );
			pop( s, &a, NULL );
			PLIM( a, LIM_FACT_I );
			j = (int64_t) a;
			a = (long double) getfact( s, (int) j );
			push( s, a );
			break;

		case 'P':
			need( s, 2 );
			pop( s, &a, &b );
			PLIM( a, LIM_FACT_I );
			PLIM( b, LIM_FACT_I );
			j = (int64_t) a;
			k = (int64_t) b;
			a = (long double) perms( s, j, k );
			push( s, a );
			break;

		case 'C':
			need( s, 2 );
			pop( s, &a, &b );
			PLIM( a, LIM_FACT_I );
			PLIM( b, LIM_FACT_I );
			j = (int64_t) a;
			k = (int64_t) b;
			a = (long double) comb( s, j, k );
			push( s, a );
			break;

		default:
			BROKEN( );
	}
}

void helper_fact_est( STACK *s, char op )
{
	long double a, b;
	int64_t j, k;

	switch( op )
	{
		case 'F':
			need( s, 1 );
			pop( s, &a, NULL );
			PLIM( a, LIM_FACT_E );
			j = (int64_t) a;
			a = est_fact( j );
			push( s, expl( a ) );
			break;

		case 'P':
			need( s, 2 );
			pop( s, &a, &b );
			PLIM( a, LIM_FACT_E );
			PLIM( b, LIM_FACT_E );
			j = (int64_t) a;
			k = (int64_t) b;
			a = est_perms( j, k );
			push( s, expl( a ) );
			break;

		case 'C':
			need( s, 2 );
			pop( s, &a, &b );
			PLIM( a, LIM_FACT_E );
			PLIM( b, LIM_FACT_E );
			j = (int64_t) a;
			k = (int64_t) b;
			a = est_comb( j, k );
			push( s, expl( a ) );
			break;

		default:
			BROKEN( );
	}
}


void helper_fncall( STACK *s, char op )
{
	long double a;

	switch( op )
	{
		// random long double 0 <= ld < 1
		case 'R':
			a = get_random_ld( );
			push( s, a );
			break;

		case 'S':
			need( s, 1 );
			pop( s, &a, NULL );
			set_random_seed( a );
			break;

		case 't':
			a = get_timedbl( );
			push( s, a );
			break;

		case 'T':
			a = (long double) time( NULL );
			push( s, a );
			break;

		default:
			BROKEN();
	}
}

void helper_root_around( STACK *s, char op )
{
	long double a, b;

	switch( op )
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
}


char *helper_input_type( STACK *s, char *p )
{
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

	return p;
}


void helper_output_type( STACK *s, char op )
{
	switch( op )
	{
		case 'I':
			setoutput( s, OUTTYPE_INT );
			break;

		case 'U':
			setoutput( s, OUTTYPE_UINT );
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
}

void helper_prime( STACK *s, char op )
{
	long double a, b;
	uint64_t u, v;
	int i;

	switch( op )
	{
		case 'T':
			need( s, 1 );
			pop( s, &a, NULL );
			v = (uint64_t) a;
			i = prime_check( v );
			push( s, (long double) i );
			break;

		case 'R':
			need( s, 2 );
			pop( s, &a, &b );
			u = (uint64_t) a;
			v = (uint64_t) b;
			i = prime_relatively( u, v );
			push( s, (long double) i );
			break;

		case 'N':
			need( s, 1 );
			pop( s, &a, NULL );
			v = (uint64_t) a;
			u = prime_next( v );
			push( s, (long double) u );
			break;

		case 'n':
			need( s, 1 );
			pop( s, &a, NULL );
			v = (uint64_t) a;
			u = prime_prev( v );
			push( s, (long double) u );
			break;

	}
}
