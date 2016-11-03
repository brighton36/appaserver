/* --------------------------------------------------- 	*/
/* create_delete_statements.c		    	   	*/
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
#define BAD_FILENAME		"../measurement.bad"
#define DELETE_STATEMENT_FILE	"delete_measurement_bad_time.sql"
#define DELETE_STATEMENT_USAGE	"delete_statement.e table=measurement field=station,datatype,measurement_date,measurement_time delimiter='|'"

/* Prototypes */
/* ---------- */
FILE *open_output_pipe( void );

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
	FILE *output_pipe;
	DATE_CONVERT *date_convert = {0};

	if ( ! ( input_file = fopen( BAD_FILENAME, "r" ) ) )
	{
		fprintf(stderr,
			"Error: cannot open %s for read.\n",
			BAD_FILENAME );
		exit( 1 );
	}

	output_pipe = open_output_pipe();

	date_convert = date_convert_new_date_convert( 
				international,
				"01-JAN-2006" );

	while( get_line( input_buffer, input_file ) )
	{
		piece( station, '|', input_buffer, 0 );
		piece( datatype, '|', input_buffer, 1 );
		piece( oracle_date, '|', input_buffer, 2 );
		piece( measurement_time, '|', input_buffer, 3 );
		piece( measurement_value, '|', input_buffer, 5 );

		date_convert_populate_return_date(
				date_convert->return_date,
				date_convert->source_format,
				date_convert->destination_format,
				oracle_date );

		fprintf( output_pipe,
			 "%s|%s|%s|%s\n",
			 station,
			 datatype,
			 date_convert->return_date,
			 measurement_time );
	}

	pclose( output_pipe );
	fclose( input_file );
	date_convert_free( date_convert );

	return 0;
} /* main() */

FILE *open_output_pipe( void )
{
	char sys_string[ 128 ];

	sprintf(	sys_string,
			"%s > %s",
			DELETE_STATEMENT_USAGE,
			DELETE_STATEMENT_FILE );

	return popen( sys_string, "w" );
}
