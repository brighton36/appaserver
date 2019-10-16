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
#include "date_convert.h"
#include "application.h"
#include "basename.h"
#include "hydrology.h"
#include "load_turkey_point.h"

/* Prototypes */
/* ---------- */
int load_turkey_point_file(	char *application_name,
				char *station_name,
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
	int load_count;
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

	printf( "<h2>%s\n", format_initial_capital( buffer, process_name ) );
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

	load_count =
		load_turkey_point_file(
			application_name,
			station_name,
			input_filename,
			execute );

	if ( execute )
	{
		printf( "<p>Process complete with %d measurements.\n",
			load_count );

		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}
	else
	{
		printf( "<p>Process did not load %d measurements.\n",
			load_count );
	}

	document_close();

	return 0;

} /* main() */

int load_turkey_point_file(	char *application_name,
				char *station_name,
				char *input_filename,
				boolean execute )
{
	int load_count = 0;
	HYDROLOGY *hydrology;
	STATION *station;
	MEASUREMENT_FREQUENCY *measurement_frequency;

	hydrology = hydrology_new();

	station = station_get_or_set_station(
			hydrology->input.station_list,
			application_name,
			station_name );

	hydrology->output.header_column_datatype_list =
		/* --------------------------- */
		/* Sets datatype->column_piece */
		/* --------------------------- */
		hydrology_get_header_column_datatype_list(
				station->station_datatype_list,
				station->station_name,
				input_filename,
				TURKEY_POINT_FIRST_COLUMN_PIECE );

	measurement_frequency = measurement_frequency_new();

	/* Sets station_datatype->measurement_list */
	/* --------------------------------------- */
	hydrology_parse_file(
		station->station_datatype_list,
		measurement_frequency->frequency_station_datatype_list,
		stderr,
		input_filename,
		TURKEY_POINT_DATE_TIME_PIECE );

	if ( execute )
	{
		load_count =
			hydrology_measurement_insert(
				station->station_name,
				station->station_datatype_list );
	}
	else
	{
		hydrology_summary_table_display(
				station->station_name,
				station->station_datatype_list );

		load_count =
			hydrology_measurement_table_display(
				station->station_name,
				station->station_datatype_list );
	}

	return load_count;

} /* load_turkey_point_file() */

