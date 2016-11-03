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


MEASUREMENT *measurement_new_measurement( 
			char *application_name )
{
	MEASUREMENT *m;

	m = (MEASUREMENT *)calloc( 1, sizeof( MEASUREMENT ) );
	m->application_name = application_name;
	m->argv_0 = "";

	return m;
} /* measurement_new_measurement() */


void measurement_open_input_process( 	MEASUREMENT *m,
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
					MEASUREMENT *m,
					char *comma_delimited_record,
					char *argv_0,
					char *database_management_system )
{
	char station[ 128 ];
	char datatype[ 128 ];
	char date[ 128 ];
	char time[ 128 ];
	char value_string[ 128 ];
	int time_int;

	if ( character_count( ',', comma_delimited_record ) != VALUE_PIECE )
	{
		fprintf( stderr,
			 "WARNING in %s/%s(): not enough fields in (%s)\n",
			 argv_0,
			 __FUNCTION__,
			 comma_delimited_record );
		return;
	}

	piece( station, ',', comma_delimited_record, STATION_PIECE );
	piece( datatype, ',', comma_delimited_record, DATATYPE_PIECE );
	piece( date, ',', comma_delimited_record, DATE_PIECE );

	if ( strcmp( database_management_system, "oracle" ) == 0 )
		strcpy( date, timlib_yyyymmdd_to_ora_date( date ) );

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

	if ( m->measurement_record ) 
		measurement_record_free( m->measurement_record );

	m->measurement_record =
			new_measurement_record(	station,
						datatype,
						date,
						time,
						value_string );
} /* measurement_set_comma_delimited_record() */

MEASUREMENT_RECORD *new_measurement_record(	char *station,
						char *datatype,
						char *date,
						char *time,
						char *value_string )
{
	MEASUREMENT_RECORD *m;

	m = (MEASUREMENT_RECORD *)calloc( 1, sizeof( MEASUREMENT_RECORD ) );
	m->station = strdup( station );
	m->datatype = strdup( datatype );
	m->measurement_date = strdup( date );	
	m->measurement_time = strdup( time );
	m->value_string = strdup( value_string );
	m->measurement_value = 
		measurement_get_value( &m->null_value, value_string );
	return m;
} /* new_measurement_record() */

void measurement_record_free( MEASUREMENT_RECORD *measurement_record )
{
	free( measurement_record->station );
	free( measurement_record->datatype );
	free( measurement_record->measurement_date );
	free( measurement_record->measurement_time );
	free( measurement_record );
}

void measurement_insert( MEASUREMENT *measurement, int really_yn )
{
	if ( !measurement->measurement_record )
	{
		fprintf( stderr,
			 "ERROR in measurment_insert(): no record set.\n" );
		exit( 1 );
	}

	if ( really_yn != 'y' )
	{
		if ( !measurement->measurement_record->null_value )
		{
			printf( "Not inserting: %s,%s,%s,%s,%lf\n",
			measurement->measurement_record->station,
			measurement->measurement_record->datatype,
			measurement->measurement_record->measurement_date,
			measurement->measurement_record->measurement_time,
			measurement->measurement_record->measurement_value );
		}
		else
		{
			printf( "Not inserting: %s,%s,%s,%s,null\n",
			measurement->measurement_record->station,
			measurement->measurement_record->datatype,
			measurement->measurement_record->measurement_date,
			measurement->measurement_record->measurement_time );
		}
		return;
	}

	/* If shef or cr10 */
	/* --------------- */
	if ( strcmp( measurement->measurement_record->measurement_time,
		     "null" ) != 0 )
	{
		if ( strcmp( measurement->load_process, "cr10" ) != 0
		&&   measurement_exists( measurement ) )
		{
			if ( measurement->measurement_record->db_null_value )
			{
				measurement_update_mysql(
			measurement->application_name,
			measurement->measurement_record->station,
			measurement->measurement_record->datatype,
			measurement->measurement_record->measurement_date,
			measurement->measurement_record->measurement_time,
			measurement->measurement_record->measurement_value );
			}
		}
		else
		{
			measurement_output_insert_pipe(
				 measurement->insert_pipe,
			measurement->measurement_record->station,
			measurement->measurement_record->datatype,
			measurement->measurement_record->measurement_date,
			measurement->measurement_record->measurement_time,
			measurement->measurement_record->measurement_value,
			measurement->measurement_record->null_value );
		}
	}
	/* ------------- */
	/* Else realdata */
	/* ------------- */
	else
	{
		if ( !measurement_exists( measurement ) )
		{
			measurement_output_insert_pipe(
				 measurement->insert_pipe,
			measurement->measurement_record->station,
			measurement->measurement_record->datatype,
			measurement->measurement_record->measurement_date,
			measurement->measurement_record->measurement_time,
			measurement->measurement_record->measurement_value,
			measurement->measurement_record->null_value );
		}
	}

} /* measurement_insert() */

double measurement_get_value_from_db(	
				int *record_exists,
				int *db_null_value,
				char *application_name,
				char *station,
				char *datatype,
				char *date,
				char *time )
{
	char where_clause[ 512 ];
	char sys_string[ 512 ];
	char *value_string;
	double value = 0.0;

	sprintf( where_clause,
		 "station = '%s' and					"
		 "datatype = '%s' and					"
		 "measurement_date = '%s' and				"
		 "measurement_time = '%s' 				",
		 station,
		 datatype,
		 date,
		 time );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			folder=measurement		"
		 "			select=measurement_value	"
		 "			where=\"%s\" 2>/dev/null	",
		 application_name,
		 where_clause );

	value_string = pipe2string( sys_string );

	if ( value_string )
	{
		*record_exists = 1;
		if ( *value_string )
		{
			*db_null_value = 0;
			value = atof( value_string );
			free( value_string );

		}
		else
		{
			*db_null_value = 1;
			free( value_string );
		}
	}
	else
	{
		*record_exists = 0;
		*db_null_value = 0;
	}
	return value;

} /* measurement_get_value_from_db() */

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


void measurement_update_mysql(	char *application_name,
				char *station,
				char *datatype,
				char *date,
				char *time,
				double value )
{
	char sys_string[ 4096 ];
	int results;
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

	results = system( sys_string );

} /* measurement_update_mysql() */

int measurement_exists(	MEASUREMENT *measurement )
{
	double value;

	/* Temporary; until we link with ADT. */
	/* ---------------------------------- */
	return 0;

	value = measurement_get_value_from_db(
			&measurement->record_exists,
			&measurement->measurement_record->db_null_value,
			measurement->application_name,
			measurement->measurement_record->station,
			measurement->measurement_record->datatype,
			measurement->measurement_record->measurement_date,
			measurement->measurement_record->measurement_time );

	if ( measurement->record_exists
	&&   !measurement->measurement_record->db_null_value )
		measurement->measurement_record->measurement_value = value;

	return measurement->record_exists;
} /* measurement_exists() */

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

int measurement_fetch(	MEASUREMENT *measurement,
			FILE *input_pipe,
			char *database_management_system )
{
	char buffer[ 1024 ];

	if ( !get_line( buffer, input_pipe ) ) return 0;

	measurement_set_comma_delimited_record(
			measurement,
			buffer,
			measurement->argv_0,
			database_management_system );
	return 1;
} /* measurement_fetch() */

void measurement_set_argv_0( MEASUREMENT *m, char *argv_0 )
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

char *measurement_display( MEASUREMENT_RECORD *m )
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

void measurement_delete( FILE *delete_pipe, MEASUREMENT_RECORD *m )
{
	fprintf( delete_pipe,
		 "%s|%s|%s|%s\n",
		 m->station,
		 m->datatype,
		 m->measurement_date,
		 m->measurement_time );

} /* measurement_delete() */
