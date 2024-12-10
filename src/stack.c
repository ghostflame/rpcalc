
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
* stack.c - stack interface                                               *
*                                                                         *
* Updates:                                                                *
**************************************************************************/

#include "rpcalc.h"

// how we output
report_fn *reporter_functions[OUTTYPE_MAX] =
{
	&report_default,
	&report_int,
	&report_uint,
	&report_hex,
	&report_uhex,
	&report_oct,
	&report_bin,
	&report_sci,
	&report_dbin,
};


STACK *make_stack( int max )
{
	STACK *s;

	if( !max )
		max = STACK_SIZE;

	s = (STACK *) calloc( 1, sizeof( STACK ) );
	s->size  = max;
	s->out   = OUTTYPE_DOUBLE;
	s->rptr  = &report_default;
	s->flags = INTYPE_HEX|INTYPE_OCT|INTYPE_BIN|OUTFLAG_PREF;
	s->prec  = PRECISION;
	s->vals  = (long double *) calloc( max, sizeof( long double ) );
	s->fact  = (uint64_t *) calloc( 1 + MAX_FACT, sizeof( uint64_t ) );

	return s;
}


void push( STACK *s, long double v )
{
	if( s->curr < s->size )
	{
		s->vals[s->curr] = v;
		++(s->curr);
	}
	else
		fprintf( stderr, "Stack max size (%d) reached.\n", s->size );
}

long double _pop( STACK *s )
{
	long double v = 0;

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

uint64_t getfact( STACK *s, int which )
{
	int i;

	if( which >= 0 && which < MAX_FACT )
	{
		// precalculate the factorials, there's not many
		if( s->fact[0] == 0 )
		{
			s->fact[0] = 1;
			for( i = 1; i < MAX_FACT; i++ )
				s->fact[i] = s->fact[i-1] * i;
		}

		return s->fact[which];
	}

	return -1;
}

void pop( STACK *s, long double *a, long double *b )
{
	if( b )
		*b = _pop( s );

	if( a )
		*a  = _pop( s );
}

long double peek( STACK *s, int idx )
{
	if( idx >= 0 && idx < s->curr )
		return s->vals[idx];

	return 0.0;
}

void flatten( STACK *s )
{
	if( s->curr > 0 )
	{
		memset( s->vals, 0, s->curr * sizeof( long double ) );
		s->curr = 0;
	}
}

int empty( STACK *s )
{
	return ( s->curr == 0 ) ? 1 : 0;
}

int full( STACK *s )
{
	return ( s->curr < s->size ) ? 0 : 1;
}

int current( STACK *s )
{
	return s->curr;
}

void set_mem( STACK *s, int offset, long double a )
{
	int bits;

	if( offset >= 0 && offset < 10 )
	{
		bits = 0x1 << offset;
		s->mem[offset] = a;
		s->membits |= bits;
	}
}

int get_mem( STACK *s, int offset, long double *a )
{
	int bits;

	if( offset >= 0 && offset < 10 )
	{
		bits = 0x1 << offset;
		if( s->membits & bits )
		{
			*a = s->mem[offset];
			return 0;
		}
	}

	return 1;
}


void need( STACK *s, int count )
{
	if( s->curr < count )
		BROKEN( );
}

void setoutput( STACK *s, int out )
{
	if( out >= OUTTYPE_DOUBLE && out < OUTTYPE_MAX )
	{
		s->out = out;
		s->rptr = reporter_functions[out];
	}
}


void setinput( STACK *s, int flags, int apply )
{
	if( apply )
		s->flags |= flags & INTYPE_MASK;
	else
		s->flags &= ~flags & INTYPE_MASK;
}

int hasinput( STACK *s, int type )
{
	return ( s->flags & type );
}

void setbare( STACK *s, int apply )
{
	if( apply )
		s->flags &= ~OUTFLAG_PREF;
	else
		s->flags |= OUTFLAG_PREF;
}

void setprecision( STACK *s, int num )
{
	if( num >= 0 && num <= 24 )
		s->prec = num;
}

void dumpstack( STACK *s )
{
	int i;

	for( i = 0; i < s->curr; i++ )
		printf( " %06d   %Lf\n", i, s->vals[i] );
}



