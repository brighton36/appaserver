/* $APPASERVER_HOME/src_hydrology/measurement_frequency_reject.c*/
/* -------------------------------------------------------------*/
/* Freely available software: see Appaserver.org		*/
/* -------------------------------------------------------------*/

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
			"Usage: %s begin_date end_date delimiter\n", 
			argv[ 0 ] );
		exit( 1 );
	}

	begin_measurement_date = argv[ 1 ];
	end_measurement_date = argv[ 2 ];
	delimiter = *argv[ 3 ];

	m = measurement_structure_new( application_name );

	if ( ! ( measurement_frequency = measurement_frequency_new() ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: measurement_frequency_new() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	while( timlib_get_line( delimited_record, stdin, 1024 ) )
	{
printf( "%s\n", delimited_record ); continue;

		if ( !measurement_set_delimited_record(
			m, 
			delimited_record,
			delimiter ) )
		{
			fprintf( stderr,
				 "Invalid data: %s\n",
				 delimited_record );
			continue;
		}

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
				 "Violates DATA_COLLECTION_FREQUENCY: %s\n",
			 	 delimited_record );
			continue;
		}

		printf( "%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			measurement_display_delimiter(
				m->measurement,
				delimiter ) );
	}

	return 0;

} /* main() */

