
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
* main.c - program entry and argument handling                           *
*                                                                         *
* Updates:                                                                *
**************************************************************************/


#include "rpcalc.h"


void *allocz( size_t size )
{
	void *p = malloc( size );
	memset( p, 0, size );
	return p;
}

long double get_random_ld( void )
{
	return (long double) drand48( );
}

void set_random_seed( long double a )
{
	srand48( (long) a );
}

long double get_timedbl( void )
{
	struct timespec ts;
	long double t;

	clock_gettime( CLOCK_REALTIME, &ts );

	t  = ts.tv_nsec;
	t /= 1000000000.0;
	t += ts.tv_sec;

	return t;
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

	//PRSET *p;

	/*
	p = prime_factors( 81154872 );
	prime_display( p, 1 );
	prime_display( p, 0 );
	printf( "%lu -> %lu\n", 1234lu, prime_next( 1234lu ) );
	printf( "%lu <- %lu\n", 1234lu, prime_prev( 1234lu ) );
	return 0;
	*/

	// grab the nsec from the raw clock
	clock_gettime( CLOCK_MONOTONIC_RAW, &ts );
	// seed our random number generator
	srand48( (long) ts.tv_nsec );

	signal( SIGALRM, &alarm_handler );

	s = make_stack( 0 );

	// are we a terminal?
	t = isatty( fileno( stdin ) );

	// handle command-line args
	for( i = 1; i < ac; i++ )
		handle_arg( s, av[i] );

	// if you want command-line args, plus stdin, you need to pass in . as an arg
	if( ac == 1 || hasinput( s, INTYPE_TERM ) )
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

	report( s );
	return 0;
}


