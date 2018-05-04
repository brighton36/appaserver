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

void output_in_clause_stdin( void );
void output_in_clause_parameter( char *argv_1 );

int main( int argc, char **argv )
{
	if ( argc == 2 )
	{
		output_in_clause_parameter( argv[ 1 ] );
	}
	else
	{
		output_in_clause_stdin();
	}

	return 0;

} /* main() */

void output_in_clause_stdin( void )
{
	char input_buffer[ 1024 ];
	LIST *data_list;
	data_list = list_new();

	while( get_line( input_buffer, stdin ) )
		list_append_pointer(
			data_list,
			strdup( input_buffer ) );

	printf( "in (%s)\n", timlib_with_list_get_in_clause( data_list ) );

} /* output_in_clause_stdin() */

void output_in_clause_parameter( char *argv_1 )
{
	printf( "in (%s)\n", timlib_get_in_clause( argv_1 ) );

} /* output_in_clause_parameter() */
