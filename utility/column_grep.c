/* utility/column_grep.c */
/* --------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "column.h"

void column_grep( int column_offset, char *pattern_to_search );

int main( int argc, char **argv )
{
	int column_offset;

	if ( argc != 3 )
	{
		fprintf( stderr, 
	"Usage: column_grep.e column_offset patter_to_search\n" );
		exit( 1 );
	}

	column_offset = atoi( argv[ 1 ] );

	column_grep( column_offset, argv[ 2 ] );
	return 0;
} /* main() */



void column_grep( int column_offset, char *pattern_to_search )
{
	char buffer[ 1024 ];
	char column_buffer[ 128 ];

	while( get_line( buffer, stdin ) )
	{
		if ( ! column( 	column_buffer, 
				column_offset, 
				buffer ) )
		{
			fprintf( stderr, 
			"column_grep.e: cannot get column: %d\n(%s)\n",
				 column_offset,
				 buffer );
			continue;
		}


		if ( strcmp( pattern_to_search, column_buffer ) == 0 )
			printf( "%s\n", buffer );
	}

} /* column_grep() */


