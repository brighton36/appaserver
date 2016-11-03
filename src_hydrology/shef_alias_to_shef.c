/* shef_alias_to_shef.c */
/* -------------------- */

#include <stdio.h>
#include <string.h>
#include "timlib.h"
#include "shef_alias.h"


int main( int argc, char **argv )
{
	char buffer[ 4096 ];
	char *entity;
	char shef_alias_string[ 128 ];
	int shef_alias_piece;
	char delimiter;
	SHEF_ALIAS *shef_alias;
	char *shef;

	if ( argc != 4 )
	{
		fprintf(stderr,
		"Usage: %s entity shef_alias_piece delimiter\n",
			argv[ 0 ] );
		fprintf(stderr, "Expected on stdin: delimited records\n" );
		exit( 1 );
	}

	entity = argv[ 1 ];
	shef_alias_piece = atoi( argv[ 2 ] );
	delimiter = *argv[ 3 ];

	shef_alias = new_shef_alias( entity );
	while( gets( buffer ) )
	{
		if ( !*buffer || *buffer == '#' ) continue;

		if ( !piece(	shef_alias_string, 
				delimiter, 
				buffer, 
				shef_alias_piece ) )
		{
			fprintf( stderr, "Cannot piece = %d of (%s)\n", 
				 shef_alias_piece, buffer );
			continue;
		}
		shef = shef_alias_get_shef(
					shef_alias->shef_alias_dictionary,
					shef_alias_string );
		if ( shef )
		{
			piece_replace(	buffer, 
					delimiter, 
					shef, 
					shef_alias_piece );
			printf( "%s\n", buffer );
		}
		else
		{
			printf( "%s\n", buffer );
		}
	}
} /* main() */

