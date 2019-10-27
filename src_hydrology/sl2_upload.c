/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/sl2_upload.c	       	*/
/* --------------------------------------------------- 	*/
/* Note: links to sl3_upload				*/
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
#include "appaserver_error.h"

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
void output_bad_records(char *bad_insert_file );

char *get_station(	char *full_filename );

void satlink_upload(	char *full_filename,
			char *shef_bad_file,
			boolean execute,
			char *station_name,
			char *argv_0,
			boolean nohtml );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *station_name;
	char *full_filename;
	boolean execute;
	DOCUMENT *document;
	char *argv_0;
	char shef_bad_file[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char buffer[ 1024 ];
	boolean nohtml;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 6 )
	{
		fprintf(stderr,
"Usage: %s ignored process station filename execute_yn|nohtml\n",
			argv[ 0 ] );
		exit( 1 );
	}

	argv_0 = argv[ 0 ];
	/* application_name = argv[ 1 ]; */
	process_name = argv[ 2 ];
	station_name = argv[ 3 ];
	full_filename = argv[ 4 ];

	execute = ( *argv[ 5 ] == 'y' );

	nohtml = (strcmp( argv[ 5 ], "nohtml" ) == 0);

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !*station_name || strcmp( station_name, "station" ) == 0 )
	{
		station_name = get_station( full_filename );

		if ( !station_name ) station_name = "unknown";
	}

	if ( !nohtml )
	{
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
	
		printf( "<h1>%s<br></h1>\n",
			format_initial_capital(
				buffer,
				process_name ) );
	
		printf( "<h2>\n" );
		fflush( stdout );
		if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
		fflush( stdout );
		printf( " Station: %s</h2>\n",
			station_name );
		fflush( stdout );

	} /* if !nohtml */

	if ( !*full_filename || strcmp( full_filename, "filename" ) == 0 )
	{
		printf( "<h3> Please upload a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	sprintf( shef_bad_file,
		 "%s/shef_bad_%d.dat",
		 appaserver_parameter_file->
		 	appaserver_data_directory,
		 getpid() );

	satlink_upload(	full_filename, 
			shef_bad_file,
			execute,
			station_name,
			argv_0,
			nohtml );

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

	if ( !nohtml ) document_close();

	return 0;

} /* main() */

void satlink_upload(	char *full_filename,
			char *shef_bad_file,
			boolean execute,
			char *station_name,
			char *argv_0,
			boolean nohtml )
{
	char insert_process[ 512 ];
	char *shef_process;
	char sys_string[ 1024 ];


	if ( nohtml )
	{
		strcpy( insert_process, "cat" );
	}
	else
	{
		sprintf(insert_process,
"measurement_insert station=%s begin=begin end=end bypass=yes execute=%c",
			station_name,
		 	(execute) ? 'y' : 'n' );
	}

	if ( strcmp( argv_0, "sl2_upload" ) == 0 )
		shef_process = "sl2_shef_to_comma_delimited";
	else
		shef_process = "sl3_shef_to_comma_delimited";

	sprintf( sys_string,
	"cat %s			      	     	|"
	"%s					|"
	"%s 2>%s		  	      	 ",
			 full_filename,
			 shef_process,
			 insert_process,
			 shef_bad_file );

	if ( system( sys_string ) ) {};

	output_bad_records( shef_bad_file );

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

void output_bad_records( char *bad_insert_file )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	"cat %s | html_table.e '^^Bad Records' '' ''",
	 	bad_insert_file );

	if ( system( sys_string ) ){};

} /* output_bad_records() */

