/* ------------------------------------------------------- */
/* utility/in_clause.c		       			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "timlib.h"

int main( void )
{
	char input_buffer[ 1024 ];
	LIST *data_list;

	data_list = list_new();

	while( get_line( input_buffer, stdin ) )
		list_append_pointer( data_list, strdup( input_buffer ) );

	printf( "in (%s)\n", timlib_with_list_get_in_clause( data_list ) );

	return 0;
} /* main() */

