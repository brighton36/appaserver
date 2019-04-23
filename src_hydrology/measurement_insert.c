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
	int really_yn;
	MEASUREMENT_STRUCTURE *m;
	int not_loaded_count = 0;
	char *database_string = {0};
	MEASUREMENT_FREQUENCY *measurement_frequency;
	MEASUREMENT_FREQUENCY_STATION_DATATYPE *
		measurement_frequency_station_datatype;
	char *begin_measurement_date = {0};
	FILE *input_pipe;

	if ( argc != 4 )
	{
		fprintf(stderr,
"Usage: echo \"station,datatype,date,time,value\" | %s application cr10|shef|realdata really_yn\n", 
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	load_process = argv[ 2 ];
	really_yn = *argv[ 3 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	m = measurement_structure_new( application_name );
	measurement_frequency = measurement_frequency_new();

	if ( really_yn != 'y' )
	{
		m->html_table_pipe = measurement_open_html_table_pipe();
	}

	measurement_open_input_process( m, load_process, really_yn );

	input_pipe = popen( "sort", "r" );

	while( get_line( comma_delimited_record, input_pipe ) )
	{
		measurement_set_comma_delimited_record(
			m, 
			comma_delimited_record,
			argv[ 0 ] );

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
		&&   really_yn != 'y' )
		{
			not_loaded_count++;
		}

		measurement_insert(
			m,
			really_yn,
			m->html_table_pipe );
	}

	pclose( input_pipe );

	if ( m->insert_pipe ) pclose( m->insert_pipe );

	if ( m->html_table_pipe ) pclose( m->html_table_pipe );

	if ( not_loaded_count )
		printf( "<p>Not loaded count = %d\n", not_loaded_count );

	return 0;

} /* main() */

