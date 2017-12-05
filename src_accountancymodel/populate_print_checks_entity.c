/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_accountancymodel/populate_print_checks_entity.c	*/
/* --------------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "entity.h"
#include "ledger.h"
#include "pay_liabilities.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void output_checks_not_taxes(
				FILE *output_pipe,
				char *application_name,
				char *fund_name );

void populate_print_checks_entity(
				char *application_name,
				char *fund_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name;
	char *database_string = {0};

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s application fund\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	fund_name = argv[ 2 ];

	populate_print_checks_entity(
		application_name,
		fund_name );

	return 0;

} /* main() */

void populate_print_checks_entity(
				char *application_name,
				char *fund_name )
{
	FILE *output_pipe;
	ENTITY *entity;
	PAY_LIABILITIES *pay_liabilities;
	LIST *liability_account_entity_list;

	output_pipe = popen( "sort", "w" );

	output_checks_not_taxes(
			output_pipe,
			application_name,
			fund_name );

	liability_account_entity_list =
		pay_liabilities_fetch_liability_account_entity_list(
			application_name );

	if ( !list_rewind( liability_account_entity_list ) )
	{
		pclose( output_pipe );
		return;
	}

	do {
		entity = list_get_pointer( liability_account_entity_list );

		fprintf( output_pipe,
			 "%s^%s [%.2lf]\n",
			 entity->full_name,
			 entity->street_address,
			 entity->sum_balance );

	} while( list_next( liability_account_entity_list ) );

	pclose( output_pipe );

} /* populate_pay_liabilities_entity() */

void output_checks_not_taxes(
				FILE *output_pipe,
				char *application_name,
				char *fund_name )
{
	char sys_string[ 1024 ];
	char input_buffer[ 512 ];
	FILE *input_pipe;

	sprintf( sys_string,
		 "populate_print_checks_entity.sh %s \"%s\" not_taxes",
		 application_name,
		 fund_name );

	input_pipe = popen( sys_string, "r" );

	while( timlib_get_line( input_buffer, input_pipe, 512 ) )
	{
		fprintf( output_pipe, "%s\n", input_buffer );
	}

	pclose( input_pipe );

} /* output_checks_not_taxes() */

