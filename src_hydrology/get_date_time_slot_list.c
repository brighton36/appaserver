/* --------------------------------------------------- 	*/
/* get_date_time_slot_list.c			       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "julian.h"
#include "date_aggregate.h"
#include "environ.h"

/* Constants */
/* --------- */
#define MINUTES_PER_DAY		1440.0

/* Prototypes */
/* ---------- */
void output_slot_list_sort_ascending(	char *aggregate_level,
					double begin_julian,
					double end_julian,
					double julian_minutes_per_slot,
					char delimiter );

void output_slot_list_sort_descending(	char *aggregate_level,
					double begin_julian,
					double end_julian,
					double julian_minutes_per_slot,
					char delimiter );

int main( int argc, char **argv )
{
	char *begin_date;
	char *end_date;
	int slots_per_day;
	double begin_julian, end_julian;
	double minutes_per_slot;
	double julian_minutes_per_slot;
	char *aggregate_level;
	char delimiter;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 6 )
	{
		fprintf(stderr,
"Usage: %s begin_date end_date slots_per_day aggregate_level delimiter\n",
			argv[ 0 ] );
		exit( 1 );
	}

	begin_date = argv[ 1 ];
	end_date = argv[ 2 ];
	slots_per_day = atoi( argv[ 3 ] );
	aggregate_level = argv[ 4 ];
	delimiter = *argv[ 5 ];

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();

	begin_julian = 
		julian_yyyy_mm_dd_time_hhmm_to_julian( begin_date, "0000" );

	end_julian = 
		julian_yyyy_mm_dd_time_hhmm_to_julian( end_date, "2359" );

	if ( strcmp( aggregate_level, "half_hour" ) == 0 )
	{
		slots_per_day = 48;
	}
	else
	if ( strcmp( aggregate_level, "hourly" ) == 0 )
	{
		slots_per_day = 24;
	}

	minutes_per_slot = MINUTES_PER_DAY / slots_per_day;

	julian_minutes_per_slot =
			minutes_per_slot / MINUTES_PER_DAY;

	if ( strcmp( aggregate_level, "real_time" ) == 0
	||   strcmp( aggregate_level, "half_hour" ) == 0
	||   strcmp( aggregate_level, "hourly" ) == 0
	||   strcmp( aggregate_level, "daily" ) == 0 )
	{
		output_slot_list_sort_ascending(
					aggregate_level,
					begin_julian,
					end_julian,
					julian_minutes_per_slot,
					delimiter );
	}
	else
	{
		output_slot_list_sort_descending(
					aggregate_level,
					begin_julian,
					end_julian,
					julian_minutes_per_slot,
					delimiter );
	}
	exit( 0 );
} /* main() */

void output_slot_list_sort_ascending(	char *aggregate_level,
					double begin_julian,
					double end_julian,
					double julian_minutes_per_slot,
					char delimiter )
{
	FILE *output_pipe;
	char *date_yyyy_mm_dd;
	char *time_hhmm;
	double date_time_julian;
	DATE_AGGREGATE *date_aggregate = {0};

	output_pipe = popen( "sort -u", "w" );

	if ( strcmp( aggregate_level, "real_time" ) != 0
	&&   strcmp( aggregate_level, "half_hour" ) != 0
	&&   strcmp( aggregate_level, "hourly" ) != 0 )
	{
		date_aggregate = new_date_aggregate( aggregate_level );
	}

	for (	date_time_julian = begin_julian;
		date_time_julian <= end_julian + DAYS_PER_MINUTE;
		date_time_julian += julian_minutes_per_slot )
	{
		date_yyyy_mm_dd = julian_to_yyyy_mm_dd( date_time_julian );

		if ( strcmp( aggregate_level, "real_time" ) == 0
		||   strcmp( aggregate_level, "half_hour" ) == 0
		||   strcmp( aggregate_level, "hourly" ) == 0 )
		{
			time_hhmm = julian_to_hhmm( date_time_julian );
			fprintf( output_pipe,
				 "%s%c%s\n",
				 date_yyyy_mm_dd,
				 delimiter,
				 time_hhmm );
		}
		else
		{
			date_aggregate_set_yyyy_mm_dd( 	date_aggregate,
							date_yyyy_mm_dd );

			fprintf( output_pipe,
				 "%s%c0000\n",
				 date_aggregate_get_yyyy_mm_dd(
					date_aggregate ),
				 delimiter );
		}
	}

	pclose( output_pipe );
} /* output_slot_list_sort_ascending() */

void output_slot_list_sort_descending(	char *aggregate_level,
					double begin_julian,
					double end_julian,
					double julian_minutes_per_slot,
					char delimiter )
{
	FILE *output_pipe;
	char *date_yyyy_mm_dd;
	DATE_AGGREGATE *date_aggregate;
	double date_time_julian;

	output_pipe = popen( "sort -r -u", "w" );

	date_aggregate = new_date_aggregate( aggregate_level );

	for (	date_time_julian = end_julian;
		date_time_julian >= begin_julian;
		date_time_julian -= julian_minutes_per_slot )
	{
		date_yyyy_mm_dd = julian_to_yyyy_mm_dd( date_time_julian );

		date_aggregate_set_yyyy_mm_dd( 	date_aggregate,
						date_yyyy_mm_dd );
		fprintf( output_pipe,
			 "%s%c0000\n",
			 date_aggregate_get_yyyy_mm_dd(
					date_aggregate ),
			 delimiter );
	}
	pclose( output_pipe );
} /* output_slot_list_sort_descending() */

