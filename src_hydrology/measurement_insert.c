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
#include "name_arg.h"

/* Prototypes */
/* ---------- */
void setup_arg(		NAME_ARG *arg, int argc, char **argv );

void fetch_parameters(	char **begin_date,
			char **end_date,
			char **bypass_reject_yn,
			char **delimiter,
			char **execute_yn,
			NAME_ARG *arg );

int main( int argc, char **argv )
{
	char *application_name;
	char *begin_measurement_date;
	char *end_measurement_date;
	char delimited_record[ 1024 ];
	char *delimiter;
	char *bypass_reject_yn;
	char *execute_yn;
	MEASUREMENT_STRUCTURE *m;
	MEASUREMENT_FREQUENCY *measurement_frequency = {0};
	MEASUREMENT_FREQUENCY_STATION_DATATYPE *
		measurement_frequency_station_datatype;
	boolean bypass_reject;
	boolean execute;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	int row_number = 0;
	NAME_ARG *arg;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	arg = name_arg_new( argv[ 0 ] );

	setup_arg( arg, argc, argv );

	fetch_parameters(
		&begin_measurement_date,
		&end_measurement_date,
		&bypass_reject_yn,
		&delimiter,
		&execute_yn,
		arg );

	bypass_reject = ( *bypass_reject_yn == 'y' );
	execute = ( *execute_yn == 'y' );

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
		 *delimiter );

	input_pipe = popen( sys_string, "r" );

	while( timlib_get_line( delimited_record, input_pipe, 1024 ) )
	{
		row_number++;

		if ( !measurement_set_delimited_record(
			m, 
			delimited_record,
			*delimiter ) )
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

void fetch_parameters(	char **begin_date,
			char **end_date,
			char **bypass_reject_yn,
			char **delimiter,
			char **execute_yn,
			NAME_ARG *arg )
{
	*begin_date = fetch_arg( arg, "begin_date" );
	*end_date = fetch_arg( arg, "end_date" );
	*bypass_reject_yn = fetch_arg( arg, "bypass_reject_yn" );
	*delimiter = fetch_arg( arg, "delimiter" );
	*execute_yn = fetch_arg( arg, "execute_yn" );

} /* fetch_parameters() */

void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

        ticket = add_valid_option( arg, "begin_date" );
        ticket = add_valid_option( arg, "end_date" );

        ticket = add_valid_option( arg, "bypass_reject_yn" );
        add_valid_value( arg, ticket, "yes" );
        add_valid_value( arg, ticket, "no" );
        set_default_value( arg, ticket, "no" );

        ticket = add_valid_option( arg, "delimiter" );
        set_default_value( arg, ticket, "^" );

        ticket = add_valid_option( arg, "execute_yn" );
        add_valid_value( arg, ticket, "yes" );
        add_valid_value( arg, ticket, "no" );
        set_default_value( arg, ticket, "no" );

        ins_all( arg, argc, argv );

} /* setup_arg() */

