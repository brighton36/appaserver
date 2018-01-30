/* ---------------------------------------------------	*/
/* src_hydrology/load_sfwmd_breakpoint_data.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

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
#include "list.h"
#include "environ.h"
#include "date.h"
#include "boolean.h"
#include "process.h"
#include "dictionary.h"
#include "session.h"
#include "column.h"
#include "application.h"
#include "basename.h"
#include "date_convert.h"
#include "sfwmd_station_datatype.h"
#include "appaserver_link_file.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
/*
#define ERROR_FILE_TEMPLATE		"%s/%s/load_sfwmd_breakpoint_bad_%d.txt"
#define ERROR_FTP_TEMPLATE		"%s://%s/%s/load_sfwmd_breakpoint_bad_%d.txt"
*/
#define PROMPT	"&lt;Left Click&gt; to view bad records or &lt;Right Click&gt; to save."

/* Prototypes */
/* ---------- */
SFWMD_STATION_DATATYPE *get_sfwmd_station_datatype(
				HASH_TABLE *sfwmd_station_datatype_hash_table,
				char *input_buffer );

char *to_yyyy_mm_dd( char *d, char *s );

char *to_hhmm( char *d, char *s );

int load_sfwmd_breakpoint_data(
					char *error_filespecification,
					char *application_name,
					char *input_filespecification,
					char change_existing_data_yn,
					char really_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char change_existing_data_yn;
	char really_yn;
	char *input_filespecification;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *error_filespecification;
	char *error_ftp_filename;
	int process_count;
	char *process_name;
	int process_id = getpid();
	char *database_string = {0};
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process filename change_existing_data_yn really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	input_filespecification = argv[ 3 ];
	change_existing_data_yn = *argv[ 4 ];
	really_yn = *argv[ 5 ];

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

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->
				document_root,
			process_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"txt" );

/*
	sprintf( error_filespecification,
		 ERROR_FILE_TEMPLATE,
		 appaserver_parameter_file->
		 	appaserver_mount_point,
		 application_name,
		 process_id );
	
	sprintf( error_ftp_filename, 
		 ERROR_FTP_TEMPLATE, 
		 application_get_http_prefix( application_name ),
		 appaserver_library_get_server_address(),
		 application_name,
		 process_id );
*/

	error_filespecification =
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

	error_ftp_filename =
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

	process_count = load_sfwmd_breakpoint_data(
			error_filespecification,
			application_name,
			input_filespecification,
			change_existing_data_yn,
			really_yn );

	if ( really_yn == 'y' )
		printf( "<p>Processed %d records\n", process_count );
	else
		printf( "<p>Did not process %d records\n", process_count );

	if ( timlib_file_populated( error_filespecification ) )
	{
		printf( "<br>" );
		appaserver_library_output_ftp_prompt(
				error_ftp_filename, 
				PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

	document_close();

	exit( 0 );
} /* main() */

char *to_hhmm( char *d, char *s )
{
	char time_only[ 128 ];

	if ( !column( time_only, 1, s ) ) return (char *)0;

	if ( character_count( ':', time_only ) == 2 )
	{
		piece_inverse( time_only, ':', 2 );
	}

	timlib_delete_character( time_only, ':' );

	sprintf( d, 
		 "%04d",
		 atoi( time_only ) );

	return d;

} /* to_hhmm() */

char *to_yyyy_mm_dd( char *d, char *s )
{
	char date_only[ 128 ];
	DATE_CONVERT *date;
/*
	char yyyy[ 128 ], mon[ 128 ], dd[ 128 ];
*/

	column( date_only, 0, s );

	date = date_convert_new_date_convert( 
				international,
				date_only );

	strcpy( d, date->return_date );
	date_convert_free( date );

/*
	if ( ! piece( mon, '/', date_only, 0 ) ) return (char *)0;
	if ( ! piece( dd, '/', date_only, 1 ) ) return (char *)0;
	if ( ! piece( yyyy, '/', date_only, 2 ) ) return (char *)0;

	sprintf( d, 
		 "%s-%02d-%02d",
		 yyyy,
		 atoi( mon ),
		 atoi( dd ) );
*/

	return d;

} /* to_yyyy_mm_dd() */

int load_sfwmd_breakpoint_data(
			char *error_filespecification,
			char *application_name,
			char *input_filespecification,
			char change_existing_data_yn,
			char really_yn )
{
	HASH_TABLE *sfwmd_station_datatype_hash_table;
	FILE *input_pipe;
	char input_sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	SFWMD_STATION_DATATYPE *sfwmd_station_datatype;
	FILE *error_file;
	int process_count = 0;
	FILE *insert_pipe = {0};
	FILE *update_pipe = {0};

	sfwmd_station_datatype_hash_table =
		load_sfwmd_station_datatype_hash_table(
			application_name );

	error_file = fopen( error_filespecification, "w" );
	if ( !error_file )
	{
		printf( "<p>ERROR: cannot open %s for write.\n",
			error_filespecification );
		document_close();
		exit( 1 );
	}

	if ( really_yn == 'y' )
	{
		char *field_list_string;
		char *measurement_table_name;
		char sys_string[ 1024 ];

		measurement_table_name =
			get_table_name(
				application_name,
				"measurement" );

		field_list_string =
"station,datatype,measurement_date,measurement_time,measurement_value,last_validation_date,last_validation_process,measurement_update_method";

		sprintf( sys_string,
			 "insert_statement.e %s %s '^'			|"
			 "sql.e 2>&1					|"
			 "html_paragraph_wrapper.e		 	 ",
			 measurement_table_name,
			 field_list_string );

		insert_pipe = popen( sys_string, "w" );

		if ( change_existing_data_yn == 'y' )
		{
			field_list_string =
			"station,datatype,measurement_date,measurement_time";

			sprintf( sys_string,
			 	"update_statement.e %s %s '^^'		|"
			 	"sql.e 2>&1				|"
				"html_paragraph_wrapper.e		 ",
			 	measurement_table_name,
			 	field_list_string );

			update_pipe = popen( sys_string, "w" );
		}
	}

	sprintf( input_sys_string,
		 "cat %s",
		 input_filespecification );

	input_pipe = popen( input_sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( ! ( sfwmd_station_datatype =
				get_sfwmd_station_datatype(
					sfwmd_station_datatype_hash_table,
					input_buffer ) ) )
		{
			fprintf( error_file, "%s\n", input_buffer );
			continue;
		}

		if ( !*sfwmd_station_datatype->station )
		{
			fprintf( error_file, "%s\n", input_buffer );
			continue;
		}

		if ( !*sfwmd_station_datatype->datatype )
		{
			fprintf( error_file, "%s\n", input_buffer );
			continue;
		}

		process_count++;

		if ( really_yn == 'y' )
		{
			if ( sfwmd_station_datatype->is_null )
			{
		 		fprintf(
				 insert_pipe,
				 "%s^%s^%s^%s^^%s^%s^%s\n",
				 sfwmd_station_datatype->station,
				 sfwmd_station_datatype->datatype,
				 sfwmd_station_datatype->
					measurement_date_string,
				 sfwmd_station_datatype->
					measurement_time_string,
				 sfwmd_station_datatype->
					last_validation_date_string,
				 sfwmd_station_datatype->db_key,
				 sfwmd_station_datatype->
				 	measurement_update_method );

				if ( update_pipe )
				{
		 			fprintf(
				 	update_pipe,
				 	"%s^%s^%s^%s^measurement_value^\n",
				 	sfwmd_station_datatype->station,
				 	sfwmd_station_datatype->datatype,
				 	sfwmd_station_datatype->
						measurement_date_string,
				 	sfwmd_station_datatype->
						measurement_time_string );
				}
			}
			else
			{
		 		fprintf(
				 insert_pipe,
				 "%s^%s^%s^%s^%lf^%s^%s^%s\n",
				 sfwmd_station_datatype->station,
				 sfwmd_station_datatype->datatype,
				 sfwmd_station_datatype->
					measurement_date_string,
				 sfwmd_station_datatype->
					measurement_time_string,
				 sfwmd_station_datatype->value,
				 sfwmd_station_datatype->
					last_validation_date_string,
				 sfwmd_station_datatype->db_key,
				 sfwmd_station_datatype->
				 	measurement_update_method );

				if ( update_pipe )
				{
		 			fprintf(
				 	update_pipe,
				 	"%s^%s^%s^%s^measurement_value^%lf\n",
				 	sfwmd_station_datatype->station,
				 	sfwmd_station_datatype->datatype,
				 	sfwmd_station_datatype->
						measurement_date_string,
				 	sfwmd_station_datatype->
						measurement_time_string,
				 	sfwmd_station_datatype->value );
				}
			}

			if ( sfwmd_station_datatype->db_key
			&&   *sfwmd_station_datatype->db_key )
			{
				if ( update_pipe )
				{
		 			fprintf( update_pipe,
				 "%s^%s^%s^%s^last_validation_process^%s\n",
				 	sfwmd_station_datatype->station,
				 	sfwmd_station_datatype->datatype,
				 	sfwmd_station_datatype->
						measurement_date_string,
				 	sfwmd_station_datatype->
						measurement_time_string,
				 	sfwmd_station_datatype->db_key );
				}
			}

			if ( *sfwmd_station_datatype->
				last_validation_date_string )
			{
				if ( update_pipe )
				{
		 			fprintf( update_pipe,
				 "%s^%s^%s^%s^last_validation_date^%s\n",
				 	sfwmd_station_datatype->station,
				 	sfwmd_station_datatype->datatype,
				 	sfwmd_station_datatype->
						measurement_date_string,
				 	sfwmd_station_datatype->
						measurement_time_string,
				 	sfwmd_station_datatype->
						last_validation_date_string );
				}
			}

			if ( update_pipe )
			{
		 		fprintf( update_pipe,
				 "%s^%s^%s^%s^measurement_update_method^%s\n",
				 	sfwmd_station_datatype->station,
				 	sfwmd_station_datatype->datatype,
				 	sfwmd_station_datatype->
						measurement_date_string,
				 	sfwmd_station_datatype->
						measurement_time_string,
				 	sfwmd_station_datatype->
				 		measurement_update_method );
			}
		}
	}

	if ( really_yn == 'y' )
	{
		pclose( insert_pipe );
		if ( update_pipe ) pclose( update_pipe );
	}

	pclose( input_pipe );
	fclose( error_file );

	return process_count;

} /* load_sfwmd_breakpoint_data() */

SFWMD_STATION_DATATYPE *get_sfwmd_station_datatype(
				HASH_TABLE *sfwmd_station_datatype_hash_table,
				char *input_buffer )
{
	char process_buffer[ 1024 ];
	char piece_buffer[ 1024 ];
	SFWMD_STATION_DATATYPE *sfwmd_station_datatype;
	static char measurement_date_string[ 16 ];
	static char measurement_time_string[ 16 ];
	char *measurement_date_pointer;
	char *measurement_time_pointer;

	strcpy( process_buffer, input_buffer );
	search_replace_string( process_buffer, "\"", "" );

	/* piece db_key */
	/* ------------ */
	if ( !piece( piece_buffer, ',', process_buffer, 2 ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	if ( ! ( sfwmd_station_datatype =
		hash_table_fetch(
			sfwmd_station_datatype_hash_table,
			piece_buffer ) ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	/* piece measurement_date */
	/* ---------------------- */
	if ( !piece( piece_buffer, ',', process_buffer, 0 ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	if ( ! ( measurement_date_pointer =
			to_yyyy_mm_dd(
				measurement_date_string,
				piece_buffer ) ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	if ( ! ( measurement_time_pointer =
			to_hhmm(
				measurement_time_string,
				piece_buffer ) ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	strcpy(	sfwmd_station_datatype->measurement_date_string,
		measurement_date_pointer );

	strcpy(	sfwmd_station_datatype->measurement_time_string,
		measurement_time_pointer );

	/* piece measurement_value */
	/* ----------------------- */
	if ( !piece( piece_buffer, ',', process_buffer, 3 ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	if ( !*piece_buffer )
	{
		sfwmd_station_datatype->is_null = 1;
	}
	else
	{
		sfwmd_station_datatype->is_null = 0;
		sfwmd_station_datatype->value = atof( piece_buffer );
	}

	if ( sfwmd_station_datatype->conversion_factor )
	{
		sfwmd_station_datatype->value *=
			sfwmd_station_datatype->conversion_factor;
	}

	strcpy(	sfwmd_station_datatype->last_validation_date_string,
		sfwmd_station_datatype->measurement_date_string );

	return sfwmd_station_datatype;

} /* get_sfwmd_station_datatype() */

