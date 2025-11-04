
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
* rpcalc.h - includes, defines, function defs                             *
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

#define RPCALC_VERSION	"0.3.0"

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

#define SQRT_FIVE		2.23606797749978969640

#define PRI_CALC_MAX	999966000289
#define SM_PRI_COUNT	78498


#define BROKEN( )		exit( fprintf( stderr, "Invalid formula.  Try a ?\n" ) )
#define STATE( )		printf( "a = %lf, b = %lf\n", a, b )
#define PLIM( _x, _m )	if( _x > _m ) exit( fprintf( stderr, "Internal value limit - max is %Lf, value %Lf\n", _m, _x ) )

// typedefs
typedef struct stacker STACK;
typedef struct constant_option rpcst_opt;
typedef struct prime_divisor PRDIV;
typedef struct prime_set PRSET;
typedef struct help_group HEGR;

// functions
typedef void report_fn ( STACK *s, long double, long long int, uint64_t );


// stack structure
struct stacker
{
	long double				*	vals;
	uint64_t				*	fact;		// factorial record
	report_fn				*	rptr;
	long double					mem[10];	// memory section
	int							membits;	// only lowest 10 bits matter
	int							size;
	int							curr;
	int							out;		// single type
	int							flags;
	int							prec;
};

// constants options
struct constant_option
{
	char		opt;
	long double	val;
};

struct prime_divisor
{
	PRDIV					*	next;
	uint64_t					pri;
	int32_t						pwr;
};

// prime number factorisation
struct prime_set
{
	uint64_t					num;
	PRDIV					*	divs;
	int32_t						dct;
	int32_t						is_prime;
};

struct help_group
{
	char						key;
	char					*	texts[20];
};


extern uint64_t smaller_primes[SM_PRI_COUNT];
extern HEGR help_groups[];

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
int binstr( uint64_t val, char *dest, int len );
void report( STACK *s );

// args file
void handle_arg( STACK *s, char *arg );

// help file
void usage( char grp, int exval );

// handler functions
void *allocz( size_t size );
long double get_random_ld( void );
void set_random_seed( long double a );
long double get_timedbl( void );

// calc functions
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

// parsing helper functions
void helper_const( STACK *s, char cst );
void helper_fetch( STACK *s, char which );
void helper_store( STACK *s, char which );
void helper_bitwise( STACK *s, char op );
void helper_bitshift( STACK *s, char op );
void helper_stack_ops( STACK *s, char op );
void helper_prime( STACK *s, char op );
void helper_trig( STACK *s, char op );
void helper_logs( STACK *s, char op );
void helper_fact( STACK *s, char op );
void helper_fact_est( STACK *s, char op );
void helper_fncall( STACK *s, char op );
void helper_root_around( STACK *s, char op );
char *helper_input_type( STACK *s, char *p );
void helper_output_type( STACK *s, char op );

// prime functions
int prime_check( uint64_t val );
PRSET *prime_factors( uint64_t val );
void prime_display( PRSET *p, int compact );
int prime_relatively( uint64_t va, uint64_t vb );
uint64_t prime_next( uint64_t val );
uint64_t prime_prev( uint64_t val );


// in order, as it happens
report_fn report_default;
report_fn report_int;
report_fn report_uint;
report_fn report_hex;
report_fn report_uhex;
report_fn report_oct;
report_fn report_bin;
report_fn report_sci;
report_fn report_dbin;

