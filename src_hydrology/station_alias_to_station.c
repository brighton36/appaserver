/* station_alias_to_station.c */
/* -------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "station_alias.h"
#include "piece.h"


int main( int argc, char **argv )
{
	char buffer[ 4096 ];
	char *application;
	char station_alias_string[ 128 ];
	int station_alias_piece;
	char delimiter;
	STATION_ALIAS *station_alias;
	char *station;

	if ( argc != 4 )
	{
		fprintf(stderr,
"Usage: %s application station_alias_piece delimiter\n",
			argv[ 0 ] );
		fprintf(stderr, "Expected on stdin: delimited records\n" );
		exit( 1 );
	}

	application = argv[ 1 ];
	station_alias_piece = atoi( argv[ 2 ] );
	delimiter = *argv[ 3 ];

	station_alias = new_station_alias( application );
	while( get_line( buffer, stdin ) )
	{
		if ( !*buffer || *buffer == '#' ) continue;

		if ( !piece(	station_alias_string, 
				delimiter, 
				buffer, 
				station_alias_piece ) )
		{
			fprintf( stderr, "%s\n", buffer );
			continue;
		}

		station = station_alias_get_station(
					station_alias->station_alias_dictionary,
					station_alias_string );
		if ( station )
		{
			piece_replace(	buffer, 
					delimiter, 
					station, 
					station_alias_piece );
			printf( "%s\n", buffer );
		}
		else
		{
			fprintf( stderr, "%s\n", buffer );
		}
	}
	exit( 0 );
} /* main() */

