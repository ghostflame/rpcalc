#define _GNU_SOURCE

#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STACK_SIZE		65536
#define MAX_FACT		21
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
#define INTYPE_MASK		0x00ff

#define OUTFLAG_PREF	0x0100		// leading 0x, 0, 0b

// some constants
#define RPCST_AVAGADRO	6.02214076e23
#define RPCST_PLANCK	6.62607015e-34
#define RPCST_LIGHT		299792458
#define RPCST_GRAV		6.674301e-11
#define RPCST_CHARGE	1.602176634e-19


#define BROKEN( )		exit( fprintf( stderr, "Invalid formula.  Try a ?\n" ) )
#define STATE( )		printf( "a = %f, b = %f\n", a, b )
#define PLIM( _x, _m )	if( _x > _m ) exit( fprintf( stderr, "Internal value limit - max is %f, value %f\n", _m, _x ) )

// stack structure
typedef struct stacker STACK;
struct stacker
{
	double					*	vals;
	long long int			*	fact;		// factorial record
	int							size;
	int							curr;
	int							out;		// single type
	int							flags;
	int							prec;
};

// stack interface
STACK *make_stack( int max );
void push( STACK *s, double v );
void pop( STACK *s, double *a, double *b );
long long int getfact( STACK *s, int which );
double peek( STACK *s, int idx );
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
