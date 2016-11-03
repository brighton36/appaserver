/* merged_datasets_remove_all_but_first_heading.c       */
/* ---------------------------------------------------- */
/* This is used in merged_datasets_output_clean_html.sh */
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( void )
{
	char input_buffer[ 1024 ];
	int gotit = 0;

	while( fgets( input_buffer, 1023, stdin ) )
	{
		if ( strncmp( input_buffer, "Date", 4 ) == 0 )
		{
			if ( !gotit )
				gotit = 1;
			else
				continue;
		}
		printf( "%s", input_buffer );
	}
	exit( 0 );

} /* main() */

