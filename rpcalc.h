
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
* rpcalc.c - includes, defines, function defs                             *
*                                                                         *
* Updates:                                                                *
**************************************************************************/

#define _GNU_SOURCE

#include <math.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RPCALC_VERSION	"0.1.0"

#define STACK_SIZE		262144
#define MAX_FACT		21
#define LIM_FACT_I		20.0L
#define LIM_FACT_E		470.0L
#define PRECISION		6

enum output_types
{
	OUTTYPE_DOUBLE = 0,
	OUTTYPE_INT,
	OUTTYPE_UINT,
	OUTTYPE_HEX,
	OUTTYPE_UHEX,
	OUTTYPE_OCT,
	OUTTYPE_BIN,
	OUTTYPE_SCI,
	OUTTYPE_DBIN,
	OUTTYPE_MAX
};


#define INTYPE_OCT		0x0001
#define INTYPE_HEX		0x0002
#define INTYPE_BIN		0x0004
#define INTYPE_TERM		0x0010
#define INTYPE_MASK		0x00ff

#define OUTFLAG_PREF	0x0100		// leading 0x, 0, 0b

// some constants
#define RPCST_PHI		1.61803398874989484820L
#define RPCST_AVAGADRO	6.02214076e23L
#define RPCST_PLANCK	6.62607015e-34L
#define RPCST_LIGHT		299792458L
#define RPCST_GRAV		6.674301e-11L
#define RPCST_CHARGE	1.602176634e-19L

#define SQRT_FIVE		2.23606797749978969640


#define BROKEN( )		exit( fprintf( stderr, "Invalid formula.  Try a ?\n" ) )
#define STATE( )		printf( "a = %lf, b = %lf\n", a, b )
#define PLIM( _x, _m )	if( _x > _m ) exit( fprintf( stderr, "Internal value limit - max is %Lf, value %Lf\n", _m, _x ) )

// stack structure
typedef struct stacker STACK;
struct stacker
{
	long double				*	vals;
	uint64_t				*	fact;		// factorial record
	long double					mem[10];	// memory section
	int							membits;	// only lowest 10 bits matter
	int							size;
	int							curr;
	int							out;		// single type
	int							flags;
	int							prec;
};

// stack interface
STACK *make_stack( int max );
void push( STACK *s, long double v );
void pop( STACK *s, long double *a, long double *b );
uint64_t getfact( STACK *s, int which );
long double peek( STACK *s, int idx );
void flatten( STACK *s );
void dumpstack( STACK *s );
int empty( STACK *s );
int full( STACK *s );
int current( STACK *s );
void set_mem( STACK *s, int offset, long double a );
int get_mem( STACK *s, int offset, long double *a );
void need( STACK *s, int count );
void setoutput( STACK *s, int type );
void setinput( STACK *s, int flags, int apply );
void setbare( STACK *s, int apply );
void setprecision( STACK *s, int num );
int hasinput( STACK *s, int type );
void report( STACK *s );

// args file
void usage( void );
void handle_arg( STACK *s, char *arg );

// handler functions
long double get_random_ld( void );
long double est_fact( uint64_t f );
uint64_t perms( STACK *s, uint64_t a, uint64_t b );
long double est_perms( uint64_t a, uint64_t b );
uint64_t comb( STACK *s, uint64_t a, uint64_t b );
long double est_comb( uint64_t a, uint64_t b );
long double nth_root( long double a, long double b );
long double fibonnaci( uint64_t a );
int stack_compare( const void *p1, const void *p2 );
void stack_median( STACK *s );
void stack_mode( STACK *s );
void stack_unique( STACK *s );
void stack_lcm( STACK *s );
void stack_gcd( STACK *s );
void stack_sd( STACK *s );

