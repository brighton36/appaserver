/* sedmv.c		    */
/* ------------------------ */
/* Tim Riley		    */
/* ------------------------ */

#include <stdio.h>
#include "name_arg.h"

main( argc, argv )
int argc;
char **argv;
{
	char *search_string;
	char *replace_string;
	char *testing_string;

        NAME_ARG *arg = init_arg( argv[ 0 ] );

	setup_arg( arg, argc, argv );
	fetch_parameters(	arg, 
				&search_string, 
				&replace_string, 
				&testing_string );

	if ( argc == 1 )
	{
		exit_usage( arg );
	}

	sedmv(		search_string, 
			replace_string, 
			testing_string );

	return 0;

} /* main() */


fetch_parameters(	arg, 
			search_string,
			replace_string,
			testing_string )
NAME_ARG *arg;
char **search_string;
char **replace_string;
char **testing_string;
{
	*search_string = fetch_arg( arg, "search" );
	*replace_string = fetch_arg( arg, "replace" );
	*testing_string = fetch_arg( arg, "testing" );
} /* fetch_parameters() */

setup_arg( arg, argc, argv )
NAME_ARG *arg; 
int argc;
char **argv;
{
        int ticket;

        ticket = add_valid_option( arg, "search" );
        ticket = add_valid_option( arg, "replace" );

        ticket = add_valid_option( arg, "testing" );
        add_valid_value( arg, ticket, "yes" );
        add_valid_value( arg, ticket, "no" );
        set_default_value( arg, ticket, "yes" );

        ins_all( arg, argc, argv );

} /* setup_arg() */


sedmv(		char *search_string, 
		char *replace_string, 
		char *testing_string )
{
	fprintf( stderr, "Not written yet.\n" );

} /* sedmv() */
