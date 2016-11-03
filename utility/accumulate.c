/* utility/accumulate.c	*/
/* -------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"

#define DEFAULT_DELIMITER	'^'

int main( int argc, char **argv )
{
	double sum = 0.0;
	char input_buffer[ 1024 ];
	char piece_buffer[ 1024 ];
	char delimiter = DEFAULT_DELIMITER;
	int piece_offset = 0;
	boolean append;

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s piece_offset delimiter replace|append\n",
			argv[ 0 ] );
		exit( 1 );
	}

	piece_offset = atoi( argv[ 1 ] );
	delimiter = *argv[ 2 ];

	if ( strcmp( argv[ 3 ], "replace" ) == 0 )
		append = 0;
	else
	if ( strcmp( argv[ 3 ], "append" ) == 0 )
		append = 1;
	else
	{
		fprintf(stderr,
			"ERROR in %s: must be either 'replace' or 'append'\n",
			argv[ 0 ] );
		exit( 1 );
	}

	while( get_line( input_buffer, stdin ) )
	{
		if ( !*input_buffer ) continue;
		if ( *input_buffer == '#' ) continue;

		*piece_buffer = '\0';

		if ( ! piece(	piece_buffer,
				delimiter,
				input_buffer,
				piece_offset ) )
		{
			fprintf(stderr,
	"Warning in %s/%s()/%d: cannot piece('%c',%d): (%s); continuing.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				delimiter,
				piece_offset,
				input_buffer );
			continue;
		}

		sum += atof( piece_buffer );
		sprintf( piece_buffer, "%.3lf", sum );

		if ( append )
		{
			printf( "%s\n",
				piece_insert(
					input_buffer, 
					delimiter, 
					piece_buffer, 
					piece_offset + 1 ) );
		}
		else
		{
			printf( "%s\n",
				piece_replace(
					input_buffer, 
					delimiter, 
					piece_buffer, 
					piece_offset ) );
		}
	}
	return 0;
} /* main() */

