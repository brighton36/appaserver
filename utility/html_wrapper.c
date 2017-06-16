/* html_wrapper.c */
/* ------------------------------------------------------------ */
/*								*/
/* Tim Riley							*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 4096 ];
	char *style_sheet = {0};

	if ( argc == 2 ) style_sheet = argv[ 1 ];

/*
	document_output_html_stream( stdout );
*/

	printf( "<html>\n" );

	if ( style_sheet )
	{
		printf( "<link rel=stylesheet type=text/css href=%s>\n",
			style_sheet );
	}

	while( get_line( buffer, stdin ) ) puts( buffer );

	printf( "</html>\n" );

	return 0;
} /* main() */
