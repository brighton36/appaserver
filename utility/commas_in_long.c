/* commas_in_long.c				*/
/* -------------------------------------------- */
/* 						*/
/* Tim Riley					*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

char *commas_in_long( long n );

char *commas_in_long( long n )
{
	return place_commas_in_long( n );

} /* commas_in_long() */

int main( int argc, char **argv )
{
	char buffer[ 1024 ];

	if ( argc == 1 )
	{
		while( get_line( buffer, stdin ) )
			printf( "%s\n", commas_in_long( atol( buffer ) ) );
	}
	else
	{
		while( --argc )
			printf( "%s\n", commas_in_long( atol( *++argv ) ) );
	}

	return 0;
} /* main() */

