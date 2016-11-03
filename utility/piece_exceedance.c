/* --------------------------------------------------- 	*/
/* piece_exceedance.c				       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"

/* Typedefs */
/* -------- */
typedef struct
{
	double value;
	char *input_string;
} VALUE;

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
int get_count_below(			LIST *value_list,
					double value );
void output_value_list(			LIST *value_list,
					char delimiter );
VALUE *new_value(
					double value,
					char *input_string );

LIST *load_value_list(			char delimiter );

int main( int argc, char **argv )
{
	LIST *value_list;
	char delimiter;

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s delimiter\n",
			argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	value_list = load_value_list( delimiter );

	if ( list_length( value_list ) )
		output_value_list( value_list, delimiter );

	return 0;
} /* main() */

void output_value_list( LIST *value_list, char delimiter )
{
	VALUE *value;
	int count_below;

	list_rewind( value_list );

	do {
		value = (VALUE *) list_get_pointer( value_list );

		count_below = get_count_below( value_list, value->value );

		printf( "%s%c%d%c%.3lf\n",
			value->input_string,
			delimiter,
			count_below,
			delimiter,
			( (double)count_below /
			(double)list_length( value_list ) ) * 100 );

	} while( list_next( value_list ) );
} /* output_value_list() */


LIST *load_value_list( char delimiter )
{
	char sys_string[ 128 ];
	char input_buffer[ 1024 ];
	char value_string[ 128 ];
	FILE *input_pipe;
	VALUE *value;
	LIST *value_list;

	value_list = list_new_list();

	strcpy( sys_string, "sort -nr" );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( value_string, delimiter, input_buffer, 0 );

		if ( *value_string
		&& ( strcasecmp( value_string, "null" ) != 0 ) )
		{
			value = new_value(	atof( value_string ),
						strdup( input_buffer ) );

			list_append_pointer(	value_list,
						value );
		}
	}

	pclose( input_pipe );
	return value_list;

} /* load_value_list() */

VALUE *new_value(	double value,
			char *input_string )
{
	VALUE *v;

	v = (VALUE *)calloc( 1, sizeof( VALUE ) );
	v->value = value;
	v->input_string = input_string;
	return v;
} /* new_value() */

int get_count_below( LIST *value_list, double value )
{
	int count_below = 0;
	VALUE *v;

	if ( list_at_end( value_list ) ) return 0;

	list_push( value_list );
	list_next( value_list );

	do {
		v = (VALUE *) list_get_pointer( value_list );

		if ( value != v->value )
			count_below++;
	} while( list_next( value_list ) );

	list_pop( value_list );
	return count_below;
} /* get_count_below() */

