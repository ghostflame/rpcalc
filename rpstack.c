#include "rpcalc.h"


STACK *make_stack( int max )
{
	STACK *s;

	if( !max )
		max = STACK_SIZE;

	s = (STACK *) calloc( 1, sizeof( STACK ) );
	s->size  = max;
	s->out   = OUTTYPE_DOUBLE;
	s->flags = INTYPE_HEX|INTYPE_OCT|INTYPE_BIN|OUTFLAG_PREF;
	s->prec  = PRECISION;
	s->vals  = (double *) calloc( max, sizeof( double ) );
	s->fact  = (long long int *) calloc( 1 + MAX_FACT, sizeof( long long int ) );

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
		fprintf( stderr, "Stack max size (%d) reached.\n", s->size );
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

long long int getfact( STACK *s, int which )
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

void pop( STACK *s, double *a, double *b )
{
	if( b )
		*b = _pop( s );

	if( a )
		*a  = _pop( s );
}

double peek( STACK *s, int idx )
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

void need( STACK *s, int count )
{
	if( s->curr < count )
		BROKEN( );
}

void setoutput( STACK *s, int out )
{
	if( out >= OUTTYPE_DOUBLE && out < OUTTYPE_MAX )
		s->out = out;
}

int _binstr( long long int val, char *dest, int len )
{
	int i, l;

	if( len < 65 )
		BROKEN( );

	memset( dest, 0, len );

	for( l = 0, i = 0; i < 64; ++i )
	{
		if( ( val >> i ) & 0x1 )
		{
			l = 63 - i; // record last bit, for length
			dest[63 - i] = '1';
		}
		else
			dest[63 - i] = '0';
	}

	return l;
}

void setinput( STACK *s, int flags, int apply )
{
	char binbuf[72];
	int l;

	l = _binstr( (long long int) flags, binbuf, 72 );
	printf( "Args:  0b%s  %d\n", binbuf + l, apply );

	l = _binstr( (long long int) s->flags, binbuf, 72 );
	printf( "Flags was: 0b%s\n", binbuf + l );

	if( apply )
		s->flags |= flags & INTYPE_MASK;
	else
		s->flags &= ~flags & INTYPE_MASK;

	l = _binstr( (long long int) s->flags, binbuf, 72 );
	printf( "Flags now: 0b%s\n", binbuf + l );
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
	if( num >= 0 && num <= 12 )
		s->prec = num;
}


void report( STACK *s )
{
	char prefbuf[4], fmtbuf[16], binbuf[72];
	long long int j, *lp;
	double a;
	int l;

	memset( prefbuf, 0, 4 );

	pop( s, &a, NULL );
	j = (long long int) a;

	switch( s->out )
	{
		case OUTTYPE_INT:
			printf( "%lld\n", j );
			break;

		case OUTTYPE_HEX:
			if( s->flags & OUTFLAG_PREF )
			{
				prefbuf[0] = '0';
				prefbuf[1] = 'x';
			}
			printf( "%s%llx\n", prefbuf, j );
			break;

		case OUTTYPE_OCT:
			if( j == 0 )
				printf( "0\n" );
			else
			{
				if( s->flags & OUTFLAG_PREF )
					prefbuf[0] = '0';

				printf( "%s%llo\n", prefbuf, j );
			}
			break;

		case OUTTYPE_BIN:
			if( j == 0 )
				printf( "0\n" );
			else
			{
				// glibc is no help
				l = _binstr( j, binbuf, 72 );

				if( s->flags & OUTFLAG_PREF )
				{
					prefbuf[0] = '0';
					prefbuf[1] = 'b';
				}
				printf( "%s%s\n", prefbuf, binbuf + l );
			}
			break;

		case OUTTYPE_SCI:
			snprintf( fmtbuf, 16, "%%.%dg\n", s->prec );
			printf( fmtbuf, a );
			break;

		case OUTTYPE_DBIN:
			// just grab the raw bits
			lp = (long long int *) &a;
			l = _binstr( *lp, binbuf, 72 );
			if( s->flags & OUTFLAG_PREF )
			{
				prefbuf[0] = '0';
				prefbuf[1] = 'b';
			}
			printf( "%s%s\n", prefbuf, binbuf + l );
			break;

		default:
			snprintf( fmtbuf, 16, "%%.%df\n", s->prec );
			printf( fmtbuf, a );
			break;
	}
}


