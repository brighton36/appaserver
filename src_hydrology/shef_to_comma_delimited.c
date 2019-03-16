/* src_hydrology/shef_to_comma_delimited.c */
/* --------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "julian.h"
#include "hydrology.h"
#include "station_datatype.h"
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
#  |      shef code
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
	char station_name[ 128 ];
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
	/* SHEF_DATATYPE_CODE *shef_datatype_code; */
	STATION_DATATYPE *station_datatype;
	char *datatype_name;
	double julian_date;
	char *application_name;
	HYDROLOGY *hydrology;
	STATION *station;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 2 )
	{
		fprintf( stderr, 
			 "Usage: %s ignored\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	/* application_name = argv[ 1 ]; */

	hydrology = hydrology_new();

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
		if ( !load_shef_slash_piece(	station_name,
						input_line, 
						STATION_SLASH_PIECE ) )
		{
			fprintf( stderr, "%s\n", input_line );
			continue;
		}

#ifdef NOT_DEFINED
		station_datatype =
			station_datatype_fetch_new(
				application_name,
				station_name,
				shef /* datatype_name */,
				(char *)0 /* units_name */ );

		if ( !station_datatype )
		{
			fprintf( stderr,
"Warning in %s/%s()/%d: station_datatype_fetch_new(%s/%s) returned null.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 station,
				 shef );
			continue;
		}
#endif

		if ( !station_datatype->datatype )
		{
			fprintf( stderr,
			"Warning in %s/%s()/%d: empty datatype for (%s/%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 station,
				 shef );
			continue;
		}

		datatype_name = station_datatype->datatype->datatype_name;

		if ( !datatype_name )
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
				 	datatype_name,
				 	year,
				 	month,
				 	day,
				 	time,
				 	measurement );
			}

			julian_date = 
				julian_increment_minutes(
					julian_date,
					minutes_increment );

			strcpy( full_date, julian_to_yyyymmdd( julian_date ) );
			strcpy( time, julian_to_hhmm( julian_date ) );
		}

		station_datatype_free( station_datatype );
	}
	return 0;
} /* main() */

char *load_shef_slash_piece( char *destination, char *source, int offset )
{
	if ( !piece( destination, '/', source, offset ) ) return (char *)0;
	return destination;
}
