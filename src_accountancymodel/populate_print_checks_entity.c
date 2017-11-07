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

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
LIST *fetch_liability_account_list(
				char *application_name );

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
	ACCOUNT *account;
	LIST *liability_account_list;

	liability_account_list =
		fetch_liability_account_list(
			application_name );

	output_pipe = popen( "sort", "w" );

	output_checks_not_taxes(
			output_pipe,
			application_name,
			fund_name );

	if ( list_rewind( liability_account_list ) )
	{
		do {
			account = list_get_pointer( liability_account_list );

			fprintf( output_pipe,
				 "%s^%s [%.2lf]\n",
				 account->latest_ledger->full_name,
				 account->latest_ledger->street_address,
				 account->latest_ledger->balance );

		} while( list_next( liability_account_list ) );
	}

	pclose( output_pipe );

} /* populate_print_checks_entity() */

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

LIST *fetch_liability_account_list(
				char *application_name )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char account_name[ 128 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char *folder;
	char *select;
	JOURNAL_LEDGER *latest_ledger;
	ACCOUNT *account;
	LIST *liability_account_list = list_new();

	folder = "liability_account_entity";
	select = "account,full_name,street_address";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	",
		 application_name,
		 select,
		 folder );

	input_pipe = popen( sys_string, "r" );

	while( timlib_get_line( input_buffer, input_pipe, 512 ) )
	{
		piece( account_name, FOLDER_DATA_DELIMITER, input_buffer, 0 );

		if ( ! ( latest_ledger =
				ledger_get_latest_ledger(
					application_name,
					account_name,
					(char *)0 /* as_of_date */ ) ) )
		{
			continue;
		}

		if ( !latest_ledger->balance ) continue;

		piece( full_name, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		piece( street_address, FOLDER_DATA_DELIMITER, input_buffer, 2 );

		latest_ledger->full_name = strdup( full_name );

		latest_ledger->street_address =
			strdup( street_address );

		account = ledger_account_new( account_name );
		account->latest_ledger = latest_ledger;

		list_append_pointer( liability_account_list, account );
	}

	pclose( input_pipe );

	return liability_account_list;

} /* fetch_liability_account_list() */

