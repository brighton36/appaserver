/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/populate_print_checks_entity.c	*/
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
void output_entity_list(	FILE *output_pipe,
				LIST *entity_list );

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

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s ignored fund\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

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
	PAY_LIABILITIES *p;

	p = pay_liabilities_calloc();

	p->input.current_liability_account_list =
		pay_liabilities_fetch_current_liability_account_list(
			application_name,
			fund_name,
			pay_liabilities_fetch_liability_account_list(
				application_name )
				/* exclude_account_name_list */ );

/*
fprintf( stderr, "%s/%s()/%d: got input current_liability_account_list = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
ledger_account_list_display( p->input.current_liability_account_list ) );
*/

	p->input.liability_account_entity_list =
		pay_liabilities_fetch_liability_account_entity_list(
				application_name );

/*
fprintf( stderr, "%s/%s()/%d: got input liability_account_entity_list = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
entity_list_display( p->input.liability_account_entity_list ) );
*/

	p->process.current_liability_entity_list =
		pay_liabilities_get_current_liability_entity_list(
			p->input.current_liability_account_list,
			p->input.liability_account_entity_list
				/* exclude_entity_list */ );

/*
fprintf( stderr, "%s/%s()/%d: got process current_liability_entity_list = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
entity_list_display( p->process.current_liability_entity_list ) );
*/

	output_pipe = popen( "sort", "w" );

	output_entity_list(	output_pipe,
				p->input.liability_account_entity_list );

	output_entity_list(	output_pipe,
				p->process.current_liability_entity_list );

	pclose( output_pipe );

} /* populate_pay_liabilities_entity() */

void output_entity_list(	FILE *output_pipe,
				LIST *entity_list )
{
	ENTITY *entity;

	if ( !list_rewind( entity_list ) ) return;

	do {
		entity = list_get_pointer( entity_list );

		fprintf( output_pipe,
			 "%s^%s [%.2lf]\n",
			 entity->full_name,
			 entity->street_address,
			 entity->sum_balance );

	} while( list_next( entity_list ) );

} /* output_entity_list() */

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

