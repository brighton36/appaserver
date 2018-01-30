/* --------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/load_bank_spreadsheet.c		*/
/* --------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "process.h"
#include "application.h"
#include "application_constants.h"
#include "bank_upload.h"

/* Prototypes */
/* ---------- */
int load_bank_spreadsheet(	char *application_name,
				char *fund_name,
				char *input_filename,
				boolean execute );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *fund_name;
	char *input_filename;
	boolean execute;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	int load_count;
	char buffer[ 128 ];

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process_name fund filename execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	fund_name = argv[ 3 ];
	input_filename = argv[ 4 ];
	execute = (*argv[ 5 ] == 'y');

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	printf( "<h1>%s\n",
		format_initial_capital( buffer, process_name ) );
	fflush( stdout );
	system( "date '+%x %H:%M'" );
	printf( "</h1>\n" );
	fflush( stdout );

	if ( !*input_filename || strcmp( input_filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	load_count =
		load_bank_spreadsheet(
			application_name,
			fund_name,
			input_filename,
			execute );

	if ( execute )
	{
		process_increment_execution_count(
			application_name,
			process_name,
			appaserver_parameter_file_get_dbms() );

		printf( "<p>Process complete with %d transactions.\n",
			load_count );
	}
	else
	{
		printf( "<p>Process did not load %d transactions.\n",
			load_count );
	}

	document_close();

	exit( 0 );

} /* main() */

int load_bank_spreadsheet(
			char *application_name,
			char *fund_name,
			char *input_filename,
			boolean execute )
{
	char sys_string[ 1024 ];
	FILE *input_file;
	char input_string[ 4096 ];
	char *table_name;
	char bank_date[ 128 ];
	char bank_date_international[ 128 ];
	char bank_description[ 1024 ];
	char bank_amount[ 128 ];
	char bank_running_balance[ 128 ];
	FILE *table_output_pipe = {0};
	FILE *bank_upload_insert_pipe = {0};
	boolean found_header = 0;
	char error_filename[ 128 ] = {0};
	char *insert_bank_download;
	BANK_UPLOAD_STRUCTURE *bank_upload_structure;

	bank_upload_structure =
		bank_upload_structure_new(
			fund_name,
			input_filename );

	if ( ! ( bank_upload_structure->starting_sequence_number =
			bank_upload_get_starting_sequence_number(
				application_name,
				input_filename ) ) )
	{
		printf( "<h2>ERROR: cannot get sequence number</h2>\n" );
		return 0;
	}

	if ( ! ( input_file =
			fopen(	bank_upload_structure->
					input_filename,
				"r" ) ) )
	{
		printf( "<h2>ERROR: cannot open %s for read.</h2>\n",
			input_filename );
		return 0;
	}

	bank_upload_structure->table_insert_count =
		bank_upload_table_insert(
			input_file,
			&bank_upload_structure->minimum_bank_date,
			application_name,
			bank_upload_structure->fund_name,
			execute,
			bank_upload_structure->
				starting_sequence_number );

	fclose( input_file );

	if ( !bank_upload_structure->table_insert_count ) return 0;

	bank_upload_structure->bank_upload_list =
		bank_upload_fetch_list(
			application_name,
			bank_upload_structure->
				starting_sequence_number );

	bank_upload_structure->existing_cash_journal_ledger_list =
		bank_upload_fetch_existing_cash_journal_ledger_list(
			application_name,
			bank_upload_structure->
				minimum_bank_date,
			bank_upload_structure->
				fund_name );

	bank_upload_structure->reoccurring_transaction_list =
		bank_upload_fetch_reoccurring_transaction_list(
			application_name );

	bank_upload_set_transaction(
		bank_upload_structure->bank_upload_list,
		bank_upload_structure->reoccurring_transaction_list,
		bank_upload_structure->existing_cash_journal_ledger_list );

	bank_upload_insert_transaction(
		application_name,
		bank_upload_structure->bank_upload_list );

	return bank_upload_structure->table_insert_count;

} /* load_bank_spreadsheet() */

