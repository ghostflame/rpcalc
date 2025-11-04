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
* args.c - awful big switch, calculator implementation                    *
*                                                                         *
* Updates:                                                                *
**************************************************************************/

#include "rpcalc.h"


// because - gets processed separately, we will get negative as a
// param, not part of the number

char *handle_number( STACK *s, char *arg, int neg )
{
	int64_t j;
	int dopush = 0;
	long double a;
	char *p, *q;

	p = arg;

	switch( *p )
	{
		case '0':
			// detect octal, hex and bin
			switch( *(p+1) )
			{
				case 'x':
				case 'X':
					// step over that
					p++;
					p++;

					if( hasinput( s, INTYPE_HEX ) )
					{
						j = strtoull( p, &q, 16 );
						a = (long double) j;
						p = q - 1;
					}
					else
					{
						a = strtold( p, &q );
						p = q - 1;
					}
					dopush = 1;
					break;

				case 'b':
				case 'B':
					// step over that
					p++;
					p++;

					if( hasinput( s, INTYPE_BIN ) )
					{
						j = strtoull( p, &q, 2 );
						a = (long double) j;
						p = q - 1;
					}
					else
					{
						a = strtold( p, &q );
						p = q - 1;
					}
					dopush = 1;
					break;

				// do people still do this?
				case 'o':
				case 'O':
					p++;
					p++;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
					if( hasinput( s, INTYPE_OCT ) )
					{
						j = strtoull( p, &q, 8 );
						a = (long double) j;
						p = q - 1;
					}
					else
					{
						a = strtold( p, &q );
						p = q - 1;
					}
					dopush = 1;
					break;
				case '8':
				case '9':
				case '.':
					a = strtold( p, &q );
					p = q - 1;
					dopush = 1;
					break;
	
				// else... it was input
				default:
					push( s, 0 );
					break;
			}
			break;

		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			a = strtold( p, &q );
			p = q - 1;
			dopush = 1;
			break;

		default:
			BROKEN( );
	}

	if( dopush )
	{
		a = ( neg ) ? -1.0 * a : a;
		push( s, a );
	}

	return p;
}




void handle_arg( STACK *s, char *arg )
{
	long double a, b;
	int64_t j, k;
	char *p;

	for( p = arg; *p; p++ )
	{
		// handle whitespace
		while( *p && isspace( *p ) )
			p++;

		// null is ignored
		switch( *p )
		{
			case '\0':
				break;

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
				p = handle_number( s, p, 0 );
				break;

			case '+':
				need( s, 2 );
				pop( s, &a, &b );
				a += b;
				push( s, a );
				break;

			case '-':
				if( *(p+1) >= '0' && *(p+1) <= '9' )
				{
					p = handle_number( s, p+1, 1 );
				}
				else
				{
					need( s, 2 );
					pop( s, &a, &b );
					push( s, a - b );
				}
				break;

			case 'x':	// to stop the shell grabbing *
			case '*':
				need( s, 2 );
				pop( s, &a, &b );
				push( s, a * b );
				break;

			case '/':
				need( s, 2 );
				pop( s, &a, &b );
				push( s, a / b );
				break;

			case '%':
				need( s, 2 );
				pop( s, &a, &b );
				j = (int64_t) a;
				k = (int64_t) b;
				push( s, (long double) ( j % k ) );
				break;

			case 'i':
				p++;
				switch( *p )
				{
					case 'n':
						need( s, 1 );
						pop( s, &a, NULL );
						if( a == 0.0L )
							BROKEN( );
						push( s, 1.0L / a );
						break;

					default:
						BROKEN( );
				}

				break;

			case 'r':		helper_root_around( s, *++p );		break;

			case 'n':
				helper_logs( s, *++p );
				break;

			case 't':
				helper_trig( s, *++p );
				break;

			case 'R':
				need( s, 1 );
				pop( s, &a, NULL );
				push( s, ( a * M_PIl ) / 180.0L );
				break;

			case 'D':
				need( s, 1 );
				pop( s, &a, NULL );
				push( s, ( a * 180.0L ) / M_PIl );
				break;

			case 'a':
				need( s, 1  );
				pop( s, &a, NULL );
				push( s, fabsl( a ) );
				break;

			case 'c':
				helper_const( s, *++p );
				break;

			case '^':
				need( s, 2 );
				pop( s, &a, &b );
				push( s, powl( a, b ) );
				break;

			case 's':
				helper_bitshift( s, *++p );
				break;

			case 'M':
				need( s, 2 );
				pop( s, &a, &b );
				j = (int64_t) a;
				k = (int64_t) b;
				if( ( j % k ) == 0 )
					a = 0.0;
				else
					a = 1.0;
				push( s, a );
				break;

			case 'Y':
				helper_fetch( s, *++p );
				break;

			case 'y':
				helper_store( s, *++p );
				break;

			case 'F':
				need( s, 1 );
				pop( s, &a, NULL);
				j = (int64_t) a;
				push( s, fibonnaci( j ) );
				if( j < 93 )
					setoutput( s, OUTTYPE_INT );
				break;

			case 'N':
				helper_fact( s, *++p );
				break;

			case 'E':
				helper_fact_est( s, *++p );
				setoutput( s, OUTTYPE_SCI );
				break;

			case 'S':
				helper_stack_ops( s, *++p );
				break;

			case '_':
				need( s, 1 );
				pop( s, &b, NULL );
				if( b < 0 || b > 0 )
					a = 0;
				else
					a = 1;
				push( s, a );
				break;

			// various functions
			case 'f':
				helper_fncall( s, *++p );
				break;

			case 'b':
				helper_bitwise( s, *++p );
				break;

			case '=':
				need( s, 2 );
				pop( s, &a, &b );
				if( a == b )
					a = 1;
				else
					a = 0;
				push( s, a );
				break;

			case 'G':
				need( s, 2 );
				pop( s, &a, &b );
				if( a > b )
					a = 1;
				else
					a = 0;
				push( s, a );
				break;

			case 'L':
				need( s, 2 );
				pop( s, &a, &b );
				if( a < b )
					a = 1;
				else
					a = 0;
				push( s, a );
				break;

			case 'g':
				need( s, 2 );
				pop( s, &a, &b );
				if( b > a )
					a = b;
				push( s, a );
				break;

			case 'l':
				need( s, 2 );
				pop( s, &a, &b );
				if( b < a )
					a = b;
				push( s, a );
				break;

			case 'I':
				p = helper_input_type( s, ++p );
				break;

			case 'o':
				helper_output_type( s, *++p );
				break;

			case 'p':
				helper_prime( s, *++p );
				break;

			case 'P':
				need( s, 1 );
				pop( s, &a, NULL );
				setprecision( s, (int) a );
				break;

			case ',':
				// number separator
				break;

			case '.':
				// stdin, despite commandline args
				setinput( s, INTYPE_TERM, 1 );
				break;

			case '?':
			case '@':
				usage( );
				exit( 0 );
				break;

			default:
				BROKEN( );
				break;
		}
	}
}

