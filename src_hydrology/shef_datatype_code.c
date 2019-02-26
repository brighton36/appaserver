/* src_hydrology/shef_datatype_code.c */
/* ---------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "shef_datatype_code.h"
#include "appaserver_library.h"
#include "piece.h"

SHEF_UPLOAD_AGGREGATE_MEASUREMENT *
		shef_upload_aggregate_measurement_new(
			char *station,
			char *datatype,
			char *measurement_date,
			char *measurement_time,
			double measurement_value )
{
	SHEF_UPLOAD_AGGREGATE_MEASUREMENT *s;

	s = (SHEF_UPLOAD_AGGREGATE_MEASUREMENT *)
		calloc( 1, sizeof( SHEF_UPLOAD_AGGREGATE_MEASUREMENT ) );

	if ( !s )
	{
		fprintf( stderr, 
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	s->station = station;
	s->datatype = datatype;
	s->measurement_date = measurement_date;
	s->measurement_time = measurement_time;
	s->measurement_value = measurement_value;

	return s;

} /* shef_upload_aggregate_measurement_new() */

SHEF_DATATYPE_CODE *shef_datatype_code_new( char *application_name )
{
	SHEF_DATATYPE_CODE *s;

	s = (SHEF_DATATYPE_CODE *)calloc( 1, sizeof( SHEF_DATATYPE_CODE ) );

	s->shef_upload_datatype_list = 
		shef_fetch_upload_datatype_list(
			application_name );

	if ( !list_length( s->shef_upload_datatype_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot fetch from SHEF_UPLOAD_DATATYPE\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	s->shef_download_datatype_list =
		shef_datatype_fetch_download_datatype_list(
			application_name );

	s->station_datatype_list =
		station_datatype_list_new(
			application_name );

	return s;

} /* shef_datatype_code_new() */

char *shef_datatype_code_get_shef_download_code(
			char *station, 
			char *datatype_string,
			LIST *shef_download_datatype_list )
{
	SHEF_DOWNLOAD_DATATYPE *datatype;
	char datatype_string_upper_case[ 128 ];

	strcpy( datatype_string_upper_case, datatype_string );
	up_string( datatype_string_upper_case );

	if ( list_rewind( shef_download_datatype_list ) )
	{
		do {
			datatype =
				list_get_pointer(
					shef_download_datatype_list );

			if ( timlib_strcmp(
				datatype_string_upper_case, 
				datatype->datatype ) == 0 
			&&   timlib_strcmp(
				station,
				datatype->station ) == 0 )
			{
				return datatype->shef_download_code;
			}
		} while( list_next( shef_download_datatype_list ) );
	}

	return (char *)0;

} /* shef_datatype_code_get_shef_download_code() */


char *shef_datatype_code_get_upload_datatype(
				SHEF_UPLOAD_AGGREGATE_MEASUREMENT **
					shef_upload_aggregate_measurement,
				char *station,
				char *shef_code,
				LIST *shef_upload_datatype_list,
				LIST *station_datatype_list,
				char *measurement_date,
				char *measurement_time,
				double measurement_value )
{
	SHEF_UPLOAD_DATATYPE *datatype;
	STATION_DATATYPE *station_datatype;
	char shef_code_upper_case[ 128 ];
	static char datatype_lower_case[ 128 ];
	boolean is_min_max = 0;
	int str_len;
	static HASH_TABLE *shef_upload_hash_table = {0};

	if ( !shef_upload_hash_table )
	{
		shef_upload_hash_table = hash_table_new( HASH_TABLE_MEDIUM );
	}

	*datatype_lower_case = '\0';

	strcpy( shef_code_upper_case, shef_code );
	up_string( shef_code_upper_case );

	if ( measurement_date )
	{
		if ( ( is_min_max = shef_is_min_max(
					&str_len,
					shef_code_upper_case ) ) )
		{
			/* Trim off the MM */
			/* --------------- */
			*( shef_code_upper_case + str_len - 2 ) = '\0';
		}
	}

	if ( ( datatype = shef_get_upload_datatype(
				station,
				shef_code_upper_case,
				shef_upload_datatype_list ) ) )
	{
		strcpy( datatype_lower_case, datatype->datatype );
		low_string( datatype_lower_case );
	}
	else
	if ( ( datatype = shef_get_upload_datatype(
				SHEF_DEFAULT_UPLOAD_STATION,
				shef_code_upper_case,
				shef_upload_datatype_list ) ) )
	{
		strcpy( datatype_lower_case, datatype->datatype );
		low_string( datatype_lower_case );
	}
	else
	if ( ( station_datatype = station_datatype_list_fetch(
			station_datatype_list,
			station,
			shef_code ) ) )
	{
		return station_datatype->datatype;
	}

	if ( *datatype_lower_case )
	{
		if ( is_min_max )
		{
			return shef_datatype_code_get_upload_min_max_datatype(
					shef_upload_aggregate_measurement,
					shef_upload_hash_table,
					station,
					datatype_lower_case,
					measurement_date,
					measurement_time,
					measurement_value );
		}

		return datatype_lower_case;
	}

	return (char *)0;

} /* shef_datatype_code_get_upload_datatype() */

char *shef_upload_datatype_get_key(	char *datatype_name,
					char *measurement_date,
					char *measurement_time )
{
	static char key[ 128 ];
	char hour[ 3 ] = {0};

	strncpy( hour, measurement_time, 2 );

	sprintf(	key,
			"%s^%s^%s",
			datatype_name,
			measurement_date,
			strdup( hour ) );

	return key;

} /* shef_upload_datatype_get_key() */

boolean shef_is_min_max(	int *str_len,
				char *shef_code_upper_case )
{
	*str_len = strlen( shef_code_upper_case );

	return ( *( shef_code_upper_case + *str_len - 2 ) == 'M'
		&&   *( shef_code_upper_case + *str_len - 1 ) == 'M' );

} /* shef_is_min_max() */

SHEF_UPLOAD_DATATYPE *shef_get_upload_datatype(
				char *station,
				char *shef_code,
				LIST *shef_upload_datatype_list )
{
	SHEF_UPLOAD_DATATYPE *datatype;

	if ( !list_rewind( shef_upload_datatype_list ) )
		return (SHEF_UPLOAD_DATATYPE *)0;

	do {
		datatype = list_get( shef_upload_datatype_list );

		if ( timlib_strcmp(
				shef_code,
				datatype->shef_upload_code ) == 0
		&&   timlib_strcmp(
				station,
				datatype->station ) == 0 )
		{
			return datatype;
		}
	} while( list_next( shef_upload_datatype_list ) );

	return (SHEF_UPLOAD_DATATYPE *)0;

} /* shef_get_upload_datatype() */

LIST *shef_fetch_upload_datatype_list( char *application_name )
{
	char input_line[ 128 ];
	char station_string[ 128 ];
	char datatype_string[ 128 ];
	char  shef_upload_code[ 128 ];
	char sys_string[ 1024 ];
	SHEF_UPLOAD_DATATYPE *datatype;
	LIST *datatype_list;
	FILE *p;

	datatype_list = list_new();

	sprintf( sys_string,
		 "shef_upload_list.sh %s",
		 application_name );

	p = popen( sys_string, "r" );
	while( get_line( input_line, p ) )
	{
		piece( station_string, '|', input_line, 0 );
		piece( datatype_string, '|', input_line, 1 );
		piece( shef_upload_code, '|', input_line, 2 );

		datatype = (SHEF_UPLOAD_DATATYPE *)
				calloc( 1, sizeof( SHEF_UPLOAD_DATATYPE ) );

		datatype->station = strdup( station_string );
		datatype->datatype = strdup( datatype_string );
		datatype->shef_upload_code = strdup( shef_upload_code );

		list_append_pointer( datatype_list, datatype );
	}
	pclose( p );
	return datatype_list;
} /* shef_fetch_upload_datatype_list() */

LIST *shef_datatype_fetch_download_datatype_list(
				char *application_name )
{
	char input_line[ 128 ];
	char station[ 128 ], datatype_string[ 128 ], shef_download_code[ 128 ];
	char sys_string[ 1024 ];
	SHEF_DOWNLOAD_DATATYPE *datatype;
	LIST *datatype_list;
	FILE *p;

	datatype_list = list_new();

	sprintf( sys_string,
		 "shef_download_list.sh %s", application_name );

	p = popen( sys_string, "r" );
	while( get_line( input_line, p ) )
	{
		piece( station, '|', input_line, 0 );
		piece( datatype_string, '|', input_line, 1 );
		piece( shef_download_code, '|', input_line, 2 );

		datatype = (SHEF_DOWNLOAD_DATATYPE *)
				calloc( 1, sizeof( SHEF_DOWNLOAD_DATATYPE ) );
		datatype->station = strdup( station );
		datatype->datatype = strdup( datatype_string );
		datatype->shef_download_code = strdup( shef_download_code );

		list_append_pointer( datatype_list, datatype );
	}
	pclose( p );

	return datatype_list;

} /* shef_datatype_fetch_download_datatype_list() */

char *shef_datatype_code_get_upload_min_max_datatype(
				SHEF_UPLOAD_AGGREGATE_MEASUREMENT **
					shef_upload_aggregate_measurement,
				HASH_TABLE *shef_upload_hash_table,
				char *station,
				char *datatype_lower_case,
				char *measurement_date,
				char *measurement_time,
				double measurement_value )
{
	char *key;
	char other_datatype[ 128 ];
	static char return_datatype[ 128 ];
	SHEF_UPLOAD_AGGREGATE_MEASUREMENT *
			local_shef_upload_aggregate_measurement;

	*shef_upload_aggregate_measurement =
		(SHEF_UPLOAD_AGGREGATE_MEASUREMENT *)0;

	key = shef_upload_datatype_get_key(
					datatype_lower_case,
					measurement_date,
					measurement_time );

	if ( ( local_shef_upload_aggregate_measurement =
			hash_table_fetch_pointer(
				shef_upload_hash_table,
				key ) ) )
	{
		if (	measurement_value <=
			local_shef_upload_aggregate_measurement->
				measurement_value )
		{
			sprintf( return_datatype,
				 "%s_min",
				 datatype_lower_case );

			sprintf( other_datatype,
				 "%s_max",
				 datatype_lower_case );
		}
		else
		{
			sprintf( return_datatype,
				 "%s_max",
				 datatype_lower_case );

			sprintf( other_datatype,
				 "%s_min",
				 datatype_lower_case );
		}

		local_shef_upload_aggregate_measurement->datatype =
			strdup( other_datatype );

		*shef_upload_aggregate_measurement =
			local_shef_upload_aggregate_measurement;

		return return_datatype;
	}

	local_shef_upload_aggregate_measurement =
		shef_upload_aggregate_measurement_new(
			strdup( station ),
			strdup( datatype_lower_case ),
			strdup( measurement_date ),
			strdup( measurement_time ),
			measurement_value );

	hash_table_set_pointer(
		shef_upload_hash_table,
		strdup( key ),
		local_shef_upload_aggregate_measurement );

	return SHEF_AGGREGATE_STUB;

} /* shef_datatype_code_get_upload_min_max_datatype() */

char *shef_get_upload_default_datatype(
				char *station,
				char *shef_code,
				LIST *shef_upload_datatype_list,
				LIST *station_datatype_list )
{
	SHEF_UPLOAD_DATATYPE *datatype;
	STATION_DATATYPE *station_datatype;

	if ( ( datatype = shef_get_upload_datatype(
				station,
				shef_code,
				shef_upload_datatype_list ) ) )
	{
		return datatype->datatype;
	}
	else
	if ( ( datatype = shef_get_upload_datatype(
				SHEF_DEFAULT_UPLOAD_STATION,
				shef_code,
				shef_upload_datatype_list ) ) )
	{
		return datatype->datatype;
	}
	else
	if ( ( station_datatype = station_datatype_list_fetch(
			station_datatype_list,
			station,
			shef_code ) ) )
	{
		return station_datatype->datatype;
	}
	else
	{
		return "";
	}

} /* shef_get_upload_default_datatype() */

