/* ----------------------------------------------------	*/
/* $APPASERVER_HOME/src_accountancymodel/print_checks.c	*/
/* ----------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

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
#include "appaserver_link_file.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void insert_transaction_set_purchase_order(
				char *application_name,
				char *full_name,
				char *street_address,
				double balance,
				int check_number );

double print_checks_get_balance(
				char *application_name,
				char *full_name,
				char *street_address );

char *print_checks_execute(
				char *application_name,
				LIST *full_name_list,
				LIST *street_address_list,
				int starting_check_number,
				char *memo,
				double check_amount,
				boolean post,
				char *document_root_directory,
				char *process_name );

char *print_checks(		char *application_name,
				char *full_name_list_string,
				char *street_address_list_string,
				int starting_check_number,
				char *memo,
				double check_amount,
				boolean post,
				char *document_root_directory,
				char *process_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *full_name_list_string;
	char *street_address_list_string;
	int starting_check_number;
	char *memo;
	double check_amount;
	boolean post;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DOCUMENT *document;
	char title[ 128 ];
	char *database_string = {0};
	char *pdf_filename;

	if ( argc != 9 )
	{
		fprintf( stderr,
"Usage: %s application process full_name[,full_name] street_address[,street_address] starting_check_number memo check_amount post_yn\n",
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
	full_name_list_string = argv[ 3 ];
	street_address_list_string = argv[ 4 ];
	starting_check_number = atoi( argv[ 5 ] );
	memo = argv[ 6 ];
	check_amount = atof( argv[ 7 ] );
	post = ( *argv[ 8 ] == 'y' );

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
				post,
				appaserver_parameter_file->
					document_root,
				process_name );

	printf( "<p>pdf_filename = %s\n", pdf_filename );

	document_close();

	return 0;

} /* main() */

char *print_checks(	char *application_name,
			char *full_name_list_string,
			char *street_address_list_string,
			int starting_check_number,
			char *memo,
			double check_amount,
			boolean post,
			char *document_root_directory,
			char *process_name )
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

	if ( !list_length( full_name_list ) )
	{
		printf( "<h3>Please choose a Journal Ledger</h3>\n" );
		return "";
	}

	if ( list_length( full_name_list ) !=
	     list_length( street_address_list ) )
	{
		printf(
		"<h3>An internal error occurred. Check punctuation.</h3>\n" );
		return "";
	}

	pdf_filename =
		print_checks_execute(
			application_name,
			full_name_list,
			street_address_list,
			starting_check_number,
			memo,
			check_amount,
			post,
			document_root_directory,
			process_name );

	return pdf_filename;

} /* print_checks() */

char *print_checks_execute(
			char *application_name,
			LIST *full_name_list,
			LIST *street_address_list,
			int starting_check_number,
			char *memo,
			double check_amount,
			boolean post,
			char *document_root_directory,
			char *process_name )
{
	char *full_name;
	char *street_address;
	double balance;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *output_filename;
	char *pdf_filename;
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

	/* Sends pdf_filename to output_filename */
	/* ------------------------------------- */
	sprintf( sys_string,
		 "make_checks.e stdin 2>/dev/null > %s",
		 output_filename );

	output_pipe = popen( sys_string, "w" );

	list_rewind( full_name_list );
	list_rewind( street_address_list );

	do {
		full_name = list_get_pointer( full_name_list );
		street_address = list_get_pointer( street_address_list );

		if ( ! ( balance =
				print_checks_get_balance(
					application_name,
					full_name,
					street_address ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get balance for (%s/%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 full_name,
				 street_address );
			return "";
		}

		fprintf( output_pipe,
			 "%s^%.2lf^%s\n",
			 full_name,
			 (check_amount) ? check_amount : balance,
			 (*memo) ? memo : "" );

		if ( post )
		{
			insert_transaction_set_purchase_order(
				application_name,
				full_name,
				street_address,
				balance,
				starting_check_number++ );
		}

		list_next( street_address_list );

	} while( list_next( full_name_list ) );

	pclose( output_pipe );

	sprintf( sys_string,
		 "cat %s",
		 output_filename );

	pdf_filename = pipe2string( sys_string );

	return pdf_filename;

} /* print_checks_execute() */

double print_checks_get_balance(
				char *application_name,
				char *full_name,
				char *street_address )
{
	char sys_string[ 128 ];
	char *results;
	char input_full_name[ 128 ];
	char input_street_address_balance[ 32 ];
	char input_street_address[ 128 ];
	char input_balance[ 16 ];
	static LIST *entity_list = {0};

	if ( !entity_list )
	{
		sprintf( sys_string,
		 	"populate_print_checks_entity.sh %s",
		 	application_name );

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

void insert_transaction_set_purchase_order(
				char *application_name,
				char *full_name,
				char *street_address,
				double balance,
				int check_number )
{

} /* insert_transaction_set_purchase_order() */

