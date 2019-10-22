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
	char *begin_measurement_date;
	char *end_measurement_date;
	char delimited_record[ 1024 ];
	char delimiter;
	MEASUREMENT_STRUCTURE *m;
	MEASUREMENT_FREQUENCY *measurement_frequency = {0};
	MEASUREMENT_FREQUENCY_STATION_DATATYPE *
		measurement_frequency_station_datatype;
	boolean bypass_reject;
	boolean execute;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	int row_number = 0;

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
"Usage: %s delimiter bypass_data_collection_frequency_reject_yn begin_measurement_date end_measurement_date execute_yn\n", 
			argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	bypass_reject = (*argv[ 2 ] == 'y');
	begin_measurement_date = argv[ 3 ];
	end_measurement_date = argv[ 4 ];
	execute = (*argv[ 5 ] == 'y');

	m = measurement_structure_new( application_name );

	if ( !bypass_reject )
	{
		measurement_frequency = measurement_frequency_new();
	}

	if ( !execute )
	{
		m->html_table_pipe = measurement_open_html_table_pipe();
	}

	if ( execute )
	{
		m->insert_pipe = 
			measurement_open_insert_pipe(
				m->application_name );
	}

	sprintf( sys_string,
		 "measurement_adjust_time_to_sequence delimiter='%c'",
		 delimiter );

	input_pipe = popen( sys_string, "r" );

	while( timlib_get_line( delimited_record, input_pipe, 1024 ) )
	{
		row_number++;

		if ( !measurement_set_delimited_record(
			m, 
			delimited_record,
			delimiter ) )
		{
			fprintf( stderr,
				 "Invalid data in row %d: %s\n",
				 row_number,
				 delimited_record );
			continue;
		}

		if ( measurement_frequency )
		{
			measurement_frequency_station_datatype =
			measurement_frequency_get_or_set_station_datatype(
					measurement_frequency->
						frequency_station_datatype_list,
					application_name,
					m->measurement->station_name,
					m->measurement->datatype,
					begin_measurement_date,
					end_measurement_date );

			if ( dictionary_length(
				measurement_frequency_station_datatype->
					date_time_frequency_dictionary )
			&&   measurement_data_collection_frequency_reject(
				measurement_frequency_station_datatype->
					date_time_frequency_dictionary,
				m->measurement->measurement_date,
				m->measurement->measurement_time ) )
			{
				fprintf( stderr,
			"Violates DATA_COLLECTION_FREQUENCY in row %d: %s\n",
				 	row_number,
				 	delimited_record );
				continue;
			}
		}

		if ( execute )
		{
			measurement_insert( m );
		}
		else
		{
			measurement_html_display( m, m->html_table_pipe );
		}
	}

	pclose( input_pipe );

	if ( m->insert_pipe ) pclose( m->insert_pipe );
	if ( m->html_table_pipe ) pclose( m->html_table_pipe );

	return 0;

} /* main() */

