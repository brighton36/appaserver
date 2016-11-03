/* --------------------------------------------------- 	*/
/* separate_bad_file.c			    	   	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include "piece.h"
#include "timlib.h"
#include "date_convert.h"

/* Constants */
/* --------- */
#define BAD_FILENAME	"../measurement.bad"

/* Prototypes */
/* ---------- */
FILE *open_output_file( char *station, char *datatype );

/*
CH3|ground_water_level|26-NOV-2000|2400|26-NOV-2000:2400|1.1600||||||20011228185814
CH3|stage|26-NOV-2000|2400|26-NOV-2000:2400|1.1600||||||20011228185822
CP|rain|13-NOV-2000|2400|13-NOV-2000:2400|0.0000||||||20011228191601
CP|stage|13-NOV-2000|2400|13-NOV-2000:2400|1.5838|01-JUL-2002|schardt|eye_ball|visual_inspection||20020701135216
JB|air_temperature|08-NOV-2000|2400|08-NOV-2000:2400|24.5200||||||20011228211105
JB|air_temperature|09-NOV-2000|2400|09-NOV-2000:2400|24.6900||||||20011228211105
JB|air_temperature|10-NOV-2000|2400|10-NOV-2000:2400|25.0300||||||20011228211105
JB|air_temperature|11-NOV-2000|2400|11-NOV-2000:2400|22.1000||||||20011228211105
JB|air_temperature|12-NOV-2000|2400|12-NOV-2000:2400|23.6300||||||20011228211105
JB|air_temperature|13-NOV-2000|2400|13-NOV-2000:2400|23.8500||||||20011228211106
*/

int main( int argc, char **argv )
{
	FILE *input_file;
	char input_buffer[ 1024 ];
	char station[ 128 ];
	char datatype[ 128 ];
	char oracle_date[ 128 ];
	char measurement_time[ 16 ];
	char measurement_value[ 128 ];
	char old_station[ 128 ] = {0};
	char old_datatype[ 128 ];
	FILE *output_file = {0};
	DATE_CONVERT *date_convert = {0};

	if ( ! ( input_file = fopen( BAD_FILENAME, "r" ) ) )
	{
		fprintf(stderr,
			"Error: cannot open %s for read.\n",
			BAD_FILENAME );
		exit( 1 );
	}

	while( get_line( input_buffer, input_file ) )
	{
		piece( station, '|', input_buffer, 0 );
		piece( datatype, '|', input_buffer, 1 );
		piece( oracle_date, '|', input_buffer, 2 );
		piece( measurement_time, '|', input_buffer, 3 );
		piece( measurement_value, '|', input_buffer, 5 );

		if ( !*old_station )
		{
			strcpy( old_station, station );
			strcpy( old_datatype, datatype );
			output_file = open_output_file(
						station,
						datatype );
			date_convert = date_convert_new_date_convert( 
						international,
						oracle_date );
		}

		if ( strcmp( station, old_station ) != 0
		||   strcmp( datatype, old_datatype ) != 0 )
		{
			fclose( output_file );
			output_file = open_output_file(
						station,
						datatype );
			strcpy( old_station, station );
			strcpy( old_datatype, datatype );
		}

		date_convert_populate_return_date(
				date_convert->return_date,
				date_convert->source_format,
				date_convert->destination_format,
				oracle_date );

		fprintf( output_file,
			 "%s|%s|%s|%s|%s\n",
			 station,
			 datatype,
			 date_convert->return_date,
			 measurement_time,
			 measurement_value );
	}

	if ( output_file ) fclose( output_file );
	fclose( input_file );
	date_convert_free( date_convert );

	return 0;
} /* main() */

FILE *open_output_file( char *station, char *datatype )
{
	char output_filename[ 128 ];
	FILE *output_file;

	sprintf(	output_filename,
			"%s-%s.txt",
			station,
			datatype );
	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		fprintf(stderr,
			"Error: cannot open %s for write.\n",
			output_filename );
		exit( 1 );
	}
	return output_file;
}
