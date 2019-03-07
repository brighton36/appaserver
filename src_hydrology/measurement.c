/* measurement.c					*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"
#include "hydrology_library.h"
#include "measurement.h"

MEASUREMENT_STRUCTURE *measurement_structure_new( 
			char *application_name )
{
	MEASUREMENT_STRUCTURE *m;

	m = (MEASUREMENT_STRUCTURE *)
		calloc( 1, sizeof( MEASUREMENT_STRUCTURE ) );

	if ( !m )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	m->application_name = application_name;
	m->argv_0 = "";

	return m;

} /* measurement_structure_new() */


void measurement_open_input_process( 	MEASUREMENT_STRUCTURE *m,
					char *load_process,
					int really_yn )
{
	int delete_measurements_day;

	m->load_process = load_process;

	if ( strcmp( load_process, "realdata" ) == 0 )
		delete_measurements_day = 1;
	else
		delete_measurements_day = 0;

	m->insert_pipe = 
		measurement_open_insert_pipe(	m->application_name,
						delete_measurements_day,
						really_yn  );
} /* measurement_open_input_process() */

void measurement_set_comma_delimited_record( 
					MEASUREMENT_STRUCTURE *m,
					char *comma_delimited_record,
					char *argv_0 )
{
	char station[ 128 ];
	char datatype[ 128 ];
	char date[ 128 ];
	char time[ 128 ];
	char value_string[ 128 ];
	int time_int;

	if ( character_count( ',', comma_delimited_record ) != VALUE_PIECE )
	{
		if ( timlib_strncmp( comma_delimited_record, "skipped" ) == 0 )
		{
			m->measurement =
				measurement_new(
					comma_delimited_record,
					"" /* datatype */,
					"" /* date */,
					"" /* time */,
					"" /* value_string */ );
		}
		else
		{
			fprintf( stderr,
		"WARNING in %s.%s/%s()/%d: not enough comma fields in (%s)\n",
				argv_0,
			 	__FILE__,
			 	__FUNCTION__,
				__LINE__,
			 	comma_delimited_record );
		}
		return;
	}

	piece( station, ',', comma_delimited_record, STATION_PIECE );
	piece( datatype, ',', comma_delimited_record, DATATYPE_PIECE );
	piece( date, ',', comma_delimited_record, DATE_PIECE );

	piece( time, ',', comma_delimited_record, TIME_PIECE );

	time_int = atoi( time );

	if ( ( ( ( strcmp( time, "0000" ) != 0 )
	&&  (      strcmp( time, "null" ) != 0 )
	&&  (      time_int <= 0 || time_int > 2359 ) ) )
	||  (  timlib_exists_special_character( time )
	||     timlib_exists_special_character( station )
	||     timlib_exists_special_character( datatype ) ) )
	{
		fprintf( stderr, "Ignored: %s\n", comma_delimited_record );
		return;
	}

	piece( value_string, ',', comma_delimited_record, VALUE_PIECE );

	if ( m->measurement ) 
	{
		measurement_free( m->measurement );
	}

	m->measurement =
			measurement_new(	station,
						datatype,
						date,
						time,
						value_string );

} /* measurement_set_comma_delimited_record() */

MEASUREMENT *measurement_new(		char *station,
					char *datatype,
					char *date,
					char *time,
					char *value_string )
{
	MEASUREMENT *m;

	m = (MEASUREMENT *)calloc( 1, sizeof( MEASUREMENT ) );

	if ( !m )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	m->station = strdup( station );
	m->datatype = strdup( datatype );
	m->measurement_date = strdup( date );	
	m->measurement_time = strdup( time );
	m->value_string = strdup( value_string );
	m->measurement_value = 
		measurement_get_value( &m->null_value, value_string );
	return m;
} /* measurement_new() */

void measurement_free( MEASUREMENT *m )
{
	free( m->station );
	free( m->datatype );
	free( m->measurement_date );
	free( m->measurement_time );
	free( m );
}

void measurement_insert(	MEASUREMENT_STRUCTURE *m,
				int really_yn,
				FILE *html_table_pipe )
{
	if ( !m->measurement )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: no record set.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( really_yn != 'y' )
	{
		if ( html_table_pipe )
		{
			if ( !m->measurement->null_value )
			{
				fprintf(html_table_pipe,
					"%s,%s,%s,%s,%lf\n",
					m->measurement->station,
					m->measurement->datatype,
					m->measurement->measurement_date,
					m->measurement->measurement_time,
					m->measurement->measurement_value );
			}
			else
			{
				fprintf(html_table_pipe,
					"%s,%s,%s,%s,null\n",
					m->measurement->station,
					m->measurement->datatype,
					m->measurement->measurement_date,
					m->measurement->measurement_time );
			}
		}
		else
		{
			if ( !m->measurement->null_value )
			{
				printf( "Not inserting: %s,%s,%s,%s,%lf\n",
					m->measurement->station,
					m->measurement->datatype,
					m->measurement->measurement_date,
					m->measurement->measurement_time,
					m->measurement->measurement_value );
			}
			else
			{
				printf( "Not inserting: %s,%s,%s,%s,null\n",
					m->measurement->station,
					m->measurement->datatype,
					m->measurement->measurement_date,
					m->measurement->measurement_time );
			}
		}
		return;
	}

	measurement_output_insert_pipe(
				m->insert_pipe,
				m->measurement->station,
				m->measurement->datatype,
				m->measurement->measurement_date,
				m->measurement->measurement_time,
				m->measurement->measurement_value,
				m->measurement->null_value );

} /* measurement_insert() */

void measurement_output_insert_pipe(	FILE *insert_pipe,
					char *station,
					char *datatype,
					char *date,
					char *time,
					double value,
					int null_value )
{
	if ( !null_value )
	{
		fprintf( insert_pipe,
		 	"%s|%s|%s|%s|%lf\n",
		 	station,
		 	datatype,
		 	date,
		 	time,
		 	value );
	}
	else
	{
		fprintf( insert_pipe,
		 	"%s|%s|%s|%s|\n",
		 	station,
		 	datatype,
		 	date,
		 	time );
	}
	fflush( insert_pipe );
} /* measurement_output_insert_pipe() */

void measurement_close_insert_pipe( FILE *insert_pipe )
{
	pclose( insert_pipe );
}

void measurement_close_html_table_pipe( FILE *html_table_pipe )
{
	if ( html_table_pipe )
		pclose( html_table_pipe );
}

FILE *measurement_open_html_table_pipe(	void )
{
	char sys_string[ 1024 ];
	char *heading_comma_string;
	char *justify_string;

	heading_comma_string = "station,datatype,date,time,value";
	justify_string = "left,left,left,left,right";

	sprintf( sys_string,
		 "queue_top_bottom_lines.e 50 |"
		 "html_table.e '' '%s' ',' '%s'",
		 heading_comma_string,
		 justify_string );

	return popen( sys_string, "w" );

} /* measurement_open_html_table_pipe() */

FILE *measurement_open_insert_pipe(	char *application_name,
					int delete_measurements_day,
					int really_yn  )
{
	char sys_string[ 4096 ];
	char delete_measurements_day_process[ 128 ];
	char *table_name;

	table_name = get_table_name( application_name, "measurement" );

	if ( delete_measurements_day )
	{
		sprintf( delete_measurements_day_process,
			 "delete_measurements_day %s %d %d %d %c",
			 application_name,
			 STATION_PIECE,
			 DATATYPE_PIECE,
			 DATE_PIECE,
			 really_yn );
	}
	else
	{
		strcpy( delete_measurements_day_process, "cat -" );
	}

	sprintf(sys_string,
		"%s						|"
		"insert_statement.e %s 'station,		 "
		"			datatype,		 "
		"			measurement_date,	 "
		"			measurement_time,	 "
		"			measurement_value'	|"
		"sql.e 						 ",
		delete_measurements_day_process,
		table_name );

	return popen( sys_string, "w" );

} /* measurement_open_insert_pipe() */

void measurement_update(	char *application_name,
				char *station,
				char *datatype,
				char *date,
				char *time,
				double value )
{
	char sys_string[ 4096 ];
	char *table_name;

	table_name = get_table_name( application_name, "measurement" );

	sprintf(sys_string,
		"echo \"update %s				 "
		"      set measurement_value = %lf		 "
		"      where station = '%s'			 "
		"	 and datatype = '%s'			 "
		"	 and measurement_date = '%s'		 "
		"	 and measurement_time = '%s';\"		|"
		"sql 2>&1					|"
		"html_paragraph_wrapper.e		 	 ",
		table_name,
		value,
		station,
		datatype,
		date,
		time );

	system( sys_string );

} /* measurement_update() */

double measurement_get_value(	int *null_value,
				char *value_string )
{
	if ( !*value_string
	||   strcmp( value_string, "null" ) == 0 )
	{
		*null_value = 1;
		return NULL_REPLACEMENT;
	}
	else
	{
		*null_value = 0;
		return atof( value_string );
	}
} /* measurement_get_value() */

FILE *measurement_open_input_pipe(	char *application_name,
					char *where_clause,
					char delimiter )
{
	char sys_string[ 4096 ];

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			folder=measurement	 "
		 "			select=%s		 "
		 "			where=\"%s\"		|"
		 "tr '%c' '%c'					 ",
		 application_name,
		 MEASUREMENT_SELECT_LIST,
		 where_clause,
		 FOLDER_DATA_DELIMITER,
		 delimiter );

	return popen( sys_string, "r" );
} /* measurement_open_input_pipe() */

int measurement_fetch(	MEASUREMENT_STRUCTURE *m,
			FILE *input_pipe )
{
	char buffer[ 1024 ];

	if ( !get_line( buffer, input_pipe ) ) return 0;

	measurement_set_comma_delimited_record(
			m,
			buffer,
			m->argv_0 );
	return 1;
} /* measurement_fetch() */

void measurement_set_argv_0( MEASUREMENT_STRUCTURE *m, char *argv_0 )
{
	m->argv_0 = argv_0;
}

FILE *measurement_open_delete_pipe( char *application_name )
{
	char sys_string[ 1024 ];
	char *table_name;

	table_name = get_table_name( application_name, "measurement" );

	sprintf( sys_string,
"delete_statement.e %s station,datatype,measurement_date,measurement_time | sql.e",
		 table_name );
	return popen( sys_string, "w" );
} /* measurement_open_delete_pipe() */

char *measurement_display( MEASUREMENT *m )
{
	static char buffer[ 1024 ];

	if ( !m->null_value )
	{
		sprintf( buffer,
		 	"%s,%s,%s,%s,%lf\n",
		 	m->station,
		 	m->datatype,
		 	m->measurement_date,
		 	m->measurement_time,
		 	m->measurement_value );
	}
	else
	{
		sprintf( buffer,
		 	"%s,%s,%s,%s,\n",
		 	m->station,
		 	m->datatype,
		 	m->measurement_date,
		 	m->measurement_time );
	}

	return buffer;
} /* measurement_display() */

void measurement_delete( FILE *delete_pipe, MEASUREMENT *m )
{
	fprintf( delete_pipe,
		 "%s|%s|%s|%s\n",
		 m->station,
		 m->datatype,
		 m->measurement_date,
		 m->measurement_time );

} /* measurement_delete() */

DICTIONARY *measurement_get_date_time_frequency_dictionary(
				char *application_name,
				char *station,
				char *datatype,
				char *begin_measurement_date_string,
				char *end_measurement_date_string )
{
	DICTIONARY *date_time_frequency_dictionary;
	char sys_string[ 1024 ];
	char input_buffer[ 128 ];
	FILE *input_pipe;

	sprintf(sys_string,
		"data_collection_frequency_list %s %s %s %s '%s' '%s'",
		application_name,
		station,
		datatype,
		"real_time",
		begin_measurement_date_string,
		end_measurement_date_string );

	date_time_frequency_dictionary = dictionary_medium_new();
	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		dictionary_set_pointer(
				date_time_frequency_dictionary,
				strdup( input_buffer ),
				"" );
	}

	pclose( input_pipe );

	return date_time_frequency_dictionary;

} /* measurement_get_date_time_frequency_dictionary() */

boolean measurement_date_time_frequency_exists(
				DICTIONARY *date_time_frequency_dictionary,
				char *measurement_date_string,
				char *measurement_time_string )
{
	char key[ 128 ];

	sprintf(key,
		"%s^%s",
		measurement_date_string,
		measurement_time_string );

	return dictionary_exists_key( date_time_frequency_dictionary, key );

} /* measurement_date_time_frequency_exists() */

MEASUREMENT_FREQUENCY_STATION_DATATYPE *
		measurement_frequency_station_datatype_new(
					char *station,
					char *datatype )
{
	MEASUREMENT_FREQUENCY_STATION_DATATYPE *m;

	m = (MEASUREMENT_FREQUENCY_STATION_DATATYPE *)
		calloc( 1,
			sizeof( MEASUREMENT_FREQUENCY_STATION_DATATYPE ) );

	if ( !m )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	m->station = station;
	m->datatype = datatype;

	return m;

} /* measurement_frequency_station_datatype_new() */

MEASUREMENT_FREQUENCY_STATION_DATATYPE *
		measurement_frequency_get_or_set_station_datatype(
					LIST *frequency_station_datatype_list,
					char *station,
					char *datatype )
{
	MEASUREMENT_FREQUENCY_STATION_DATATYPE *m;

	if ( !frequency_station_datatype_list )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( frequency_station_datatype_list ) )
	{
		m = measurement_frequency_station_datatype_new(
					station,
					datatype );

		list_append_pointer( frequency_station_datatype_list, m );
		return m;
	}

	do {
		m = list_get_pointer( frequency_station_datatype_list );

		if ( strcmp( m->station, station ) == 0
		&&   strcmp( m->datatype, datatype ) == 0 )
		{
			return m;
		}

	} while( list_next( frequency_station_datatype_list ) );

	m = measurement_frequency_station_datatype_new(
				station,
				datatype );

	list_append_pointer( frequency_station_datatype_list, m );

	return m;

} /* measurement_frequency_get_or_set_station_datatype() */

MEASUREMENT_FREQUENCY *measurement_frequency_new( void )
{
	MEASUREMENT_FREQUENCY *m;

	m = (MEASUREMENT_FREQUENCY *)
		calloc( 1,
			sizeof( MEASUREMENT_FREQUENCY ) );

	if ( !m )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	m->frequency_station_datatype_list = list_new();

	return m;

} /* measurement_frequency_new() */

