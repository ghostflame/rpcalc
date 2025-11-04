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
* prime.c - functions relating to prime numbers                           *
*                                                                         *
* Updates:                                                                *
**************************************************************************/

#include "rpcalc.h"

#define _pri_check( _v, _ret )		if( _v >= PRI_CALC_MAX ) return _ret

#define pri_check( _v )				_pri_check( _v, -1 )
#define prn_check( _v )				_pri_check( _v, NULL )
#define pru_check( _v )				_pri_check( _v, 0 )


void pri_add_div( PRSET *s, uint64_t v )
{
	PRDIV *d;

	if( s->divs && s->divs->pri == v )
		s->divs->pwr++;
	else
	{
		d = (PRDIV *) allocz( sizeof( PRDIV ) );
		d->pri = v;
		d->pwr = 1;

		d->next = s->divs;
		s->divs = d;

		s->dct++;
	}
}

void pri_list_rev( PRSET *p )
{
	PRDIV *d, *l;

	if( !p->dct )
		return;

	l = NULL;
	while( p->divs )
	{
		d = p->divs;
		p->divs = d->next;

		d->next = l;
		l = d;
	}

	p->divs = l;

	return;
}

void pri_list_free( PRSET *p )
{
	PRDIV *d;

	if( !p )
		return;

	while( p->divs )
	{
		d = p->divs;
		p->divs = d->next;

		free( d );
	}

	free( p );
}

// return the number of primes in common
int pri_list_common( PRSET *pa, PRSET *pb )
{
	PRDIV *da, *db;
	int k = 0;

	da = pa->divs;
	db = pb->divs;

	while( da && db )
	{
		// unequal?  keep running
		if( da->pri < db->pri )
		{
			da = da->next;
			continue;
		}
		if( db->pri < da->pri )
		{
			db = db->next;
			continue;
		}

		// equal!
		++k;
		da = da->next;
		db = db->next;
	}

	return k;
}

int prime_check( uint64_t val )
{
	uint64_t p, sr;
	double d, e;
	int i;

	pri_check( val );

	d = (double) val;
	e = sqrt( d );
	sr = (uint64_t) e;

	for( i = 0; i < SM_PRI_COUNT; ++i )
	{
	 	p = smaller_primes[i];
		if( p > sr )
			break;
		if( !( val % p ) )
			return 0;
	}

	return 1;
}

PRSET *prime_factors( uint64_t val )
{
	uint64_t p, sr;
	double d, e;
	PRSET *ps;
	int i;

	prn_check( val );

	d = (double) val;
	e = sqrt( d );
	sr = (uint64_t) e;

	ps = (PRSET *) allocz( sizeof( PRSET ) );
	ps->num = val;

	for( i = 0; i < SM_PRI_COUNT; ++i )
	{
		p = smaller_primes[i];
		if( p > sr )
			break;

		while( !( val % p ) )
		{
			pri_add_div( ps, p );
			val /= p;
		}
	}

	// were we prime in the end?
	if( ps->dct == 0 )
	{
		ps->is_prime = 1;
	}
	else
	{
		// get the list in order
		pri_list_rev( ps );
	}

	return ps;
}


int prime_relatively( uint64_t va, uint64_t vb )
{
	PRSET *pa, *pb;

	pri_check( va );
	pri_check( vb );

	pa = prime_factors( va );
	pb = prime_factors( vb );

	if( !pa || !pb )
	{
		pri_list_free( pa );
		pri_list_free( pb );
		return -1;
	}

	return !pri_list_common( pa, pb );
}

void prime_display( PRSET *p, int compact )
{
	PRDIV *d;
	int i;

	printf( "Prime factors of %lu\n   ", p->num );
	if( compact )
	{
		for( d = p->divs; d; d = d->next )
			if( d->pwr > 1 )
				printf( " %lu^%d", d->pri, d->pwr );
			else
				printf( " %lu", d->pri );
	}
	else
	{
		for( d = p->divs; d; d = d->next )
			for( i = 0; i < d->pwr; i++ )
				printf( " %lu", d->pri );
	}
	printf( "\n" );
}

uint64_t prime_next( uint64_t val )
{
	uint64_t pv;

	pru_check( val );

	for( pv = val + 1; pv < PRI_CALC_MAX; ++pv )
		if( prime_check( pv ) )
			return pv;

	return 0;
}

uint64_t prime_prev( uint64_t val )
{
	uint64_t pv;

	pru_check( val );

	if( val < 2 )
		return val;

	for( pv = val - 1; pv > 1; --pv )
		if( prime_check( pv ) )
			return pv;

	return 0;
}
