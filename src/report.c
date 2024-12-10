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
* report.c - outputting results                                           *
*                                                                         *
* Updates:                                                                *
**************************************************************************/

#include "rpcalc.h"

int binstr( uint64_t val, char *dest, int len )
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



void report_int( STACK *s, long double a, long long int j, uint64_t k )
{
	printf( "%lld\n", j );
}

void report_uint( STACK *s, long double a, long long int j, uint64_t k )
{
	printf( "%lu\n", k );
}

void report_hex( STACK *s, long double a, long long int j, uint64_t k )
{
	char prefbuf[4] = {0};

	if( s->flags & OUTFLAG_PREF )
	{
		prefbuf[0] = '0';
		prefbuf[1] = 'x';
	}
	printf( "%s%llx\n", prefbuf, j );
}

void report_uhex( STACK *s, long double a, long long int j, uint64_t k )
{
	char prefbuf[4] = {0};

	if( s->flags & OUTFLAG_PREF )
	{
		prefbuf[0] = '0';
		prefbuf[1] = 'x';
	}
	printf( "%s%lx\n", prefbuf, k );
}

void report_oct( STACK *s, long double a, long long int j, uint64_t k )
{
	char prefbuf[4] = {0};

	if( j == 0 )
		printf( "0\n" );
	else
	{
		if( s->flags & OUTFLAG_PREF )
			prefbuf[0] = '0';

		printf( "%s%lo\n", prefbuf, k );
	}
}

void report_bin( STACK *s, long double a, long long int j, uint64_t k )
{
	char binbuf[136] = {0};
	char prefbuf[4] = {0};
	int l;

	if( j == 0 )
		printf( "0\n" );
	else
	{
		// glibc is no help
		l = binstr( j, binbuf, 136 );

		if( s->flags & OUTFLAG_PREF )
		{
			prefbuf[0] = '0';
			prefbuf[1] = 'b';
		}
		printf( "%s%s\n", prefbuf, binbuf + l );
	}
}

void report_dbin( STACK *s, long double a, long long int j, uint64_t k )
{
	char binbuf[136] = {0};
	char prefbuf[4] = {0};
	uint64_t *uip;
	int l;

	// just grab the raw bits
	uip = (uint64_t *) &a;
	l = binstr( *uip, binbuf, 72 );
	if( s->flags & OUTFLAG_PREF )
	{
		prefbuf[0] = '0';
		prefbuf[1] = 'b';
	}
	printf( "%s%s\n", prefbuf, binbuf + l );

}

void report_sci( STACK *s, long double a, long long int j, uint64_t k )
{
	char fmtbuf[16] = {0};

	snprintf( fmtbuf, 16, "%%.%dLg\n", s->prec );
	printf( fmtbuf, a );
}

void report_default( STACK *s, long double a, long long int j, uint64_t k )
{
	char fmtbuf[16] = {0};

	snprintf( fmtbuf, 16, "%%.%dLf\n", s->prec );
	printf( fmtbuf, a );
}

void report( STACK *s )
{
	long long int j;
	long double a;
	uint64_t k;

	pop( s, &a, NULL );
	j = (long long int) a;
	k = (uint64_t) a;

	// and call the reporter function
	(s->rptr)( s, a, j, k );
}

