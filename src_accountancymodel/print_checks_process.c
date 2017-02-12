/* ------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_accountancymodel/print_checks_process.c	*/
/* ------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "date.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "latex.h"
#include "print_checks.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define PROMPT				"Press here to view check."

/* Prototypes */
/* ---------- */
double print_checks_get_balance(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address );

void print_checks_post(
				char *application_name,
				LIST *full_name_list,
				LIST *street_address_list,
				int starting_check_number,
				double check_amount,
				char *fund_name );

char *print_checks_create(
				char *application_name,
				LIST *full_name_list,
				LIST *street_address_list,
				char *memo,
				double check_amount,
				char *document_root_directory,
				char *process_name,
				char *session,
				char *fund_name );

char *print_checks(		char *application_name,
				char *full_name_list_string,
				char *street_address_list_string,
				int starting_check_number,
				char *memo,
				double check_amount,
				boolean execute,
				char *document_root_directory,
				char *process_name,
				char *session,
				char *fund_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *session;
	char *fund_name;
	char *full_name_list_string;
	char *street_address_list_string;
	int starting_check_number;
	char *memo;
	double check_amount;
	boolean execute;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DOCUMENT *document;
	char title[ 128 ];
	char *database_string = {0};
	char *pdf_filename;

	if ( argc != 11 )
	{
		fprintf( stderr,
"Usage: %s application process session fund full_name[,full_name] street_address[,street_address] starting_check_number memo check_amount execute_yn\n",
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

	process_name = argv[ 2 ];
	session = argv[ 3 ];
	fund_name = argv[ 4 ];
	full_name_list_string = argv[ 5 ];
	street_address_list_string = argv[ 6 ];
	starting_check_number = atoi( argv[ 7 ] );
	memo = argv[ 8 ];
	check_amount = atof( argv[ 9 ] );
	execute = ( *argv[ 10 ] == 'y' );

	appaserver_parameter_file = appaserver_parameter_file_new();

	format_initial_capital( title, process_name ),

	document = document_new( title, application_name );
	document->output_content_type = 1;

	document_output_head_stream(
			stdout,
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */,
			1 /* with close_head */ );

	printf( "<h1>%s</h1>\n", title );
	fflush( stdout );

	pdf_filename =
		print_checks(	application_name,
				full_name_list_string,
				street_address_list_string,
				starting_check_number,
				memo,
				check_amount,
				execute,
				appaserver_parameter_file->
					document_root,
				process_name,
				session,
				fund_name );

	if ( !pdf_filename )
	{
		printf(
		"<h3>Error: these checks have already been posted.</h3>\n" );
	}

	document_close();

	return 0;

} /* main() */

char *print_checks(	char *application_name,
			char *full_name_list_string,
			char *street_address_list_string,
			int starting_check_number,
			char *memo,
			double check_amount,
			boolean execute,
			char *document_root_directory,
			char *process_name,
			char *session,
			char *fund_name )
{
	LIST *full_name_list;
	LIST *street_address_list;
	char *pdf_filename;

	full_name_list =
		list_string_to_list(
			full_name_list_string,
			',' );

	street_address_list =
		list_string_to_list(
			street_address_list_string,
			',' );

	if ( ( !list_length( full_name_list ) )
	||   ( list_length( full_name_list ) == 1
		&& 	strcmp(	list_get_first_pointer(
					full_name_list ),
				"full_name" ) == 0 ) )
	{
		printf( "<h3>Please choose an Entity</h3>\n" );
		return "";
	}

	if ( list_length( full_name_list ) !=
	     list_length( street_address_list ) )
	{
		printf(
"<h3>An internal error occurred. The list lengths don't match: %d vs. %d.</h3>\n",
		list_length( full_name_list ),
		list_length( street_address_list ) );
		return "";
	}

/*
	pdf_filename =
		print_checks_create(
			application_name,
			full_name_list,
			street_address_list,
			memo,
			check_amount,
			document_root_directory,
			process_name,
			session,
			fund_name );
*/

	if ( execute )
	{
		print_checks_post(
			application_name,
			full_name_list,
			street_address_list,
			starting_check_number,
			check_amount,
			fund_name );
	}

	return pdf_filename;

} /* print_checks() */

char *print_checks_create(
			char *application_name,
			LIST *full_name_list,
			LIST *street_address_list,
			char *memo,
			double check_amount,
			char *document_root_directory,
			char *process_name,
			char *session,
			char *fund_name )
{
	char *full_name;
	char *street_address;
	double balance;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *output_filename;
	char *pdf_filename;
	char *document_root_filename;
	char *ftp_filename;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name /* filename_stem */,
			application_name,
			getpid(),
			(char *)0 /* session */,
			"dat" );

	/* make_checks.e places the pdf_filename into output_filename. */
	/* ----------------------------------------------------------- */
	output_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

/*
	sprintf( sys_string,
		 "make_checks.e stdin 2>/dev/null > %s",
		 output_filename );
*/
	sprintf( sys_string,
		 "make_checks.e stdin > %s",
		 output_filename );

	output_pipe = popen( sys_string, "w" );

	list_rewind( full_name_list );
	list_rewind( street_address_list );

	do {
		full_name = list_get_pointer( full_name_list );
		street_address = list_get_pointer( street_address_list );

		/* If no balance, then user pressed <Submit> twice. */
		/* ------------------------------------------------ */
		if ( ! ( balance =
				print_checks_get_balance(
					application_name,
					fund_name,
					full_name,
					street_address ) ) )
		{
			pclose( output_pipe );
			return (char *)0;
		}

		fprintf( output_pipe,
			 "%s^%.2lf^%s\n",
			 full_name,
			 (check_amount) ? check_amount : balance,
			 (*memo) ? memo : "" );

		list_next( street_address_list );

	} while( list_next( full_name_list ) );

	pclose( output_pipe );

	sprintf( sys_string,
		 "cat %s",
		 output_filename );

	pdf_filename = pipe2string( sys_string );

	appaserver_link_file->extension = "pdf";
	appaserver_link_file->session = session;
	appaserver_link_file->process_id = 0;

	/* Copy pdf_filename to document_root_directory. */
	/* --------------------------------------------- */
	document_root_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	sprintf(	sys_string,
			"cp %s %s",
			pdf_filename,
			document_root_filename );

	system( sys_string );

	ftp_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
			PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	return pdf_filename;

} /* print_checks_create() */

void print_checks_post(
			char *application_name,
			LIST *full_name_list,
			LIST *street_address_list,
			int starting_check_number,
			double check_amount,
			char *fund_name )
{
	PRINT_CHECKS *print_checks;

	print_checks =
		print_checks_new(
			application_name,
			fund_name,
			full_name_list,
			street_address_list,
			starting_check_number,
			check_amount /* dialog_box_check_amount */ );

	print_checks_insert_entity_check_amount_list(
		application_name,
		fund_name,
		print_checks->entity_check_amount_list,
		print_checks->dialog_box_check_amount );

	printf(
	"<h3>Execute Posting to Journal Ledger complete.</h3>\n" );

} /* print_checks_post() */

double print_checks_get_balance(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address )
{
	char sys_string[ 128 ];
	char *results;
	char input_full_name[ 128 ];
	char input_street_address_balance[ 128 ];
	char input_street_address[ 128 ];
	char input_balance[ 16 ];
	static LIST *entity_list = {0};

	if ( !entity_list )
	{
		sprintf( sys_string,
		 	"populate_print_checks_entity.sh %s '%s'",
		 	application_name,
			fund_name );

		entity_list = pipe2list( sys_string );
	}

	if ( !list_rewind( entity_list ) ) return 0.0;

	do {
		results = list_get_pointer( entity_list );

		if ( character_count(
			FOLDER_DATA_DELIMITER,
			results ) != 1 )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: not one delimiter in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 results );

			return 0.0;
		}

		piece(	input_full_name,
			FOLDER_DATA_DELIMITER,
			results,
			0 );

		piece(	input_street_address_balance,
			FOLDER_DATA_DELIMITER,
			results,
			1 );

		piece(	input_street_address,
			'[',
			input_street_address_balance,
			0 );

		piece( input_balance, '[', input_street_address_balance, 1 );

		if ( strcmp( input_full_name, full_name ) == 0
		&&   strcmp( input_street_address, street_address ) == 0 )
		{
			return atof( input_balance );
		}

	} while( list_next( entity_list ) );

	return 0.0;

} /* print_checks_get_balance() */

