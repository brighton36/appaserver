/* ---------------------------------------------------	*/
/* parse_data_speadsheet.c				*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"

#define MEASUREMENT_FILENAME		"measurement.sql"

LIST *get_elevation_datatype_record_list( void );

boolean get_elevation_datatype(
		double *elevation_meters,
		char **datatype,
		int spreadsheet_column_number );

int main( void )
{
	char input_string[ 1024 ];
	FILE *measurement_output_pipe;
	FILE *input_pipe;
	char sys_string[ 1024 ];
	char year[ 128 ];
	char full_date[ 128 ];
	char hour_minutes[ 128 ];
	char hour[ 64 ];
	char minutes[ 64 ];
	int hour_integer;
	int minutes_integer;
	char value[ 128 ];
	char *measurement_table_name;
	int datatype_offset;
	double elevation_meters;
	char *datatype;

	/* Open input pipe */
	/* --------------- */
	strcpy( sys_string, "dayofyear2date.e ',' 0 1 y 2>/dev/null" );
	input_pipe = popen( sys_string, "r" );

	/* Open output pipe */
	/* ---------------- */
	measurement_table_name = "co2flux_measurement";

	sprintf( sys_string,
"insert_statement.e table=%s field=elevation_meters,date,time,datatype,value delimiter='%c' > %s",
		 measurement_table_name,
		 '|',
		 MEASUREMENT_FILENAME );

	measurement_output_pipe = popen( sys_string, "w" );

	while( get_line( input_string, input_pipe ) )
	{
		piece( year, ',', input_string, 0 );

		if ( strcmp( year, "Year" ) == 0 ) continue;

		piece( full_date, ',', input_string, 1 );
		piece( hour_minutes, ',', input_string, 2 );

		if ( strcmp( hour_minutes, "NaN" ) == 0 ) continue;

		if ( character_exists( hour_minutes, '.' ) )
		{
			piece( hour, '.', hour_minutes, 0 );
			piece( minutes, '.', hour_minutes, 1 );
		}
		else
		{
			strcpy( hour, hour_minutes );
			*minutes = '\0';
		}
		hour_integer = atoi( hour );
		minutes_integer = atoi( minutes ) * 6;

		for(	datatype_offset = 4;
			piece(	value,
				',',
				input_string,
				datatype_offset );
			datatype_offset++ )
		{
			if ( !get_elevation_datatype(
					&elevation_meters,
					&datatype,
					datatype_offset + 1 ) )
			{
				fprintf(stderr,
	"ERROR in %s/%s()/%d: cannot get elevation_datatype for (%s)\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					input_string );
				pclose( measurement_output_pipe );
				pclose( input_pipe );
				exit( 1 );
			}

			fprintf(measurement_output_pipe,
				"%.2lf|%s|%.2d%.2d|%s|%s\n",
				elevation_meters,
				full_date,
				hour_integer,
				minutes_integer,
				datatype,
				value );
		}
	}

	pclose( measurement_output_pipe );
	pclose( input_pipe );
	exit( 0 );
} /* main() */

boolean get_elevation_datatype(
		double *elevation_meters,
		char **datatype,
		int spreadsheet_column_number )
{
	static LIST *elevation_datatype_record_list = {0};
	char *record;
	static char local_datatype[ 128 ];
	char elevation_meters_string[ 128 ];

	if ( !elevation_datatype_record_list )
	{
		elevation_datatype_record_list =
			get_elevation_datatype_record_list();
	}

	if ( !list_rewind( elevation_datatype_record_list ) ) return 0;

	do {
		record = list_get_pointer( elevation_datatype_record_list );
		if ( atoi( record ) == spreadsheet_column_number )
		{
			piece(	elevation_meters_string,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			piece(	local_datatype,
				FOLDER_DATA_DELIMITER,
				record,
				2 );

			*elevation_meters = atof( elevation_meters_string );
			*datatype = local_datatype;
			return 1;
		}
	} while( list_next( elevation_datatype_record_list ) );
	return 0;
} /* get_elevation_datatype() */

LIST *get_elevation_datatype_record_list( void )
{
	char sys_string[ 1024 ];
	char *select = "spreadsheet_column_number,elevation_meters,datatype";

	sprintf( sys_string,
		 "get_folder_data	application=co2flux		"
		 "			select=%s			"
		 "			folder=elevation_datatype	",
		 select );
	return pipe2list( sys_string );
} /* get_elevation_datatype_record_list() */
