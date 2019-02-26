/* src_hydrology/shef_to_comma_delimited.c */
/* --------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "julian.h"
#include "shef_datatype_code.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "piece.h"

/* Sample
--------------------------------------------------------------------------------
.E/3273/STG/20000805/0900/DIH1/  6.90/  6.90/  6.90/  6.90/(M)/(M)/(M)/(M)/
.E/3273/STG/20000805/1700/DIH1/  6.90/  6.90/  6.90/  6.90/  6.90/  6.89/  6.89/
.E/3273/STG/20000806/0000/DIH1/  6.89/  6.89/(M)/(M)/(M)/(M)/(M)/(M)/(M)/  6.88/
.E/3273/STG/20000806/1000/DIH1/  6.88/  6.88/  6.98/  6.98/  6.98/  6.98/  6.98/
.E/3273/STG/20000806/1700/DIH1/  6.98/  7.02/  7.02/  7.02/  7.02/  7.02/  7.02/
.E/3273/STG/20000807/0000/DIH1/  7.02/  7.02/(M)/(M)/(M)/(M)/  7.01/  7.01/
.E/3273/STG/20000807/0800/DIH1/  7.01/
.E/ANGELS/HEAD/20000805/0900/DIH1/  6.84/  6.84/  6.84/  6.87/  6.87/  6.87/(M)/
.E/ANGELS/HEAD/20000805/1600/DIH1/(M)/  6.85/  6.85/  6.85/  6.84/  6.84/  6.84/
.E/ANGELS/HEAD/20000805/2300/DIH1/  6.84/  6.84/  6.82/  6.82/  6.82/  6.82/
#  ^      ^    ^   ^ ^  ^
#  |      |    |   | |  |
#  |      |    |   | |  hhmm
#  |      |    |   | day
#  |      |    |   month
#  |      |    year
#  |      datatype code
#  station
--------------------------------------------------------------------------------
*/

/* Constants */
/* --------- */
#define STATION_SLASH_PIECE			1
#define SHEF_SLASH_PIECE			2
#define FULL_DATE_SLASH_PIECE			3
#define TIME_SLASH_PIECE			4
#define TIME_INTERVAL_SLASH_PIECE		5
#define STARTING_MEASUREMENT_SLASH_PIECE	6

/* Prototypes */
/* ---------- */
char *load_shef_slash_piece( char *destination, char *source, int offset );

int main( int argc, char **argv )
{
	char input_line[ 1024 ];
	char station[ 128 ];
	char shef[ 128 ];
	char full_date[ 128 ];
	char year[ 128 ] = {0};
	char month[ 128 ] = {0};
	char day[ 128 ] = {0};
	char time[ 128 ];
	char time_interval[ 128 ];
	int minutes_increment;
	char measurement[ 128 ];
	int i;
	SHEF_DATATYPE_CODE *shef_datatype_code;
	char *datatype;
	double julian_date;
	char *application_name;
	char *database_string = {0};

	if ( argc != 2 )
	{
		fprintf( stderr, 
			 "Usage: %s application\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	shef_datatype_code = 
		shef_datatype_code_new( application_name );

	while( get_line( input_line, stdin ) )
	{
		if ( !*input_line || *input_line == '#' )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}
		if ( !load_shef_slash_piece(	shef, 
						input_line, 
						SHEF_SLASH_PIECE ) )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}
		if ( !load_shef_slash_piece(	station, 
						input_line, 
						STATION_SLASH_PIECE ) )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}

		datatype = shef_datatype_code_get_upload_datatype(
				(SHEF_UPLOAD_AGGREGATE_MEASUREMENT **)0,
				station,
				shef,
				shef_datatype_code->shef_upload_datatype_list,
				shef_datatype_code->
					station_datatype_list->
						station_datatype_list,
				(char *)0 /* measurement_date */,
				(char *)0 /* measurement_time */,
				0.0 /* measurement_value */ );

		if ( !datatype )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}

		if ( !load_shef_slash_piece(	full_date, 
						input_line, 
						FULL_DATE_SLASH_PIECE ) )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}

		/* Looks like 20000805 */
		/* ------------------- */
		if ( strlen( full_date ) != 8 )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}

		strncpy( year, full_date, 4 );
		strncpy( month, full_date + 4, 2 );
		strncpy( day, full_date + 6, 2 );

		if ( !load_shef_slash_piece(	time, 
						input_line, 
						TIME_SLASH_PIECE ) )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}

		if ( !load_shef_slash_piece(	time_interval,
						input_line, 
						TIME_INTERVAL_SLASH_PIECE ) )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}
		else
		{
			if ( strncmp( time_interval, "DIH", 3 ) == 0 )
				minutes_increment = 
					atoi( time_interval + 3 ) * 60;
			else
				minutes_increment = 
					atoi( time_interval + 3 );
		}

		julian_date = 
			julian_yyyymmdd_time_hhmm_to_julian( full_date, time );

		for( i = STARTING_MEASUREMENT_SLASH_PIECE;
		     piece( measurement, '/', input_line, i );
		     i++ )
		{
			if ( *measurement )
			{
				if ( strcmp( measurement, "(M)" ) == 0 )
					*measurement = '\0';
	
				printf( "%s,%s,%s-%s-%s,%s,%s\n",
				 	station,
				 	datatype,
				 	year,
				 	month,
				 	day,
				 	time,
				 	measurement );
			}

			julian_date = 
				julian_increment_minutes( julian_date,
							  minutes_increment );

			strcpy( full_date, julian_to_yyyymmdd( julian_date ) );
			strcpy( time, julian_to_hhmm( julian_date ) );

		}
	}
	return 0;
} /* main() */

char *load_shef_slash_piece( char *destination, char *source, int offset )
{
	if ( !piece( destination, '/', source, offset ) ) return (char *)0;
	return destination;
}
