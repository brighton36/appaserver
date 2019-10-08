/* $APPASERVER_HOME/src_hydrology/measurement_insert.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "measurement.h"
#include "appaserver_library.h"
#include "timlib.h"
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "datatype.h"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *load_process;
	char comma_delimited_record[ 1024 ];
	boolean execute;
	MEASUREMENT_STRUCTURE *m;
	int not_loaded_count = 0;
	MEASUREMENT_FREQUENCY *measurement_frequency;
	MEASUREMENT_FREQUENCY_STATION_DATATYPE *
		measurement_frequency_station_datatype;
	char *begin_measurement_date = {0};
	FILE *input_pipe;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
"Usage: echo \"station,datatype,date,time,value\" | %s ignored cr10|shef|realdata execute_yn\n", 
			argv[ 0 ] );
		exit( 1 );
	}

	/* application_name = argv[ 1 ]; */
	load_process = argv[ 2 ];
	execute = (*argv[ 3 ] == 'y');

	m = measurement_structure_new( application_name );
	measurement_frequency = measurement_frequency_new();

	if ( !execute )
	{
		m->html_table_pipe = measurement_open_html_table_pipe();
	}

	if ( execute )
	{
		m->insert_pipe = 
			measurement_open_insert_pipe(
				m->application_name,
				0 /* delete_measurements_day */ );
	}

	input_pipe = popen( "sort", "r" );

	while( get_line( comma_delimited_record, input_pipe ) )
	{
		if ( !measurement_set_comma_delimited_record(
			m, 
			comma_delimited_record,
			argv[ 0 ] ) )
		{
			continue;
		}

		if ( !begin_measurement_date )
		{
			begin_measurement_date =
				strdup(
					m->
					measurement->
					measurement_date );
		}

		measurement_frequency_station_datatype =
			measurement_frequency_get_or_set_station_datatype(
					measurement_frequency->
						frequency_station_datatype_list,
					m->measurement->station_name,
					m->measurement->datatype );

		if ( dictionary_length(
				measurement_frequency_station_datatype->
					date_time_frequency_dictionary )
		&&   !measurement_date_time_frequency_exists(
				measurement_frequency_station_datatype->
					date_time_frequency_dictionary,
				m->measurement->measurement_date,
				m->measurement->measurement_time ) )
		{
			fprintf( stderr,
				 "INVALID_FREQUENCY: %s\n",
				 comma_delimited_record );
			continue;
		}

		if ( strcmp( load_process, "cr10" ) == 0
		&&   !execute )
		{
			not_loaded_count++;
		}

		measurement_insert(
			m,
			execute,
			m->html_table_pipe );
	}

	pclose( input_pipe );

	if ( m->insert_pipe ) pclose( m->insert_pipe );

	if ( m->html_table_pipe ) pclose( m->html_table_pipe );

	if ( not_loaded_count )
		printf( "<p>Not loaded count = %d\n", not_loaded_count );

	return 0;

} /* main() */

