/* matrix2dictionary.c						*/
/* ------------------------------------------------------------ */
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "column.h"
#include "timlib.h"
#include "list.h"

int main( int argc, char **argv )
{
	char delimiter;
	char buffer[ 1024 ];
	LIST *heading_list;
	LIST *data_list;

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s delimiter\n", argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	if ( !get_line( buffer, stdin ) ) exit( 1 );

	if ( *buffer == '#' ) strcpy( buffer, buffer + 1 );

	heading_list = list_string2list( buffer, delimiter );

	while( get_line( buffer, stdin ) )
	{
		data_list = list_string2list( buffer, delimiter );

		if ( !list_length( data_list )
		|| ( list_length( data_list ) != list_length( heading_list ) ) )
		{
			continue;
		}

		list_rewind( data_list );
		list_rewind( heading_list );

		do {
			printf( "%s^%s\n",
				(char *)list_get_pointer( heading_list ),
				(char *)list_get_pointer( data_list ) );
			list_next( heading_list );
		} while( list_next ( data_list ) );
		list_free_string_list( data_list );
	}

	return 0;
} /* main() */

