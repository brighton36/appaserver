/* -----------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/load_turkey_point.c		*/
/* -----------------------------------------------------	*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ---------------------------------------------		*/

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
#include "application.h"
#include "basename.h"
#include "hydrology.h"
#include "load_turkey_point.h"

/* Prototypes */
/* ---------- */
void output_bad_records(
		 		char *bad_parse_file,
		 		char *bad_time_file,
		 		char *bad_frequency_file,
		 		char *bad_insert_file );

void load_turkey_point_file(	char *appaserver_data_directory,
				char *station,
				char *input_filename,
				boolean execute );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *station_name;
	boolean execute;
	char *input_filename;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char buffer[ 128 ];

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 5 )
	{
		fprintf( stderr, 
			 "Usage: %s process station filename execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	station_name = argv[ 2 ];
	input_filename = argv[ 3 ];
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

	printf( "<h1>%s</h1>\n",
		format_initial_capital( buffer, process_name ) );
	printf( "<h2>" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !*input_filename || strcmp( input_filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	load_turkey_point_file(
		appaserver_parameter_file->
			appaserver_data_directory,
		station_name,
		input_filename,
		execute );

	if ( execute )
	{
		printf( "<h3>Insert complete.</h3>\n" );

		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}
	else
	{
		printf( "<h3>Insert not executed.\n" );
	}

	document_close();

	return 0;

} /* main() */

void load_turkey_point_file(	char *appaserver_data_directory,
				char *station_name,
				char *input_filename,
				boolean execute )
{
	char sys_string[ 1024 ];
	char *begin_measurement_date = {0};
	char *end_measurement_date = {0};
	char bad_parse[ 128 ];
	char bad_time[ 128 ];
	char bad_frequency[ 128 ];
	char bad_insert[ 128 ];
	char *date_heading_label;
	pid_t pid;
	char *dir;

	date_heading_label = "date";
	pid = getpid();
	dir = appaserver_data_directory;

	hydrology_parse_begin_end_dates(
					&begin_measurement_date,
					&end_measurement_date,
					input_filename,
					date_heading_label,
					1 /* date_piece */ );

	sprintf( bad_parse, "%s/parse_%d.dat", dir, pid );
	sprintf( bad_time, "%s/time_%d.dat", dir, pid );
	sprintf( bad_frequency, "%s/frequency_%d.dat", dir, pid );
	sprintf( bad_insert, "%s/insert_%d.dat", dir, pid );

	sprintf( sys_string,
"spreadsheet_parse file=\"%s\" station=\"%s\" time=no date_piece=1 2>%s	|"
"measurement_adjust_time_to_sequence 2>%s				|"
"measurement_frequency_reject %s %s '^' 2>%s				|"
"measurement_insert bypass=y begin=%s end=%s execute=%c 2>%s		|"
"cat									 ",
		 input_filename,
		 station_name,
		 bad_parse,
		 bad_time,
		 begin_measurement_date,
		 end_measurement_date,
		 bad_frequency,
		 begin_measurement_date,
		 end_measurement_date,
		 (execute) ? 'y' : 'n',
		 bad_insert );

	if ( system( sys_string ) ) {};

	output_bad_records(
		 bad_parse,
		 bad_time,
		 bad_frequency,
		 bad_insert );

} /* load_turkey_point_file() */

void output_bad_records(
		 	char *bad_parse_file,
		 	char *bad_time_file,
		 	char *bad_frequency_file,
		 	char *bad_insert_file )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	"cat %s %s %s %s | html_table.e '^^Bad Records' '' ''",
	 	bad_parse_file,
	 	bad_time_file,
	 	bad_frequency_file,
	 	bad_insert_file );

	if ( system( sys_string ) ){};

} /* output_bad_records() */

