/* ------------------------------------------		*/
/* $APPASERVER_HOME/src_hydrology/LT_upload.c		*/
/* ------------------------------------------		*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

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
#include "basename.h"
#include "hydrology.h"
#include "application_constants.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void output_bad_records(
			 	char *bad_parse_file,
			 	char *bad_insert_file );

void LT_upload(		
				char *filename,
				char *station,
				boolean change_existing_data,
				boolean execute,
				char *appaserver_data_directory );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *station;
	boolean change_existing_data;
	boolean execute;
	char *filename;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char buffer[ 128 ];

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s process_name station filename change_existing_data_yn execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	station = argv[ 2 ];
	filename = argv[ 3 ];
	change_existing_data = (*argv[ 4 ] == 'y');
	execute = (*argv[ 5 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	printf( "<h2>%s\n", format_initial_capital( buffer, process_name ) );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !*filename || strcmp( filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	LT_upload(
		filename,
		station,
		change_existing_data,
		execute,
		appaserver_parameter_file->
			appaserver_data_directory );

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
		printf( "<p>Process did not execute.\n" );
	}

	document_close();

	return 0;

} /* main() */

void LT_upload(		char *filename,
			char *station,
			boolean change_existing_data,
			boolean execute,
			char *appaserver_data_directory )
{
	char sys_string[ 1024 ];
	char *begin_measurement_date = {0};
	char *end_measurement_date = {0};
	char bad_parse[ 128 ];
	char bad_insert[ 128 ];
	char *date_heading_label;
	pid_t pid;
	char *dir;

	date_heading_label = "datetime";
	pid = getpid();
	dir = appaserver_data_directory;

	hydrology_parse_begin_end_dates(
		&begin_measurement_date,
		&end_measurement_date,
		filename,
		date_heading_label,
		0 /* date_piece */ );

	if ( !begin_measurement_date || !*begin_measurement_date )
	{
		printf( "<h3>Could not extract the begin/end dates.</h3>\n" );
		document_close();
		exit( 0 );
	}

	sprintf( bad_parse, "%s/parse_%d.dat", dir, pid );
	sprintf( bad_insert, "%s/insert_%d.dat", dir, pid );

	sprintf( sys_string,
"spreadsheet_parse file=\"%s\" station=\"%s\" time=no 2>%s		|"
"measurement_insert begin=%s end=%s replace=%c execute=%c 2>%s		|"
"cat									 ",
		 filename,
		 station,
		 bad_parse,
		 begin_measurement_date,
		 end_measurement_date,
		 (change_existing_data) ? 'y' : 'n',
		 (execute) ? 'y' : 'n',
		 bad_insert );

	if ( system( sys_string ) ) {};

	output_bad_records(
		bad_parse,
		bad_insert );

} /* LT_upload() */

void output_bad_records(
		 	char *bad_parse_file,
		 	char *bad_insert_file )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	"cat %s %s | html_table.e '^^Bad Records' '' ''",
	 	bad_parse_file,
	 	bad_insert_file );

	if ( system( sys_string ) ){};

} /* output_bad_records() */

