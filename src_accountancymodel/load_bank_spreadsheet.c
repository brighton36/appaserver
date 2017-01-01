/* --------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_accountancymodel/load_bank_spreadsheet.c	*/
/* --------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
#include "date_convert.h"
#include "application.h"
#include "application_constants.h"

/* Constants */
/* --------- */
#define INSERT_BANK_DOWNLOAD	\
	"bank_date,bank_description,sequence_number,bank_amount,bank_running_balance,fund"

/* Prototypes */
/* ---------- */
int get_sequence_number(
				char *application_name,
				char *input_filename );

int get_line_count(		char *input_filename );

boolean get_bank_date_international(
				char *bank_date_international,
				char *bank_date );

int load_bank_spreadsheet(
				char *application_name,
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

	if ( strcmp( fund_name, "fund" ) == 0 ) fund_name = "";

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
		printf( "<p>Process complete with %d transactions.\n",
			load_count );
	else
		printf( "<p>Process did not load %d transactions.\n",
			load_count );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
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
	int load_count = 0;
	int sequence_number = 0;
	boolean found_header = 0;
	char error_filename[ 128 ] = {0};

	if ( ! ( sequence_number =
			get_sequence_number(
				application_name,
				input_filename ) ) )
	{
		printf( "<h2>ERROR: cannot get sequence number</h2>\n" );
		return 0;
	}

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		printf( "<h2>ERROR: cannot open %s for read.</h2>\n",
			input_filename );
		return 0;
	}

	if ( execute )
	{
		table_name =
			get_table_name(	application_name,
					"bank_upload" );

		sprintf(	error_filename,
				"/tmp/%s_%d.err",
				table_name,
				getpid() );

		sprintf( sys_string,
		 "insert_statement table=%s field=%s del='%c' 		  |"
		 "sql.e 2>&1						  |"
		 "cat > %s						   ",
		 	table_name,
		 	INSERT_BANK_DOWNLOAD,
		 	FOLDER_DATA_DELIMITER,
			error_filename );

/*
		sprintf( sys_string,
		 "insert_statement table=%s field=%s del='%c' 		  |"
		 "sql.e 2>&1						  |"
		 "html_paragraph_wrapper.e				   ",
		 	table_name,
		 	INSERT_BANK_DOWNLOAD,
		 	FOLDER_DATA_DELIMITER );
*/

		bank_upload_insert_pipe = popen( sys_string, "w" );
	}
	else
	{
		sprintf( sys_string,
		"queue_top_bottom_lines.e 50				|"
		"html_table.e '^Insert into Bank Download' %s '%c'	 ",
			 INSERT_BANK_DOWNLOAD,
			 FOLDER_DATA_DELIMITER);

		table_output_pipe = popen( sys_string, "w" );
	}

	while( timlib_get_line( input_string, input_file, 4096 ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !piece_quote_comma(
				bank_date,
				input_string,
				0 ) )
		{
			continue;
		}

		if ( !found_header )
		{
			if ( strcmp( bank_date, "Date" ) == 0 )
			{
				found_header = 1;
			}
			continue;
		}

		if ( !piece_quote_comma(
				bank_description,
				input_string,
				1 ) )
		{
			continue;
		}

		if ( *fund_name )
		{
			if ( timlib_strcmp(
				bank_description,
				"interest earned" ) == 0
			||   timlib_strcmp(
				bank_description,
				"deposit" ) == 0 )
			{
				sprintf(
				bank_description + strlen( bank_description ),
			 	" %s",
			 	fund_name );
			}
		}

/*
		if ( timlib_strncmp(
			bank_description,
			"beginning balance as of" ) == 0 )
		{
			continue;
		}
*/
	
		if ( !piece_quote_comma(
				bank_amount,
				input_string,
				2 ) )
		{
			continue;
		}

		if ( !piece_quote_comma(
				bank_running_balance,
				input_string,
				3 ) )
		{
			continue;
		}

		if ( !get_bank_date_international(
				bank_date_international,
				bank_date ) )
		{
			continue;
		}

		if ( table_output_pipe )
		{
			fprintf(table_output_pipe,
			 	"%s^%s^%d^%s^%s^%s\n",
			 	bank_date_international,
			 	bank_description,
				sequence_number++,
			 	bank_amount,
				bank_running_balance,
				fund_name );
		}
		else
		{
			fprintf(bank_upload_insert_pipe,
			 	"%s^%s^%d^%s^%s^%s\n",
			 	bank_date_international,
			 	bank_description,
				sequence_number++,
			 	bank_amount,
				bank_running_balance,
				fund_name );
		}

		load_count++;
	}

	fclose( input_file );

	if ( execute )
	{
		pclose( bank_upload_insert_pipe );
		int error_file_lines;

		sprintf( sys_string,
			 "wc -l %s",
			 error_filename );

		error_file_lines = atoi( pipe2string( sys_string ) );
		load_count -= error_file_lines;

		sprintf( sys_string,
			 "rm -f %s",
			 error_filename );

		system( sys_string );
	}
	else
	{
		pclose( table_output_pipe );
	}

	return load_count;

} /* load_bank_spreadsheet() */

boolean get_bank_date_international(
				char *bank_date_international,
				char *bank_date )
{
	date_convert_source_american(
		bank_date_international,
		international,
		bank_date );

	return date_convert_is_valid_international( bank_date_international );

} /* get_bank_date_international() */

int get_sequence_number(
			char *application_name,
			char *input_filename )
{
	int line_count;
	char sys_string[ 1024 ];

	line_count =
		get_line_count(
			input_filename );

	sprintf( sys_string,
		 "reference_number.sh %s %d",
		 application_name,
		 line_count );

	return atoi( pipe2string( sys_string ) );

} /* get_sequence_number() */

int get_line_count( char *input_filename )
{
	char input_string[ 4096 ];
	char bank_date[ 128 ];
	FILE *input_file;
	int line_count = 0;
	boolean found_header = 0;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		printf( "<h2>ERROR: cannot open %s for read</h2>\n",
			input_filename );
		document_close();
		exit( 1 );
	}

	while( timlib_get_line( input_string, input_file, 4096 ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !piece_quote_comma(
				bank_date,
				input_string,
				0 ) )
		{
			continue;
		}

		if ( !found_header )
		{
			if ( strcmp( bank_date, "Date" ) == 0 )
			{
				found_header = 1;
			}
			continue;
		}

		line_count++;
	}

	fclose( input_file );
	return line_count;

} /* get_line_count() */

