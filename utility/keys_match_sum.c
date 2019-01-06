/* $APPASERVER_HOME/utility/keys_match_sum.c	*/
/* ----------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"

/* Constants */
/* --------- */
#define DEFAULT_DELIMITER	'^'

/* Prototypes */
/* ---------- */
void keys_match_sum(	double match_sum,
			int key_piece_offset,
			char delimiter,
			LIST *input_list );

int main( int argc, char **argv )
{
	char delimiter = DEFAULT_DELIMITER;
	int key_piece_offset = 0;
	double match_sum;
	LIST *input_list;

	if ( argc < 2 )
	{
		fprintf(stderr,
			"Usage: %s match_sum [key_piece_offset] [delimiter]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	match_sum = atof( argv[ 1 ] );

	if ( argc >= 3 ) key_piece_offset = atoi( argv[ 2 ] );

	if ( argc >= 4 ) delimiter = *argv[ 3 ];

	input_list = pipe2list( "cat" );

	if ( list_length( input_list ) )
	{
		keys_match_sum(
			match_sum,
			key_piece_offset,
			delimiter,
			input_list );
	}

	return 0;

} /* main() */

void keys_match_sum(	double match_sum,
			int key_piece_offset,
			char delimiter,
			LIST *input_list )
{
	printf( "%s\n", list_display( input_list ) );

} /* keys_match_sum() */
