/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/shef_upload.c	       	*/
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
#include "appaserver_error.h"
#include "hydrology.h"

/* Constants */
/* --------- */

/* Sample
--------------------------------------------------------------------------------
.E/3273/STG/20000805/0900/DIH1/  6.90/  6.90/  6.90/  6.90/(M)/(M)/(M)/(M)/
.E/3273/STG/20000805/1700/DIH1/  6.90/  6.90/  6.90/  6.90/  6.90/  6.89/  6.89/
.E/3273/STG/20000806/0000/DIH1/  6.89/  6.89/(M)/(M)/(M)/(M)/(M)/(M)/(M)/  6.88/
.E/3273/STG/20000806/1000/DIH1/  6.88/  6.88/  6.98/  6.98/  6.98/  6.98/  6.98/
.E/3273/STG/20000806/1700/DIH1/  6.98/  7.02/  7.02/  7.02/  7.02/  7.02/  7.02/
.E/3273/STG/20000807/0000/DIH1/  7.02/  7.02/(M)/(M)/(M)/(M)/  7.01/  7.01/
.E/3273/STG/20000807/0800/DIH1/  7.01/
.E/ANGELS/HEAD/20000805/0900/DIH1/  6.84/  6.84/  6.84/  6.87/  6.87/  6.87/(M)/
.E/ANGELS/HEAD/20000805/1600/DIH1/(M)/  6.85/  6.85/  6.85/  6.84/  6.84/  6.84/
.E/ANGELS/HEAD/20000805/2300/DIH1/  6.84/  6.84/  6.82/  6.82/  6.82/  6.82/
#  ^      ^    ^   ^ ^  ^
#  |      |    |   | |  |
#  |      |    |   | |  hhmm
#  |      |    |   | day
#  |      |    |   month
#  |      |    year
#  |      shef code
#  station
--------------------------------------------------------------------------------
*/

/* Prototypes */
/* ---------- */
void output_bad_records(		char *station_bad_file,
				 	char *shef_bad_file,
					char *insert_bad_file );


void shef_upload(	char *station_bad_file, 
			char *shef_bad_file,
			char *insert_bad_file,
			char *application_name,
			char *shef_filename,
			boolean execute );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *shef_filename;
	boolean execute;
	DOCUMENT *document;
	char station_bad_file[ 128 ];
	char shef_bad_file[ 128 ];
	char insert_bad_file[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *session;
	char *dir;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
		"Usage: %s process_name session shef_filename execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	session = argv[ 2 ];
	shef_filename = argv[ 3 ];
	execute = (*argv[ 4 ] == 'y');

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

	dir = appaserver_parameter_file->appaserver_data_directory;

	sprintf( station_bad_file, "%s/station_bad_%s.dat", dir, session );
	sprintf( shef_bad_file, "%s/shef_bad_%s.dat", dir, session );
	sprintf( insert_bad_file, "%s/insert_bad_%s.dat", dir, session );

	shef_upload(	station_bad_file, 
			shef_bad_file,
			insert_bad_file,
			application_name,
			shef_filename,
			execute );

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
		printf( "<p>Process not executed.\n" );
	}

	document_close();
	return 0;

} /* main() */

void shef_upload(	char *station_bad_file, 
			char *shef_bad_file,
			char *insert_bad_file,
			char *application_name,
			char *shef_filename,
			boolean execute )

{
	char insert_process[ 512 ];
	char sys_string[ 4096 ];
	char *begin_measurement_date = {0};
	char *end_measurement_date = {0};

	hydrology_parse_begin_end_dates(
		&begin_measurement_date,
		&end_measurement_date,
		shef_filename,
		"timestamp" /* date_heading_label */,
		0 /* date_piece */ );

	if ( !begin_measurement_date || !end_measurement_date )
	{
		printf(
			"<h3>ERROR: Cannot extract begin/end dates.</h3>\n" );
		document_close();
		return;
	}

	sprintf( insert_process,
"measurement_insert begin=%s end=%s execute=%c",
		 begin_measurement_date,
		 end_measurement_date,
		 (execute) ? 'y' : 'n' );

	sprintf( sys_string,
	"cat %s					      	     	     |"
	"station_alias_to_station %s 1 \'/\' 2>%s 		     |"
	"shef_to_comma_delimited %s 2>%s		  	     |"
	"%s 2>%s				      		      ",
			 shef_filename,
			 application_name,
			 station_bad_file,
			 application_name,
			 shef_bad_file,
			 insert_process,
			 insert_bad_file );

	if ( system( sys_string ) ) {};

} /* shef_upload() */

void output_bad_records(		char *station_bad_file,
				 	char *shef_bad_file,
					char *insert_bad_file )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	"cat %s %s %s | html_table.e '^^Bad Records' '' ''",
	 	station_bad_file,
	 	shef_bad_file,
		insert_bad_file );

	if ( system( sys_string ) ){};

} /* output_bad_records() */

