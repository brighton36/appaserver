/* double_quote_comma_delimited.c */
/* ------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"

void local_double_quote_comma_delimited( char *string, char delimiter );

int main( int argc, char **argv )
{
	char buffer[ 65536 ];
	char delimiter;

	if ( argc < 2 )
	{
		fprintf( stderr, "Usage: %s delimiter [string]\n", argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	if ( argc == 3 )
	{
		local_double_quote_comma_delimited( argv[ 2 ], delimiter );
	}
	else
	{
		while( get_line( buffer, stdin ) )
			local_double_quote_comma_delimited( buffer, delimiter );
	}

	return 0;

} /* main() */

void local_double_quote_comma_delimited( char *string, char delimiter )
{
	char buffer[ 4096 ];
	int i;

	for( i = 0; piece( buffer, delimiter, string, i ); i++ )
	{
		if ( i != 0 ) printf( "," );

		printf( "\"%s\"", buffer );
	}
	printf( "\n" );

} /* local_double_quote_comma_delimited() */

