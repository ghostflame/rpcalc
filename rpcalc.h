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

#define STACK_SIZE		65536
#define MAX_FACT		21
#define LIM_FACT_I		20.0L
#define LIM_FACT_E		470.0L
#define PRECISION		6

enum output_types
{
	OUTTYPE_DOUBLE = 0,
	OUTTYPE_INT,
	OUTTYPE_HEX,
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
	long long int			*	fact;		// factorial record
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
long long int getfact( STACK *s, int which );
long double peek( STACK *s, int idx );
void flatten( STACK *s );
int empty( STACK *s );
int full( STACK *s );
int current( STACK *s );
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
long double est_fact( long long int f );
long long int perms( STACK *s, long long int a, long long int b );
long double est_perms( long long int a, long long int b );
long long int comb( STACK *s, long long int a, long long int b );
long double est_comb( long long int a, long long int b );
double fibonnaci( long long int a );
int stack_compare( const void *p1, const void *p2 );
void stack_median( STACK *s );
void stack_unique( STACK *s );
void stack_lcm( STACK *s );
void stack_gcd( STACK *s );
void stack_sd( STACK *s );
