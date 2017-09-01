/* $APPASERVER_HOME/utility/breakin_whitelist.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "fopen_path.h"

#define WHITELIST_FILE	"ufw_whitelist.dat"

int main( void )
{
	FILE *input_file;
	char buffer[ 1024 ];
	FILE *output_pipe;

	output_pipe = popen( "sort -n", "w" );

	if ( ( input_file = fopen_path( WHITELIST_FILE, "r" ) ) )
	{
		while( get_line( buffer, input_file ) )
		{
			fprintf( output_pipe, "%s\n", buffer );

		}
		pclose( output_pipe );
	}

	return 0;

} /* main() */

