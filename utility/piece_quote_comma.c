/* utility/piece_quote_comma.c 				*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "piece.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 4096 ];
	char offset_str[ 128 ];
	char component[ 4096 ];
	char *offset_list_string;
	int offset, i;
	char destination_delimiter = ',';

	if ( argc < 2 )
	{
		fprintf( stderr, 
	"Usage: %s offset_1[,...,offset_n] [destination_delimiter]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	offset_list_string = argv[ 1 ];

	if ( argc == 3 ) destination_delimiter = *argv[ 2 ];

	while( get_line( buffer, stdin ) )
	{
		for( i = 0; piece(	offset_str,
					',',
					offset_list_string,
					i ); i++ )
		{
			offset = atoi( offset_str );

			if ( offset < 0 )
			{
				fprintf( stderr,
			 "ERROR in :%s: Offset (%d) is less than zero\n",
			 		argv[ 0 ],
			 		offset );
				exit( 1 );
			}
	
			if ( !piece_quote_comma(
						component,
						buffer,
						offset ) )
			{
				fprintf( stderr,
		"Warning %s: There are less than %d delimiters in: (%s)\n",
					argv[ 0 ],
				 	offset,
				 	buffer );
			}
			else
			{
				if ( i == 0 )
					printf( "%s", component );
				else
					printf( "%c%s",
						destination_delimiter,
						component );
			}
		}
		printf( "\n" );
	}

	return 0;
} /* main() */

