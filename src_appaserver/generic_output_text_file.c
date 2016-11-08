/* ---------------------------------------------------	*/
/* src_appaserver/generic_output_text_file.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "application.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "process_generic_output.h"
#include "appaserver.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */
enum output_medium { text_file, output_medium_stdout, table, spreadsheet };

/* Constants */
/* --------- */
/*
#define OUTPUT_FILE_SPREADSHEET		"%s/%s/%s_%s%s_%d.csv"
#define PREPEND_HTTP_FTP_FILE_SPREADSHEET "%s://%s/%s/%s_%s%s_%d.csv"
#define FTP_FILE_SPREADSHEET		"/%s/%s_%s%s_%d.csv"

#define OUTPUT_FILE_TEXT_FILE		"%s/%s/%s_%s%s_%d.txt"
#define PREPEND_HTTP_FTP_FILE_TEXT_FILE	"%s://%s/%s/%s_%s%s_%d.txt"
#define FTP_FILE_TEXT_FILE		"/%s/%s_%s%s_%d.txt"
*/

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *process_set_name;
	char end_date_suffix[ 128 ];
	char *begin_date = {0};
	char *end_date = {0};
	DOCUMENT *document;
	int process_id = getpid();
	char *process_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *output_medium_string;
	DICTIONARY *post_dictionary;
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	char *sys_string = {0};
	char *email_address = {0};
	char *where_clause = {0};
	char *database_string = {0};
	int length_select_list = 0;
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	enum output_medium output_medium;
	char *units_label;
	char delimiter;
	char *accumulate_yn;
	boolean accumulate;
	int time_piece = -1;
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( argc != 5 )
	{
		fprintf( stderr,
	"Usage: %s application process_set output_medium dictionary\n",
			 argv[ 0 ] );
		fprintf( stderr,
	"Note: output_medium = {text_file,stdout,table,spreadsheet}.\n"
	"      output_medium used to be 'n'o, don't output to stdout or\n"
	"                               'y'es, do output to stdout.\n"
	"      These options are still preserved.\n" );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_set_name = argv[ 2 ];
	output_medium_string = argv[ 3 ];

	/* Note: 'n' used to mean no-output-to-stdout */
	/* ------------------------------------------ */
	if ( strcmp( output_medium_string, "text_file" ) == 0
	||   strcmp( output_medium_string, "n" ) == 0 )
	{
		output_medium = text_file;
	}
	else
	/* ------------------------------------------- */
	/* Note: 'y' used to mean yes-output-to-stdout */
	/* ------------------------------------------- */
	if ( strcmp( output_medium_string, "stdout" ) == 0
	||   strcmp( output_medium_string, "y" ) == 0 )
	{
		output_medium = output_medium_stdout;
	}
	else
	if ( strcmp( output_medium_string, "spreadsheet" ) == 0 )
	{
		output_medium = spreadsheet;
	}
	else
	if ( strcmp( output_medium_string, "table" ) == 0 )
	{
		output_medium = table;
	}
	else
	{
		fprintf(stderr,
"ERROR in %s: output_medium must be either 'stdout','text_file','spreadsheet',or 'table'.\n",
			argv[ 0 ] );
		exit( 1 );
	}

	post_dictionary =
		dictionary_string2dictionary( argv[ 4 ] );

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	QUERY_FROM_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	QUERY_STARTING_LABEL );

	appaserver_parameter_file = appaserver_parameter_file_new();

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

/*
	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );
*/

	accumulate_yn =
		dictionary_fetch_index_zero(
			post_dictionary,
			"accumulate_yn" );

	accumulate = (accumulate_yn && *accumulate_yn == 'y' );

	process_generic_output =
		process_generic_output_new(
			application_name,
			(char *)0 /* process_name */,
			process_set_name,
			accumulate );

	process_generic_output->value_folder->datatype =
		process_generic_datatype_new(
			application_name,
			process_generic_output->
				value_folder->
					foreign_folder->
						foreign_attribute_name_list,
			process_generic_output->
				value_folder->
					datatype_folder->
						datatype_folder_name,
			process_generic_output->
				value_folder->
					datatype_folder->
						primary_attribute_name_list,
			process_generic_output->
				value_folder->
					datatype_folder->
						exists_aggregation_sum,
			process_generic_output->
				value_folder->
					datatype_folder->
						exists_bar_graph,
			process_generic_output->
				value_folder->
					datatype_folder->
						exists_scale_graph_zero,
			process_generic_output->
				value_folder->
					units_folder_name,
			post_dictionary,
			0 /* dictionary_index */ );

	if ( !process_generic_output->value_folder->datatype )
	{
		char buffer[ 1024 ];

		format_initial_capital( buffer, process_set_name );

		document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<h1>%s</h1><h3>ERROR: insufficient input.</h3>\n",
			buffer );
		document_close();
		exit( 0 );
	}

	process_name =
		process_generic_output_get_process_name(
			process_set_name,
			post_dictionary );

	aggregate_level =
		aggregate_level_get_aggregate_level(
			dictionary_get_index_zero(
				post_dictionary,
				"aggregate_level" ) );

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
			dictionary_get_index_zero(
				post_dictionary,
				"aggregate_statistic" ),
			aggregate_level );

	if ( aggregate_statistic == aggregate_statistic_none
	&&   aggregate_level != aggregate_level_none
	&&   aggregate_level != real_time )
	{
		aggregate_statistic =
			process_generic_output_get_database_aggregate_statistic(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point,
				process_generic_output->
					value_folder->
					datatype->
					primary_attribute_data_list );
	}

	if ( output_medium == table
	||   output_medium == text_file
	||   output_medium == output_medium_stdout )
	{
		delimiter = FOLDER_DATA_DELIMITER;
	}
	else
	/* Pipe to double_quote_comma_delimited.e */
	/* -------------------------------------- */
	{
		delimiter = '\0';
	}

	sys_string = process_generic_output_get_text_file_sys_string(
			&begin_date,
			&end_date,
			&where_clause,
			&units_label,
			(int *)0 /* datatype_entity_piece */,
			(int *)0 /* datatype_piece */,
			(int *)0 /* date_piece */,
			&time_piece,
			(int *)0 /* value_piece */,
			&length_select_list,
			application_name,
			process_generic_output,
			post_dictionary,
			delimiter,
			aggregate_level,
			aggregate_statistic,
			0 /* append_low_high */,
			0 /* not concat_datatype_entity */,
			0 /* not concat_datatype */,
			process_generic_output->accumulate );

	if ( end_date && *end_date )
		sprintf( end_date_suffix, "_%s", end_date );
	else
		*end_date_suffix = '\0';

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->document_root,
	 		process_generic_output->
				value_folder->
				value_folder_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			(char *)0 /* extension */ );

	appaserver_link_file->application_name = application_name;
	appaserver_link_file->begin_date_string = begin_date;
	appaserver_link_file->end_date_string = end_date_suffix;

	if ( output_medium == table )
	{
		FILE *input_pipe;
		char input_buffer[ 512 ];
		FILE *output_pipe;
		char justify_column_list_string[ 128 ];
		char title[ 512 ];
		int i;
		int left_column_count;

		document = document_new( "", application_name );
		document_set_output_content_type( document );
	
		document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_get_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	
		document_output_body(
				document->application_name,
				document->onload_control_string );

		fflush( stdout );

		input_pipe = popen( sys_string, "r" );
		get_line( input_buffer, input_pipe );

		/* Headings need to be comma delimited */
		/* ----------------------------------- */
		search_replace_character( input_buffer, '^', ',' );

		if ( aggregate_level == aggregate_level_none
		||   aggregate_level == real_time )
		{
			left_column_count = length_select_list - 1;
		}
		else
		{
			left_column_count = length_select_list - 2;
		}

		for ( i = 0; i < left_column_count; i++ )
		{
			if ( i == 0 )
				strcpy(	justify_column_list_string,
					"left" );
			else
				strcat(	justify_column_list_string,
					",left" );
		}

		strcat( justify_column_list_string, ",right" );

		/* The frequency per period */
		/* ------------------------ */
		if ( aggregate_level != real_time )
		{
			strcat( justify_column_list_string, ",right" );
		}

		sprintf(title,
			"%s: %s",
			process_generic_output->
				value_folder->value_folder_name,
			where_clause );

		format_initial_capital( title, title );

		sprintf(sys_string,
			"queue_top_bottom_lines.e %d		|"
			"html_table.e \"%s\" \"%s\" '^' %s	 ",
			HTML_TABLE_QUEUE_TOP_BOTTOM,
			title,
		        input_buffer,
			justify_column_list_string );	

		output_pipe = popen( sys_string, "w" );

		while( get_line( input_buffer, input_pipe ) )
		{
			if ( process_generic_output->
				value_folder->
				time_attribute_name
			&&   *process_generic_output->
				value_folder->
				time_attribute_name
			&&   aggregate_level != aggregate_level_none
			&&   aggregate_level != real_time )
			{
				piece_inverse( input_buffer, '^', time_piece );
			}

			fprintf( output_pipe, "%s\n", input_buffer );
		}

		pclose( input_pipe );
		pclose( output_pipe );
		document_close();
	}
	else
	if ( output_medium == text_file )
	{
		FILE *input_pipe;
		char input_buffer[ 512 ];
		char *output_filename;
		char *link_prompt;
		FILE *output_pipe;
		boolean first_time = 1;

		appaserver_link_file->extension = "txt";

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

		link_prompt =
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

/*
		sprintf(output_filename,
		 	OUTPUT_FILE_TEXT_FILE,
		 	appaserver_parameter_file->
				appaserver_mount_point,
		 	application_name, 
		 	process_generic_output->
				value_folder->
				value_folder_name,
		 	begin_date,
		 	end_date_suffix,
		 	process_id );

		if ( application_get_prepend_http_protocol_yn(
					application_name ) == 'y' )
		{
			sprintf(ftp_filename, 
			 	PREPEND_HTTP_FTP_FILE_TEXT_FILE,
				application_get_http_prefix( application_name ),
			 	appaserver_library_get_server_address(),
			 	application_name,
			 	process_generic_output->
					value_folder->
					value_folder_name,
			 	begin_date,
			 	end_date_suffix,
			 	process_id );
		}
		else
		{
			sprintf(ftp_filename, 
			 	FTP_FILE_TEXT_FILE,
			 	application_name,
			 	process_generic_output->
					value_folder->
					value_folder_name,
			 	begin_date,
			 	end_date_suffix,
			 	process_id );
		}
*/

		document = document_new( "", application_name );
		document_set_output_content_type( document );
	
		document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_get_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	
		document_output_body(
				document->application_name,
				document->onload_control_string );

		input_pipe = popen( sys_string, "r" );

		/* The frequency per period */
		/* ------------------------ */
		sprintf(sys_string,
			"tr '%c' '%c' > %s",
			FOLDER_DATA_DELIMITER,
			OUTPUT_TEXT_FILE_DELIMITER,
			output_filename );

		output_pipe = popen( sys_string, "w" );

		while( get_line( input_buffer, input_pipe ) )
		{
			if ( first_time )
			{
				search_replace_character(
					input_buffer,
					' ',
					'_' );
				fprintf( output_pipe, "%s\n", input_buffer );
				first_time = 0;
				continue;
			}

			if ( process_generic_output->
				value_folder->
				time_attribute_name
			&&   *process_generic_output->
				value_folder->
				time_attribute_name
			&&   aggregate_level != aggregate_level_none
			&&   aggregate_level != real_time )
			{
				piece_inverse( input_buffer, '^', time_piece );
			}

			fprintf( output_pipe, "%s\n", input_buffer );
		}

		pclose( input_pipe );
		pclose( output_pipe );

		email_address =
			dictionary_get_index_zero(
				post_dictionary,
				"email_address" );

		if ( email_address
		&&   *email_address
		&&   strcmp( email_address, "email_address" ) != 0 ) 
		{
			char sys_string[ 1024 ];
	
			search_replace_string(	where_clause,
						"1 = 1 and ",
						"" );

			sprintf( sys_string,
				 "cat %s				|"
				 "mailx -s \"%s %s\" %s	 		 ",
				 output_filename,
				 where_clause,
				 process_generic_output->
					value_folder->value_folder_name,
				 email_address );
			system( sys_string );

			printf( "<h1>%s Transmission<br></h1>\n",
				process_generic_output->
					value_folder->value_folder_name );
			printf( "<BR><p>Search criteria: %s<hr>\n",
				where_clause );

			printf( "<BR><p>Sent to %s<hr>\n", email_address );
			printf( "<h2>\n" );
			fflush( stdout );
			system( "date '+%x %H:%M'" );
			fflush( stdout );
			printf( "</h2>\n" );
		}
		else
		{
			char buffer[ 1024 ];

			printf( "<h1>%s Transmission<br></h1>\n",
				format_initial_capital(
					buffer,
					process_generic_output->
					value_folder->value_folder_name ) );
			printf( "<h2>\n" );
			fflush( stdout );
			system( "date '+%x %H:%M'" );
			fflush( stdout );
			printf( "</h2>\n" );
		
			printf( "<BR><p>Search criteria: %s<hr>\n",
				search_replace_string(	where_clause,
							"1 = 1 and ",
							"" ) );

			appaserver_library_output_ftp_prompt(
					link_prompt, 
					TRANSMIT_PROMPT,
					(char *)0 /* target */,
					(char *)0 /* application_type */ );
		}
		document_close();
	}
	else
	if ( output_medium == spreadsheet )
	{
		FILE *input_pipe;
		char *output_filename;
		char *link_prompt;
		char input_buffer[ 512 ];
		FILE *output_pipe;
		boolean first_time = 1;

		appaserver_link_file->extension = "csv";

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

		link_prompt =
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

/*
		sprintf(output_filename,
		 	OUTPUT_FILE_SPREADSHEET,
		 	appaserver_parameter_file->
				appaserver_mount_point,
		 	application_name, 
		 	process_generic_output->
				value_folder->
				value_folder_name,
		 	begin_date,
		 	end_date_suffix,
		 	process_id );

		if ( application_get_prepend_http_protocol_yn(
					application_name ) == 'y' )
		{
			sprintf(ftp_filename, 
			 	PREPEND_HTTP_FTP_FILE_SPREADSHEET,
				application_get_http_prefix( application_name ),
			 	appaserver_library_get_server_address(),
			 	application_name,
			 	process_generic_output->
					value_folder->
					value_folder_name,
			 	begin_date,
			 	end_date_suffix,
			 	process_id );
		}
		else
		{
			sprintf(ftp_filename, 
			 	FTP_FILE_SPREADSHEET,
			 	application_name,
			 	process_generic_output->
					value_folder->
					value_folder_name,
			 	begin_date,
			 	end_date_suffix,
			 	process_id );
		}
*/

		input_pipe = popen( sys_string, "r" );

		sprintf(sys_string,
			"cat > %s",
			output_filename );

		output_pipe = popen( sys_string, "w" );

		while( get_line( input_buffer, input_pipe ) )
		{
			search_replace_character(
				input_buffer,
				'^',
				',' );

			if ( first_time )
			{
				fprintf( output_pipe, "%s\n", input_buffer );
				first_time = 0;
				continue;
			}

			if ( process_generic_output->
				value_folder->
				time_attribute_name
			&&   *process_generic_output->
				value_folder->
				time_attribute_name
			&&   aggregate_level != aggregate_level_none
			&&   aggregate_level != real_time )
			{
				piece_inverse( input_buffer, ',', time_piece );
			}

			fprintf( output_pipe, "%s\n", input_buffer );
		}

		pclose( input_pipe );
		pclose( output_pipe );

		document = document_new( "", application_name );
		document_set_output_content_type( document );
	
		document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_get_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	
		document_output_body(
				document->application_name,
				document->onload_control_string );

		email_address =
			dictionary_get_index_zero(
				post_dictionary,
				"email_address" );

		if ( email_address
		&&   *email_address
		&&   strcmp( email_address, "email_address" ) != 0 ) 
		{
			char sys_string[ 1024 ];
	
			search_replace_string(	where_clause,
						"1 = 1 and ",
						"" );

			sprintf( sys_string,
				 "cat %s				|"
				 "mailx -s \"%s %s\" %s	 		 ",
				 output_filename,
				 where_clause,
				 process_generic_output->
					value_folder->value_folder_name,
				 email_address );
			system( sys_string );

			printf( "<h1>%s Transmission<br></h1>\n",
				process_generic_output->
					value_folder->value_folder_name );
			printf( "<BR><p>Search criteria: %s<hr>\n",
				where_clause );

			printf( "<BR><p>Sent to %s<hr>\n", email_address );
			printf( "<h2>\n" );
			fflush( stdout );
			system( "date '+%x %H:%M'" );
			fflush( stdout );
			printf( "</h2>\n" );
		}
		else
		{
			char buffer[ 1024 ];

			printf( "<h1>%s Transmission<br></h1>\n",
				format_initial_capital(
					buffer,
					process_generic_output->
					value_folder->value_folder_name ) );
			printf( "<h2>\n" );
			fflush( stdout );
			system( "date '+%x %H:%M'" );
			fflush( stdout );
			printf( "</h2>\n" );
		
			printf( "<BR><p>Search criteria: %s<hr>\n",
				search_replace_string(	where_clause,
							"1 = 1 and ",
							"" ) );

			appaserver_library_output_ftp_prompt(
					link_prompt, 
					TRANSMIT_PROMPT,
					(char *)0 /* target */,
					(char *)0 /* application_type */ );
		}
		document_close();
	}
	else
	if ( output_medium == output_medium_stdout )
	{
		FILE *input_pipe;
		char input_buffer[ 512 ];
		FILE *output_pipe;
		boolean first_time = 1;

		input_pipe = popen( sys_string, "r" );

		/* The frequency per period */
		/* ------------------------ */
/*
		int right_justify_count = 1;
		if ( aggregate_level != real_time
		&&   aggregate_level != aggregate_level_none )
		{
			right_justify_count++;
		}

		sprintf(sys_string,
			"delimiter2padded_columns.e '^' %d",
			right_justify_count );
*/

		sprintf(sys_string,
			"tr '^' '%c'",
			OUTPUT_TEXT_FILE_DELIMITER );

		output_pipe = popen( sys_string, "w" );

		while( get_line( input_buffer, input_pipe ) )
		{
			if ( first_time )
			{
				search_replace_character(
					input_buffer,
					' ',
					'_' );
				fprintf( output_pipe, "#" );
				first_time = 0;
			}

			fprintf( output_pipe, "%s\n", input_buffer );
		}

		pclose( input_pipe );
		pclose( output_pipe );
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

