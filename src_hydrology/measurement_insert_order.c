/* --------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/measurement_insert_order.c	*/
/* --------------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org		*/
/* --------------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "boolean.h"
#include "list.h"
#include "measurement.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
LIST *get_order_integer_list( char *order_integer_list_string );

int main( int argc, char **argv )
{
	char input_buffer[ 1024 ];
	MEASUREMENT *measurement;
	char delimiter;
	LIST *order_integer_list;

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s delimiter order_integer_list\n",
			argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	order_integer_list = get_order_integer_list( argv[ 2 ] );

	while( timlib_get_line( input_buffer, stdin, 1024 ) )
	{
		measurement =
			measurement_variable_parse(
					input_buffer,
					delimiter,
					order_integer_list );

		if ( !measurement->null_value )
		{
			printf( "%s%c%s%c%s%c%s%c%.3lf\n",
				measurement->station_name,
				delimiter,
				measurement->datatype,
				delimiter,
				measurement->measurement_date,
				delimiter,
				measurement->measurement_time,
				delimiter,
				measurement->measurement_value );
		}
		else
		{
			printf( "%s%c%s%c%s%c%s%c\n",
				measurement->station_name,
				delimiter,
				measurement->datatype,
				delimiter,
				measurement->measurement_date,
				delimiter,
				measurement->measurement_time,
				delimiter );
		}

		measurement_free( measurement );
	}

	return 0;

} /* main() */

LIST *get_order_integer_list( char *order_integer_list_string )
{
	LIST *return_list = list_new();
	int *order_integer_ptr;
	int i;
	char piece_buffer[ 16 ];

	for(	i = 0;
		piece( piece_buffer, ',', order_integer_list_string, i );
		i++ )
	{
		order_integer_ptr = (int *)calloc( 1, sizeof( int ) );
		*order_integer_ptr = atoi( piece_buffer );
		list_append_pointer( return_list, order_integer_ptr );
	}

	return return_list;

} /* get_order_integer_list() */

