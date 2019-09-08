/* --------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/feeder_upload_missing.c		*/
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
#include "list.h"
#include "environ.h"
#include "date.h"
#include "process.h"
#include "application.h"
#include "application_constants.h"
#include "bank_upload.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
LIST *feeder_upload_get_possible_description_list(
				char *bank_description_embedded,
				char *bank_description,
				double bank_amount );

char *bank_upload_get_bank_description_original(
				char *bank_description,
				double bank_amount );

void feeder_upload_missing_pipe(
				FILE *output_pipe,
				char *bank_date,
				char *bank_description_embedded,
				char *bank_description,
				double bank_amount );

void feeder_upload_missing_execute(
				LIST *bank_upload_list );

void feeder_upload_missing(	
				char *application_name,
				char *fund_name,
				char *feeder_account,
				char *input_filename,
				boolean reverse_order,
				int date_piece_offset,
				int description_piece_offset,
				int debit_piece_offset,
				int credit_piece_offset,
				int balance_piece_offset );

/* Global variables */
/* ---------------- */
enum bank_upload_exception bank_upload_exception = {0};

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *fund_name;
	char *feeder_account;
	char *input_filename;
	char *date_column_string;
	char *description_column_string;
	char *debit_column_string;
	char *credit_column_string;
	char *balance_column_string;
	int date_piece_offset;
	int description_piece_offset;
	int debit_piece_offset;
	int credit_piece_offset;
	int balance_piece_offset;
	boolean reverse_order;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char buffer[ 128 ];

	/* Exits if not found. */
	/* ------------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	if ( argc != 11 )
	{
		fprintf( stderr,
"Usage: %s process fund feeder_account filename date_column description_column debit_column credit_column balance_column reverse_order_yn\n",
			 argv[ 0 ] );

		fprintf( stderr,
"\nNotes: Column numbers are one based. Delimiters are either comma only, or quote-comma.\n" );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	fund_name = argv[ 2 ];
	feeder_account = argv[ 3 ];
	input_filename = argv[ 4 ];
	date_column_string = argv[ 5 ];
	description_column_string = argv[ 6 ];
	debit_column_string = argv[ 7 ];
	credit_column_string = argv[ 8 ];
	balance_column_string = argv[ 9 ];
	reverse_order = (*argv[ 10 ] == 'y' );

	if ( *date_column_string
	&&   strcmp( date_column_string, "date_column" ) != 0 )
	{
		date_piece_offset = atoi( date_column_string ) - 1;
	}
	else
	{
		date_piece_offset = 0;
	}

	if ( *description_column_string
	&&   strcmp( description_column_string, "description_column" ) != 0 )
	{
		description_piece_offset =
			atoi( description_column_string ) - 1;
	}
	else
	{
		description_piece_offset = 1;
	}

	if ( *debit_column_string
	&&   strcmp( debit_column_string, "debit_column" ) != 0 )
	{
		debit_piece_offset = atoi( debit_column_string ) - 1;
	}
	else
	{
		debit_piece_offset = 2;
	}

	if ( *credit_column_string
	&&   strcmp( credit_column_string, "credit_column" ) != 0 )
	{
		credit_piece_offset = atoi( credit_column_string ) - 1;
	}
	else
	{
		/* The same as debit_piece_offset */
		/* ------------------------------ */
		credit_piece_offset = 2;
	}

	if ( *balance_column_string
	&&   strcmp( balance_column_string, "balance_column" ) != 0 )
	{
		balance_piece_offset = atoi( balance_column_string ) - 1;
	}
	else
	{
		balance_piece_offset = 3;
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

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
	if ( system( "date '+%x %H:%M'" ) ) {};
	printf( "</h1>\n" );
	fflush( stdout );

	if ( !*input_filename || strcmp( input_filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !*feeder_account
	||   strcmp( feeder_account, "feeder_account" ) == 0 )
	{
		printf( "<h3>Please choose a feeder account.</h3>\n" );
		document_close();
		exit( 0 );
	}

	feeder_upload_missing(
				application_name,
				fund_name,
				feeder_account,
				input_filename,
				reverse_order,
				date_piece_offset,
				description_piece_offset,
				debit_piece_offset,
				credit_piece_offset,
				balance_piece_offset );

	process_increment_execution_count(
		application_name,
		process_name,
		appaserver_parameter_file_get_dbms() );

	document_close();

	exit( 0 );

} /* main() */

void feeder_upload_missing(
			char *application_name,
			char *fund_name,
			char *feeder_account,
			char *input_filename,
			boolean reverse_order,
			int date_piece_offset,
			int description_piece_offset,
			int debit_piece_offset,
			int credit_piece_offset,
			int balance_piece_offset )
{
	BANK_UPLOAD_STRUCTURE *bank_upload_structure;

	bank_upload_structure =
		bank_upload_structure_new(
			application_name,
			fund_name,
			feeder_account,
			input_filename,
			reverse_order,
			date_piece_offset,
			description_piece_offset,
			debit_piece_offset,
			credit_piece_offset,
			balance_piece_offset );

	if ( !bank_upload_structure )
	{
		char *msg;

		msg = "<h3>Internal ERROR.</h3>";

		printf( "%s\n", msg );
	
		return;
	}

	feeder_upload_missing_execute(
		bank_upload_structure->
			file.
			bank_upload_file_list );

} /* feeder_upload_missing() */

void feeder_upload_missing_execute(
			LIST *bank_upload_list )
{
	BANK_UPLOAD *bank_upload;
	char sys_string[ 1024 ];
	char *heading;
	FILE *output_pipe;

	if ( !list_rewind( bank_upload_list ) ) return;

	heading = "bank_date,bank_description,amount";

	sprintf( sys_string,
		 "html_table.e '' %s '^' left,left,right",
		 heading );

	output_pipe = popen( sys_string, "w" );

	do {
		bank_upload = list_get( bank_upload_list );

		feeder_upload_missing_pipe(
			output_pipe,
			bank_upload->bank_date,
			bank_upload->bank_description_embedded,
			bank_upload->bank_description,
			bank_upload->bank_amount );

	} while( list_next( bank_upload_list ) );

	pclose( output_pipe );

} /* feeder_upload_missing_execute() */

LIST *feeder_upload_get_possible_description_list(
			char *bank_description_embedded,
			char *bank_description,
			double bank_amount )
{
	char *bank_description_original;
	char buffer[ 512 ];
	LIST *possible_description_list = list_new();

	list_append_pointer(
		possible_description_list,
		strdup( bank_description_embedded ) );

	if ( timlib_strncmp( bank_description, "check " ) == 0 )
	{
		sprintf( buffer,
			 "%s%c",
			 bank_description,
			 '%' );

		list_append_pointer(
			possible_description_list,
			strdup( buffer ) );
	}

	bank_description_original =
		bank_upload_get_bank_description_original(
			bank_description,
			bank_amount );

	list_append_pointer(
		possible_description_list,
		strdup( bank_description_original ) );

	list_append_pointer(
		possible_description_list,
		strdup( bank_description ) );

	return possible_description_list;

} /* feeder_upload_get_possible_description_list() */

void feeder_upload_missing_pipe(
			FILE *output_pipe,
			char *bank_date,
			char *bank_description_embedded,
			char *bank_description,
			double bank_amount )
{
	char sys_string[ 1024 ];
	char where[ 1024 ];
	char *bank_description_original;

	sprintf( sys_string,
		 "echo \"select count(*)			 "
		 "from bank_upload				 "
		 "where %s;\"					|"
		 "sql.e						 ",
		 bank_upload_get_where(
			where,
		 	bank_date,
		 	bank_description_embedded ) );

	if ( atoi( pipe2string( sys_string ) ) ) return;

	if ( timlib_strncmp( bank_description, "check " ) == 0 )
	{
		sprintf( where,
			 "bank_date = '%s' and bank_description like '%s%c'",
			 bank_date,
			 bank_description,
			 '%' );

		sprintf( sys_string,
	 		"echo \"select count(*)			 "
	 		"from bank_upload			 "
	 		"where %s;\"				|"
	 		"sql.e					 ",
			where );

		if ( atoi( pipe2string( sys_string ) ) ) return;
	}

	bank_description_original =
		bank_upload_get_bank_description_original(
			bank_description,
			bank_amount );

	sprintf( sys_string,
	 	"echo \"select count(*)			 	 "
	 	"from bank_upload				 "
	 	"where %s;\"					|"
	 	"sql.e						 ",
	 	bank_upload_get_where(
			where,
	 		bank_date,
	 		bank_description_original ) );

	if ( atoi( pipe2string( sys_string ) ) ) return;

	sprintf( sys_string,
 		"echo \"select count(*)			 "
 		"from bank_upload			 "
 		"where %s;\"				|"
 		"sql.e					 ",
 		bank_upload_get_where(
			where,
 			bank_date,
 			bank_description ) );

	if ( atoi( pipe2string( sys_string ) ) ) return;

	fprintf(output_pipe,
		"%s^%s^%.2lf\n",
		bank_date,
		bank_description,
		bank_amount );

} /* feeder_upload_missing_pipe() */

char *bank_upload_get_bank_description_original(
				char *bank_description,
				double bank_amount )
{
	static char bank_description_original[ 1024 ];
	char bank_portion[ 512 ];

	*bank_portion = '\0';

	sprintf( bank_portion, "%.2lf", bank_amount );

	if ( *( bank_portion + strlen( bank_portion ) - 1 ) == '0' )
	{
		*( bank_portion + strlen( bank_portion ) - 1 ) = '\0';
	}

	if ( *( bank_portion + strlen( bank_portion ) - 1 ) == '0' )
	{
		*( bank_portion + strlen( bank_portion ) - 1 ) = '\0';
	}

	if ( *( bank_portion + strlen( bank_portion ) - 1 ) == '.' )
	{
		*( bank_portion + strlen( bank_portion ) - 1 ) = '\0';
	}

	sprintf( bank_description_original,
		 "%s %s",
		 bank_description,
		 bank_portion );

	return bank_description_original;

} /* bank_upload_get_bank_description_original() */

