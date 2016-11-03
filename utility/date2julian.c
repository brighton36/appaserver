/* date2julian.c */
/* ------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "julian.h"
#include "date_convert.h"

#define DEFAULT_DELIMITER		'|'

int main( int argc, char **argv )
{
	int piece_offset;
	char input_buffer[ 1024 ];
	char piece_buffer[ 128 ];
	char julian_string[ 64 ];
	JULIAN *julian;
	DATE_CONVERT *date_convert;
	char delimiter;

	if ( argc < 2 )
	{
		fprintf( stderr, 
		"Usage: %s piece_offset [delimiter]\n", argv[ 0 ] );
		exit( 1 );
	}

	piece_offset = atoi( argv[ 1 ] );

	if ( argc == 3 )
		delimiter = *argv[ 2 ];
	else
		delimiter = DEFAULT_DELIMITER;

	while( get_line( input_buffer, stdin ) )
	{
		if ( ! piece(	piece_buffer,
				delimiter,
				input_buffer,
				piece_offset ) )
		{
			printf( "Warning: cannot get date in (%s)\n",
				input_buffer );
			continue;
		}

		date_convert = date_convert_new_date_convert(
				international /* destination_format */,
				piece_buffer );

		if ( !date_convert ) continue;

		julian = julian_yyyy_mm_dd_new( date_convert->return_date );

		if (!julian ) continue;

		date_convert_free( date_convert );

		sprintf( julian_string, "%.1lf", julian->current );

		printf( "%s\n", 
			replace_piece(	input_buffer,
					delimiter,
					julian_string, 
					piece_offset ) );
		julian_free( julian );
	}

	return 0;

} /* main() */

