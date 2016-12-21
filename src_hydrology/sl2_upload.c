/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/sl2_upload.c	       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "timlib.h"
#include "basename.h"
#include "piece.h"
#include "list.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "process.h"
#include "session.h"
#include "application.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */

/* Sample
--------------------------------------------------------------------------------
STG,2000/08/05,09:00/24.74
 ^   ^           ^    ^
 |   |           |    |
 |   |           |    |
 |   |           |    |
 |   |           |  Value
 |   |         HH:MM
 |  Y/M/D
shef
--------------------------------------------------------------------------------
*/

/* Prototypes */
/* ---------- */
char *get_station(	char *full_filename );

void satlink_upload(	char *full_filename,
			char *shef_bad_file,
			boolean execute,
			char *application_name,
			char *station_name,
			char *argv_0 );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *station_name;
	char *full_filename;
	boolean execute;
	DOCUMENT *document;
	char *argv_0;
	char *shef_bad_file;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char buffer[ 128 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 6 )
	{
		fprintf(stderr,
"Usage: %s application process station filename execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	argv_0 = argv[ 0 ];
	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	station_name = argv[ 3 ];
	full_filename = argv[ 4 ];
	execute = ( *argv[ 5 ] == 'y' );

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !*station_name || strcmp( station_name, "station" ) == 0 )
	{
		station_name = get_station( full_filename );

		if ( !station_name ) station_name = "unknown";
	}

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

	printf( "<h1>%s<br></h1>\n",
		format_initial_capital(
			buffer,
			process_name ) );

	printf( "<h2>\n" );
	fflush( stdout );
	system( "date '+%x %H:%M'" );
	fflush( stdout );
	printf( " Station: %s</h2>\n",
		station_name );
	fflush( stdout );

	if ( !*full_filename || strcmp( full_filename, "filename" ) == 0 )
	{
		printf( "<h3> Please upload a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

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
			getpid(),
			(char *)0 /* session */,
			"dat" );

	shef_bad_file =
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

	satlink_upload(	full_filename, 
			shef_bad_file,
			execute,
			application_name,
			station_name,
			argv_0 );

	if ( execute )
	{
		printf( "<p>Process complete.\n" );

		process_increment_execution_count(
			application_name,
			process_name,
			appaserver_parameter_file_get_dbms() );
	}
	else
	{
		printf( "<p>Process NOT executed.\n" );
	}

	document_close();
	exit( 0 );

} /* main() */

void satlink_upload(	char *full_filename,
			char *shef_bad_file,
			boolean execute,
			char *application_name,
			char *station_name,
			char *argv_0 )
{
	char insert_process[ 512 ];
	char remove_process[ 512 ];
	char display_error_process[ 512 ];
	char *shef_process;
	char sys_string[ 1024 ];

	sprintf(remove_process,
		"/bin/rm -f %s 2>/dev/null",
		shef_bad_file );

	sprintf(display_error_process,
		"echo \"<br><big>Bad records:</big>\"			;"
		"cat %s | queue_top_bottom_lines.e 50			|"
		"html_paragraph_wrapper					;"
		"echo \"<br><big>END Bad records</big>\"		 ",
		shef_bad_file );

	sprintf( insert_process,
"queue_top_bottom_lines.e 50 		|"
"measurement_insert %s shef %c 2>>%s	|"
"cat					 ",
		 application_name,
		 (execute) ? 'y' : 'n',
		 shef_bad_file );

	if ( strcmp( argv_0, "sl2_upload" ) == 0 )
		shef_process = "sl2_shef_to_comma_delimited";
	else
		shef_process = "sl3_shef_to_comma_delimited";

	sprintf( sys_string,
	"cat %s			      	     |"
	"%s %s %s 2>%s		  	     |"
	"%s				     ;"
	"%s				      ",
			 full_filename,
			 shef_process,
			 application_name,
			 station_name,
			 shef_bad_file,
			 insert_process,
			 display_error_process );

	system( sys_string );

	sprintf( sys_string,
		 "rm -f %s",
		 shef_bad_file );

	system( sys_string );

} /* satlink_upload() */

char *get_station( char *full_filename )
{
	char *filename;
	static char station[ 256 ];
	char input_buffer[ 1024 ];
	char piece_buffer[ 1024 ];
	FILE *input_file;

	filename = basename_get_filename( full_filename );

	/* ------------------------------------ */
	/* Sample filename:			*/
	/* DBC1_log_20161216_header.csv		*/
	/* ^					*/
	/* |					*/
	/* Station				*/
	/* ------------------------------------ */
	piece( station, '_', filename, 0 );

	if ( ! ( input_file = fopen( full_filename, "r" ) ) )
		return (char *)0;

	/* -------------------------------------------- */
	/* Sample input:				*/
	/* Station Name, model and version, ...		*/
	/* DBC1, Sutron Satlink 3 Logger V2 Version ...	*/
	/* ^						*/
	/* |						*/
	/* Station					*/
	/* -------------------------------------------- */
	if ( !get_line( input_buffer, input_file ) )
	{
		fclose( input_file );
		return station;
	}

	if ( piece( piece_buffer, ',', input_buffer, 0 ) )
	{
		if ( strcasecmp( piece_buffer, "Station Name" ) != 0 )
		{
			fclose( input_file );
			return station;
		}

		if ( !get_line( input_buffer, input_file ) )
		{
			fclose( input_file );
			return station;
		}

		if ( !piece( station, ',', input_buffer, 0 ) )
		{
			fclose( input_file );
			return station;
		}
	}

	fclose( input_file );
	return station;

} /* get_station() */

