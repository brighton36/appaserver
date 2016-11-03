/* ---------------------------------------------------	*/
/* src_benthic/sed_in_zeros.c				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"

int main( void )
{
	char input_buffer[ 1024 ];
	char piece_buffer[ 128 ];
	int i;
	int comma_count;

	while( get_line( input_buffer, stdin ) )
	{
		comma_count = character_count( ',', input_buffer );

		for(	i = 0;
			i <= comma_count;
			i++ )
		{
			*piece_buffer = '\0';
			piece( piece_buffer, ',', input_buffer, i );

			if ( !*piece_buffer )
			{
				piece_replace( input_buffer, ',', "0", i );
			}
		}
		printf(	"%s\n", input_buffer );
	}

	return 0;
} /* main() */

