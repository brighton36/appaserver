/* utility/trim.c					*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( void )
{
	char buffer[ 65536 ];

	while( get_line( buffer, stdin ) )
	{
		printf( "%s\n", trim( buffer ) );
	}
	return 0;

} /* main() */

