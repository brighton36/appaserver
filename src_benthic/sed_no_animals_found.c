/* ---------------------------------------------------	*/
/* src_benthic/sed_no_animals_found.c			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

char *pad_zeros( int zeros_to_pad );

int main( int argc, char **argv )
{
	char input_buffer[ 1024 ];
	int leading_column_count;
	int caridean_shrimp_count;
	int crab_count;	
	int fish_count;
	int penaeid_shrimp_count;
	int commas_to_expect;
	int comma_count;

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s leading_column_count caridean_shrimp_count crab_count fish_count penaeid_shrimp_count\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	leading_column_count = atoi( argv[ 1 ] );
	caridean_shrimp_count = atoi( argv[ 2 ] );
	crab_count = atoi( argv[ 3 ] );
	fish_count = atoi( argv[ 4 ] );
	penaeid_shrimp_count = atoi( argv[ 5 ] );

	commas_to_expect = leading_column_count - 2;
	commas_to_expect += ( caridean_shrimp_count );
	commas_to_expect += ( crab_count );
	commas_to_expect += ( fish_count );
	commas_to_expect += ( penaeid_shrimp_count );

	while( get_line( input_buffer, stdin ) )
	{
		comma_count = character_count( ',', input_buffer );

		printf(	"%s%s\n",
			input_buffer,
			pad_zeros( commas_to_expect - comma_count ) );
	}

	return 0;
} /* main() */

char *pad_zeros( int zeros_to_pad )
{
	static char return_string[ 1024 ] = {0};
	char *ptr = return_string;
	int i;

	if ( zeros_to_pad <= 0 ) return "";

	for ( i = 0; i < zeros_to_pad; i++ )
	{
		ptr += sprintf( ptr, ",z" );
	}

	*ptr = '\0';
	return return_string;
} /* pad_zeros() */

