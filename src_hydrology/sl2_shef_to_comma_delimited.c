/* $APPASERVER_HOME/src_hydrology/sl2_to_comma_delimited.c */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "timlib.h"
#include "shef_datatype_code.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "piece.h"

/* Sample
--------------------------------------------------------------------------------
STG,2000/08/05,09:00/24.74,Good
 ^   ^           ^    ^     ^
 |   |           |    |     |
 |   |           |    |     |
 |   |           |    |    Status
 |   |           |  Value
 |   |         HH:MM
 |  Y/M/D
shef
--------------------------------------------------------------------------------
*/

/* Constants */
/* --------- */
#define SHEF_COMMA_PIECE			0
#define DATE_COMMA_PIECE			1
#define TIME_COMMA_PIECE			2
#define MEASUREMENT_COMMA_PIECE			3
#define STATUS_COMMA_PIECE			4

/* Prototypes */
/* ---------- */
char *load_shef_comma_piece( char *destination, char *source, int offset );

int main( int argc, char **argv )
{
	char input_line[ 4096 ];
	char shef[ 1024 ];
	char measurement_date[ 1024 ];
	char year[ 1024 ];
	char month[ 1024 ];
	char day[ 1024 ];
	char measurement_time[ 1024 ];
	char hour[ 1024 ];
	char minute[ 1024 ];
	char measurement_value_string[ 1024 ];
	char status[ 1024 ];
	SHEF_DATATYPE_CODE *shef_datatype_code;
	char *datatype_name;
	char *station_name;
	char *application_name;
	char *database_string = {0};
	SHEF_UPLOAD_AGGREGATE_MEASUREMENT *shef_upload_aggregate_measurement;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 3 )
	{
		fprintf( stderr, 
			 "Usage: %s ignored station\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	station_name = argv[ 2 ];

	shef_datatype_code = 
		shef_datatype_code_new( application_name );

	while( get_line( input_line, stdin ) )
	{
		if ( !*input_line || *input_line == '#' )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}
		if ( !load_shef_comma_piece(	shef,
						input_line, 
						SHEF_COMMA_PIECE ) )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}

		if ( !load_shef_comma_piece(	measurement_time,
						input_line, 
						TIME_COMMA_PIECE ) )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}

		/* Looks like 15:00:00 */
		/* ------------------- */
		if ( character_count( ':', measurement_time ) < 1 )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}

		piece( hour, ':', measurement_time, 0 );
		piece( minute, ':', measurement_time, 1 );

		sprintf(	measurement_time,
				"%s%s",
				hour,
				minute );

		if ( !load_shef_comma_piece(	measurement_value_string,
						input_line, 
						MEASUREMENT_COMMA_PIECE ) )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}

		if ( !load_shef_comma_piece(	measurement_date, 
						input_line, 
						DATE_COMMA_PIECE ) )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}

		/* Looks like 2000/08/05 or 05/08/2000 */
		/* ----------------------------------- */
		if ( character_count( '/', measurement_date ) != 2 )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}

		piece( year, '/', measurement_date, 0 );

		if ( atoi( year ) >= 1900 )
		{
			piece( month, '/', measurement_date, 1 );
			piece( day, '/', measurement_date, 2 );
		}
		else
		{
			strcpy( month, year );
			piece( day, '/', measurement_date, 1 );
			piece( year, '/', measurement_date, 2 );
		}

		sprintf(	measurement_date,
				"%s-%s-%s",
				year,
				month,
				day );

		shef_upload_aggregate_measurement =
			(SHEF_UPLOAD_AGGREGATE_MEASUREMENT *)0;

		datatype_name =
			shef_datatype_code_get_upload_datatype(
				&shef_upload_aggregate_measurement,
				station_name,
				shef,
				shef_datatype_code->shef_upload_datatype_list,
				shef_datatype_code->
					station_datatype_list,
				measurement_date,
				measurement_time,
				atof( measurement_value_string ) );

		if ( !datatype_name )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}

		if ( strcmp( datatype_name, SHEF_AGGREGATE_STUB ) != 0 )
		{
			printf( "%s,%s,%s,%s,%s\n",
		 		station_name,
		 		datatype_name,
		 		measurement_date,
		 		measurement_time,
		 		measurement_value_string );
		}

		if ( shef_upload_aggregate_measurement )
		{
			printf( "%s,%s,%s,%s,%.3lf\n",
				station_name,
		 		shef_upload_aggregate_measurement->
					datatype,
		 		shef_upload_aggregate_measurement->
					measurement_date,
		 		shef_upload_aggregate_measurement->
					measurement_time,
		 		shef_upload_aggregate_measurement->
					measurement_value );
		}

		if ( load_shef_comma_piece(	status,
						input_line, 
						STATUS_COMMA_PIECE ) )
		{
			if ( !*status || toupper( *status ) != 'G' )
			{
				fprintf( stderr,
				 	"Warning, inserting bad: %s\n",
				 	input_line );
			}
		}
	}

	return 0;

} /* main() */

char *load_shef_comma_piece( char *destination, char *source, int offset )
{
	if ( !piece( destination, ',', source, offset ) ) return (char *)0;
	return destination;
}
