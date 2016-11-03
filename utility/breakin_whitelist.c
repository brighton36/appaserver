/* utility/breakin_whitelist.c				*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "column.h"
#include "fopen_path.h"

#define WHITELIST_FILE	"breakin_whitelist.dat"

int main( void )
{
	FILE *input_file;
	LIST *whitelist = {0};
	char column_buffer[ 1024 ];
	char buffer[ 1024 ];
	FILE *output_pipe;

	output_pipe = popen( "sort -n 1>&2", "w" );

	if ( ( input_file = fopen_path( WHITELIST_FILE, "r" ) ) )
	{
		whitelist = list_new();

		fflush( stderr );
		fprintf( stderr, "Whitelist:\n" );
		fprintf( stderr, "===============\n" );
		fflush( stderr );

		while( get_line( buffer, input_file ) )
		{
			fprintf( output_pipe, "%s\n", buffer );

			column( column_buffer, 0, buffer );

			list_append_pointer(	whitelist,
						strdup( column_buffer ) );
		}
		pclose( output_pipe );
		fflush( stderr );
		fprintf( stderr, "===============\n" );
		fflush( stderr );
		fclose( input_file );
	}

	while( get_line( buffer, stdin ) )
	{
		if ( !list_exists_string_beginning( whitelist, buffer ) )
			printf( "%s\n", buffer );
	}
	return 0;

} /* main() */

