/* /usr2/utility/tee_process.c */
/* --------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( argc, argv )
int argc;
char **argv;
{
	char input_buffer[ 65537 ];
	FILE *output_pipe;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: tee_process.e \"command_string\"\n" );
		exit( 1 );
	}

	output_pipe = popen( argv[ 1 ], "w" );
	if ( !output_pipe )
	{
		fprintf( stderr, "ERROR tee_process.e: cannot open pipe\n" );
		exit( 1 );
	}

	while( get_line( input_buffer, stdin ) )
	{
		fprintf( output_pipe, "%s\n", input_buffer );
		fflush( output_pipe );

		printf( "%s\n", input_buffer );
		fflush( stdout );
	}

	pclose( output_pipe );
	return 0;
}
