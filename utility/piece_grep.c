/* html/utility/piece_grep.c */
/* ------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"

int main( int argc, char **argv )
{
	int piece_offset;
	int delimiter;
	char *pattern_to_search;
	char input_buffer[ 65536 ];
	boolean grep_exclude = 0;

	if ( argc < 4 )
	{
		fprintf(
		stderr,
		"Usage: %s delimiter offset search_1[,search_n] [-v]\n",
		argv[ 0 ] );

		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	piece_offset = atoi( argv[ 2 ] );
	pattern_to_search = argv[ 3 ];

	if ( argc == 5 && strcmp( argv[ 4 ], "-v" ) == 0 )
	{
		grep_exclude = 1;
	}

	while( get_line( input_buffer, stdin ) )
	{
		if ( piece_grep(	input_buffer,
					delimiter, 
					pattern_to_search,
					piece_offset,
					grep_exclude ) )
		{
			printf( "%s\n", input_buffer );
		}
	}
	return 0;

} /* main() */



#ifdef NOT_DEFINED
piece_grep( delimiter, piece_offset, pattern_to_search, output_if_non_exists )
int delimiter;
int piece_offset;
char *pattern_to_search;
int output_if_non_exists;
{
	char buffer[ 1024 ];
	char p_buffer[ 128 ];

	while( get_line( buffer, stdin ) )
	{
		if ( ! piece( p_buffer, delimiter, buffer, piece_offset ) )
		{
			fprintf( stderr, 
			"piece_grep.e: cannot get piece: %d\n(%s)\n",
				 piece_offset,
				 buffer );
			continue;
		}


		if ( output_if_non_exists )
		{
			if ( !pattern_exists(	pattern_to_search, 
						p_buffer ) )
			{
				printf( "%s\n", buffer );
			}
		}
		else
		{
			if ( pattern_exists(	pattern_to_search, 
						p_buffer ) )
			{
				printf( "%s\n", buffer );
			}
		}
	}

} /* piece_grep() */


pattern_exists( pattern_to_search, search_this )
char *pattern_to_search, *search_this;
{
	char piece_buffer[ 128 ];
	int i;

	for( i = 0; piece( piece_buffer, ',', pattern_to_search, i ); i++ )
	{
		if ( strcmp( piece_buffer, search_this ) == 0 )
			return 1;
	}

	return 0;

} /* pattern_exists() */

#endif
