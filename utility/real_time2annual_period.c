/* real_time2annual_period.c				*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "julian.h"
#include "piece.h"
#include "timlib.h"
#include "statistics_weighted.h"
#include "appaserver_library.h"

/* Constants */
/* --------- */
#define LEAP_YEAR	"2000"

/* Enumerated types */
/* ---------------- */
enum season_year_relationship {	season_crosses_year,
				season_within_year,
				season_entire_year };

enum season_match_results {	different_season,
				same_season_different_year,
				same_season_same_year };

enum within_range_results {	was_outside_range_now_within,
				within_range_again,
				within_range_first_time,
				outside_range,
				within_range_unknown };

/* Prototypes */
/* ---------- */
enum season_year_relationship get_season_year_relationship(
				char *begin_season_month_day_string,
				char *end_season_month_day_string );

char *get_season_match_results_string(
			enum season_match_results season_match_results );

boolean get_seasonally_begin_end_month_day_string(
				char *begin_season_month_day_string,
				char *end_season_month_day_string,
				char *application_name,
				char *period );

boolean is_same_year(		JULIAN *new_date,
				JULIAN *old_date );

enum season_match_results get_season_match_results(
				JULIAN *old_date,
				JULIAN *new_date,
				int first_year,
				char *begin_season_month_day_string,
				char *end_season_month_day_string,
				enum season_year_relationship );

void compute_statistics_and_output(
				JULIAN *date,
				char *aggregate_statistic,
				char delimiter,
				STATISTICS_WEIGHTED *statistics_weighted );

void output(
				JULIAN *date,
				char *aggregate_statistic, 
				char delimiter,
				STATISTICS_WEIGHTED *statistics_weighted );

int get_month_integer(		char *month_string );

int get_week_integer(		char *containing_date_string );

int get_day_integer(		char *containing_date_string );

int get_day_of_month_integer(	char *containing_date_string );

int get_daily_month_integer(	char *containing_date_string );


int real_time2annual_annually_period(
				char *aggregate_statistic,
				JULIAN *old_date,
				JULIAN *new_date,
				char *value_string,
				char delimiter,
				boolean output_only );

int real_time2annual_seasonally_period(
				char *application_name,
				char *aggregate_statistic,
				JULIAN *old_date,
				JULIAN *new_date,
				char *value_string,
				char *period,
				char delimiter,
				int first_year,
				boolean output_only );

int real_time2annual_weekly_period(
				char *aggregate_statistic,
				JULIAN *old_date,
				JULIAN *new_date,
				char *value_string,
				int period_week,
				char delimiter,
				boolean output_only );

int real_time2annual_daily_period(
				char *aggregate_statistic,
				JULIAN *old_date,
				JULIAN *new_date,
				char *value_string,
				int period_day,
				int period_month,
				char delimiter,
				boolean output_only );

int real_time2annual_dynamic_period(
				char *aggregate_statistic,
				JULIAN *old_date,
				JULIAN *new_date,
				char *value_string,
				int period_day,
				int ending_period_day,
				char delimiter,
				boolean output_only );

int real_time2annual_monthly_period(
				char *aggregate_statistic,
				JULIAN *old_date,
				JULIAN *new_date,
				char *value_string,
				char *period,
				char delimiter,
				boolean output_only );

int main( int argc, char **argv )
{
	char *application_name;
	char *aggregate_statistic;
	int date_piece_offset;
	int value_piece_offset;
	char delimiter;
	char *aggregate_period;
	char *period;
	char *ending_period = {0};
	char input_buffer[ 1024 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	JULIAN *new_date;
	JULIAN *old_date;
	int first_time = 1;
	int period_week = 0;
	int period_day = 0;
	int period_month = 0;
	int ending_period_day = 0;
	int first_year = -1;

	output_starting_argv_stderr( argc, argv );

	if ( argc < 8 )
	{
		fprintf(stderr,
"Usage: %s application aggregate_statistic date_piece_offset value_piece_offset delimiter aggregate_period period [ending_period]\n",
			argv[ 0 ] );
		fprintf(stderr,
"Note: the ending_period is used if the aggregate_period is dynamic.\n" );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	aggregate_statistic = argv[ 2 ];
	date_piece_offset = atoi( argv[ 3 ] );
	value_piece_offset = atoi( argv[ 4 ] );
	delimiter = *argv[ 5 ];
	aggregate_period = argv[ 6 ];
	period = argv[ 7 ];

	if ( argc == 9 ) ending_period = argv[ 8 ];

	new_date = julian_new_julian( 0 );
	old_date = julian_new_julian( 0 );

	if ( strcmp( aggregate_period, "weekly" ) == 0 )
	{
		if ( ( period_week = get_week_integer( period ) ) == -1 )
		{
			fprintf(stderr,
			"ERROR in %s/%s(): cannot get week integer for %s\n",
				__FILE__,
				__FUNCTION__,
				period );
			return 0;
		}
	}
	else
	if ( strcmp( aggregate_period, "daily" ) == 0 )
	{
		if ( ( period_day = get_day_of_month_integer( period ) ) == -1 )
		{
			fprintf(stderr,
		"ERROR in %s/%s(): cannot get day of month integer for %s\n",
				__FILE__,
				__FUNCTION__,
				period );
			return 0;
		}

		if ( ( period_month =
			get_daily_month_integer( period ) ) == -1 )
		{
			fprintf(stderr,
		"ERROR in %s/%s(): cannot get period month integer for %s\n",
				__FILE__,
				__FUNCTION__,
				period );
			return 0;
		}
	}
	else
	if ( strcmp( aggregate_period, "dynamically" ) == 0 )
	{
		if ( ( period_day = get_day_integer( period ) ) == -1 )
		{
			fprintf(stderr,
			"ERROR in %s/%s(): cannot get day integer for %s\n",
				__FILE__,
				__FUNCTION__,
				period );
			return 0;
		}

		if ( ( ending_period_day =
				get_day_integer( ending_period ) ) == -1 )
		{
			fprintf(stderr,
			"ERROR in %s/%s(): cannot get day integer for %s\n",
				__FILE__,
				__FUNCTION__,
				ending_period );
			return 0;
		}
	}

	while( get_line( input_buffer, stdin ) )
	{

/*
fprintf( stderr, "input_buffer = %s\n", input_buffer );
*/

		if ( !piece(	date_string, 
				delimiter, 
				input_buffer, 
				date_piece_offset ) )
		{
			fprintf(stderr,
"%s Warning: cannot extract date using delimiter = (%c) in (%s)\n",
				__FILE__,
				delimiter,
				input_buffer );
			continue;
		}

		if ( first_year == -1 )
		{
			first_year = atoi( date_string );
		}

		*value_string = '\0';

		if ( !piece(	value_string, 
				delimiter, 
				input_buffer, 
				value_piece_offset ) )
		{
			fprintf(stderr,
"%s Warning: cannot extract value using delimiter = (%c) in (%s)\n",
				__FILE__,
				delimiter,
				input_buffer );
			continue;
		}

		julian_set_yyyy_mm_dd( new_date, date_string );

/*
fprintf( stderr, "got new_date = %s\n", julian_display( new_date->current ) );
*/

		if ( first_time )
		{
			julian_copy( old_date, new_date );
			first_time = 0;
		}

/*
fprintf( stderr, "got old_date = %s\n", julian_display( old_date->current ) );
*/

		if ( strcmp( aggregate_period, "annually" ) == 0 )
		{
			if ( !real_time2annual_annually_period(
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					delimiter,
					0 /* not output_only */ ) )
			{
				exit( 1 );
			}
		} /* if annually */
		else
		if ( strcmp( aggregate_period, "seasonally" ) == 0 )
		{
			if ( !real_time2annual_seasonally_period(
					application_name,
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period,
					delimiter,
					first_year,
					0 /* not output_only */ ) )
			{
				exit( 1 );
			}
		} /* if seasonally */
		else
		if ( strcmp( aggregate_period, "monthly" ) == 0 )
		{
			if ( !real_time2annual_monthly_period(
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period,
					delimiter,
					0 /* not output_only */ ) )
			{
				exit( 1 );
			}
		} /* if monthly */
		else
		if ( strcmp( aggregate_period, "weekly" ) == 0 )
		{
			if ( !real_time2annual_weekly_period(
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period_week,
					delimiter,
					0 /* not output_only */ ) )
			{
				exit( 1 );
			}
		} /* if weekly */
		else
		if ( strcmp( aggregate_period, "daily" ) == 0 )
		{
			if ( !real_time2annual_daily_period(
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period_day,
					period_month,
					delimiter,
					0 /* not output_only */ ) )
			{
				exit( 1 );
			}
		} /* if daily */
		else
		if ( strcmp( aggregate_period, "dynamically" ) == 0 )
		{
			if ( !real_time2annual_dynamic_period(
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period_day,
					ending_period_day,
					delimiter,
					0 /* not output_only */ ) )
			{
				exit( 1 );
			}
		} /* if dynamically */

		julian_copy( old_date, new_date );

	} /* while( get_line( input_buffer, stdin ) ) */

	if ( strcmp( aggregate_period, "annually" ) == 0 )
	{
		real_time2annual_annually_period(
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					delimiter,
					1 /* output_only */ );
	}
	else
	if ( strcmp( aggregate_period, "seasonally" ) == 0 )
	{
		real_time2annual_seasonally_period(
					application_name,
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period,
					delimiter,
					first_year,
					1 /* output_only */ );
	}
	else
	if ( strcmp( aggregate_period, "monthly" ) == 0 )
	{
		real_time2annual_monthly_period(
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period,
					delimiter,
					1 /* output_only */ );
	}
	else
	if ( strcmp( aggregate_period, "weekly" ) == 0 )
	{
		real_time2annual_weekly_period(
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period_week,
					delimiter,
					1 /* output_only */ );
	}
	else
	if ( strcmp( aggregate_period, "daily" ) == 0 )
	{
		real_time2annual_daily_period(
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period_day,
					period_month,
					delimiter,
					1 /* output_only */ );
	}
	else
	if ( strcmp( aggregate_period, "dynamically" ) == 0 )
	{
		real_time2annual_dynamic_period(
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period_day,
					ending_period_day,
					delimiter,
					1 /* output_only */ );
	}

	return 0;
} /* main() */

int get_month_integer( char *month_string )
{
	char *month_string_array[] =	{	"january",
						"february",
						"march",
						"april",
						"may",
						"june",
						"july",
						"august",
						"september",
						"october",
						"november",
						"december",
						(char *)0 };
	char **compare_month_pointer = month_string_array;
	int month_integer = 1;

	while( **compare_month_pointer )
	{
		if ( strcasecmp( month_string, *compare_month_pointer ) == 0 )
			return month_integer;
		compare_month_pointer++;
		month_integer++;
	}
	return 0;
} /* get_month_integer() */

int get_week_integer( char *containing_date_string )
{
	JULIAN *date;
	int week_integer;

	date = julian_yyyy_mm_dd_new( containing_date_string );

	if ( !date ) return -1;

	week_integer = julian_get_week_number( date->current );
	julian_free( date );
	return week_integer;
} /* get_week_integer() */

int get_daily_month_integer( char *containing_date_string )
{
	JULIAN *date;
	int month_integer;

	date = julian_yyyy_mm_dd_new( containing_date_string );

	if ( !date ) return -1;

	month_integer = julian_get_month_number( date->current );
	julian_free( date );
	return month_integer;
} /* get_dayly_month_integer() */

int get_day_of_month_integer( char *containing_date_string )
{
	JULIAN *date;
	int day_of_month_integer;

	date = julian_yyyy_mm_dd_new( containing_date_string );

	if ( !date ) return -1;

	day_of_month_integer = julian_get_day_of_month_number( date->current );
	julian_free( date );
	return day_of_month_integer;
} /* get_day_of_month_integer() */

int get_day_integer( char *containing_date_string )
{
	JULIAN *date;
	int day_integer;

	date = julian_yyyy_mm_dd_new( containing_date_string );

	if ( !date ) return -1;

	day_integer = julian_get_day_number( date->current );
	julian_free( date );
	return day_integer;
} /* get_day_integer() */

void output(		JULIAN *date,
			char *aggregate_statistic, 
			char delimiter,
			STATISTICS_WEIGHTED *statistics_weighted )
{
	char results_string[ 128 ];

	if ( julian_get_year_number( date->current ) == 1900 ) return;

	if ( statistics_weighted->count == 0 )
	{
		strcpy( results_string, "null" );
	}
	else
	if ( strcmp( aggregate_statistic, "sum" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->sum );
	else
	if ( strcmp( aggregate_statistic, "total" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->sum );
	else
	if ( strcmp( aggregate_statistic, "average" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->average );
	else
	if ( strcmp( aggregate_statistic, "minimum" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->minimum );
	else
	if ( strcmp( aggregate_statistic, "median" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->median );
	else
	if ( strcmp( aggregate_statistic, "maximum" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->maximum );
	else
	if ( strcmp( aggregate_statistic, "range" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->range );
	else
	if ( strcmp( aggregate_statistic, "standard_deviation" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->standard_deviation );
	else
		strcpy( results_string, "AGGREGATE?" );

	printf( "%s%c%s%c%ld\n",
		julian_get_yyyy_mm_dd_string( date->current ),
		delimiter,
		results_string,
		delimiter,
		statistics_weighted->count );

} /* output() */

int real_time2annual_weekly_period(
				char *aggregate_statistic,
				JULIAN *old_date,
				JULIAN *new_date,
				char *value_string,
				int period_week,
				char delimiter,
				boolean output_only )
{
	static STATISTICS_WEIGHTED *statistics_weighted = {0};
	int new_week;
	int new_year;
	int old_year;

	if ( output_only )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
		}

		return 1;
	}

	old_year = julian_get_year_number( old_date->current );
	new_year = julian_get_year_number( new_date->current );
	new_week = julian_get_week_number( new_date->current );

	if ( period_week != new_week )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}
		return 1;
	}
	else
	if ( old_year != new_year )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}

		statistics_weighted =
			statistics_weighted_new_statistics_weighted();

		statistics_weighted->buffer = value_string;

		statistics_weighted_accumulate(
			&statistics_weighted->number_array,
			&statistics_weighted->weighted_input_count,
			&statistics_weighted->exceeded_max_numbers,
			&statistics_weighted->count,
			&statistics_weighted->count_non_zero,
			&statistics_weighted->weighted_count,
			&statistics_weighted->sum,
			&statistics_weighted->weighted_sum,
			statistics_weighted->buffer,
			-1 /* statistics_weighted->value_piece */,
			-1 /* statistics_weighted->weight_piece */,
			(char)0 /* delimiter */ );
		return 1;
	}

	if ( !statistics_weighted )
	{
		statistics_weighted =
			statistics_weighted_new_statistics_weighted();
	}

	statistics_weighted->buffer = value_string;

	statistics_weighted_accumulate(
		&statistics_weighted->number_array,
		&statistics_weighted->weighted_input_count,
		&statistics_weighted->exceeded_max_numbers,
		&statistics_weighted->count,
		&statistics_weighted->count_non_zero,
		&statistics_weighted->weighted_count,
		&statistics_weighted->sum,
		&statistics_weighted->weighted_sum,
		statistics_weighted->buffer,
		-1 /* statistics_weighted->value_piece */,
		-1 /* statistics_weighted->weight_piece */,
		(char)0 /* delimiter */ );
	return 1;
} /* real_time2annual_weekly_period() */

int real_time2annual_daily_period(
				char *aggregate_statistic,
				JULIAN *old_date,
				JULIAN *new_date,
				char *value_string,
				int period_day,
				int period_month,
				char delimiter,
				boolean output_only )
{
	static STATISTICS_WEIGHTED *statistics_weighted = {0};
	int new_year;
	int old_year;
	int new_day;
	int new_month;

	if ( output_only )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
		}

		return 1;
	}

	old_year = julian_get_year_number( old_date->current );
	new_year = julian_get_year_number( new_date->current );
	new_day = julian_get_day_of_month_number( new_date->current );
	new_month = julian_get_month_number( new_date->current );

	if ( period_day != new_day
	||   period_month != new_month )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}
		return 1;
	}
	else
	if ( old_year != new_year )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}

		statistics_weighted =
			statistics_weighted_new_statistics_weighted();

		statistics_weighted->buffer = value_string;

		statistics_weighted_accumulate(
			&statistics_weighted->number_array,
			&statistics_weighted->weighted_input_count,
			&statistics_weighted->exceeded_max_numbers,
			&statistics_weighted->count,
			&statistics_weighted->count_non_zero,
			&statistics_weighted->weighted_count,
			&statistics_weighted->sum,
			&statistics_weighted->weighted_sum,
			statistics_weighted->buffer,
			-1 /* statistics_weighted->value_piece */,
			-1 /* statistics_weighted->weight_piece */,
			(char)0 /* delimiter */ );
		return 1;
	}

	if ( !statistics_weighted )
	{
		statistics_weighted =
			statistics_weighted_new_statistics_weighted();
	}

	statistics_weighted->buffer = value_string;

/*
fprintf( stderr, "%s/%s()/%d: accumulating year = %d, month = %d, day = %d\n",
__FILE__,
__FUNCTION__,
__LINE__,
new_year,
new_month,
new_day );
*/

	statistics_weighted_accumulate(
		&statistics_weighted->number_array,
		&statistics_weighted->weighted_input_count,
		&statistics_weighted->exceeded_max_numbers,
		&statistics_weighted->count,
		&statistics_weighted->count_non_zero,
		&statistics_weighted->weighted_count,
		&statistics_weighted->sum,
		&statistics_weighted->weighted_sum,
		statistics_weighted->buffer,
		-1 /* statistics_weighted->value_piece */,
		-1 /* statistics_weighted->weight_piece */,
		(char)0 /* delimiter */ );
	return 1;
} /* real_time2annual_daily_period() */

int real_time2annual_monthly_period(
				char *aggregate_statistic,
				JULIAN *old_date,
				JULIAN *new_date,
				char *value_string,
				char *period,
				char delimiter,
				boolean output_only )
{
	int period_month;
	static STATISTICS_WEIGHTED *statistics_weighted = {0};
	int new_year;
	int new_month;
	int old_year;

	if ( output_only )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
		}

		return 1;
	}

	old_year = julian_get_year_number( old_date->current );
	new_year = julian_get_year_number( new_date->current );
	new_month = julian_get_month_number( new_date->current );

	if ( ! ( period_month = get_month_integer( period ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot get month integer for %s\n",
			__FILE__,
			__FUNCTION__,
			period );
		return 0;
	}

	if ( period_month != new_month )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}
		return 1;
	}
	else
	if ( old_year != new_year )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}

		statistics_weighted =
			statistics_weighted_new_statistics_weighted();

		statistics_weighted->buffer = value_string;

		statistics_weighted_accumulate(
			&statistics_weighted->number_array,
			&statistics_weighted->weighted_input_count,
			&statistics_weighted->exceeded_max_numbers,
			&statistics_weighted->count,
			&statistics_weighted->count_non_zero,
			&statistics_weighted->weighted_count,
			&statistics_weighted->sum,
			&statistics_weighted->weighted_sum,
			statistics_weighted->buffer,
			-1 /* statistics_weighted->value_piece */,
			-1 /* statistics_weighted->weight_piece */,
			(char)0 /* delimiter */ );
		return 1;
	}

	if ( !statistics_weighted )
	{
		statistics_weighted =
			statistics_weighted_new_statistics_weighted();
	}

	statistics_weighted->buffer = value_string;

	statistics_weighted_accumulate(
		&statistics_weighted->number_array,
		&statistics_weighted->weighted_input_count,
		&statistics_weighted->exceeded_max_numbers,
		&statistics_weighted->count,
		&statistics_weighted->count_non_zero,
		&statistics_weighted->weighted_count,
		&statistics_weighted->sum,
		&statistics_weighted->weighted_sum,
		statistics_weighted->buffer,
		-1 /* statistics_weighted->value_piece */,
		-1 /* statistics_weighted->weight_piece */,
		(char)0 /* delimiter */ );
	return 1;
} /* real_time2annual_monthly_period() */

void compute_statistics_and_output(
				JULIAN *date,
				char *aggregate_statistic,
				char delimiter,
				STATISTICS_WEIGHTED *statistics_weighted )
{
	statistics_weighted->average =
		statistics_weighted_compute_average(
				statistics_weighted->weighted_count,
				statistics_weighted->weighted_sum );

	statistics_weighted_sort_number_array(
			&statistics_weighted->number_array );

	statistics_weighted->standard_deviation =
			statistics_weighted_compute_standard_deviation(
				&statistics_weighted->coefficient_of_variation,
				&statistics_weighted->standard_error_of_mean,
				statistics_weighted->average,
				&statistics_weighted->number_array );

	statistics_weighted->percent_missing =
			statistics_weighted_compute_percent_missing(
				statistics_weighted->
					number_array.number_in_array,
				statistics_weighted->weighted_input_count,
				statistics_weighted->count );

	statistics_weighted_compute_minimum_median_maxium_range(
				&statistics_weighted->minimum,
				&statistics_weighted->median,
				&statistics_weighted->maximum,
				&statistics_weighted->range,
				&statistics_weighted->number_array );

	output(		date, 
			aggregate_statistic,
			delimiter,
			statistics_weighted );

} /* compute_statistics_and_output() */

int real_time2annual_annually_period(
				char *aggregate_statistic,
				JULIAN *old_date,
				JULIAN *new_date,
				char *value_string,
				char delimiter,
				boolean output_only )
{
	static STATISTICS_WEIGHTED *statistics_weighted = {0};

	if ( output_only )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
		}

		return 1;
	}

	if ( !is_same_year( new_date, old_date ) )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}

		statistics_weighted =
			statistics_weighted_new_statistics_weighted();

		statistics_weighted->buffer = value_string;

		statistics_weighted_accumulate(
			&statistics_weighted->number_array,
			&statistics_weighted->weighted_input_count,
			&statistics_weighted->exceeded_max_numbers,
			&statistics_weighted->count,
			&statistics_weighted->count_non_zero,
			&statistics_weighted->weighted_count,
			&statistics_weighted->sum,
			&statistics_weighted->weighted_sum,
			statistics_weighted->buffer,
			-1 /* statistics_weighted->value_piece */,
			-1 /* statistics_weighted->weight_piece */,
			(char)0 /* delimiter */ );
	}
	else
	/* ----------------- */
	/* Must be same year */
	/* ----------------- */
	{
		if ( !statistics_weighted )
		{
			statistics_weighted =
				statistics_weighted_new_statistics_weighted();
		}
	
		statistics_weighted->buffer = value_string;
	
		statistics_weighted_accumulate(
			&statistics_weighted->number_array,
			&statistics_weighted->weighted_input_count,
			&statistics_weighted->exceeded_max_numbers,
			&statistics_weighted->count,
			&statistics_weighted->count_non_zero,
			&statistics_weighted->weighted_count,
			&statistics_weighted->sum,
			&statistics_weighted->weighted_sum,
			statistics_weighted->buffer,
			-1 /* statistics_weighted->value_piece */,
			-1 /* statistics_weighted->weight_piece */,
			(char)0 /* delimiter */ );
	}
	return 1;
} /* real_time2annual_annually_period() */

int real_time2annual_seasonally_period(
				char *application_name,
				char *aggregate_statistic,
				JULIAN *old_date,
				JULIAN *new_date,
				char *value_string,
				char *period,
				char delimiter,
				int first_year,
				boolean output_only )
{
	static STATISTICS_WEIGHTED *statistics_weighted = {0};
	enum season_match_results season_match_results;
	static char begin_season_month_day_string[ 16 ] = {0};
	static char end_season_month_day_string[ 16 ] = {0};
	static enum season_year_relationship season_year_relationship;

	if ( !*begin_season_month_day_string )
	{
		if ( !get_seasonally_begin_end_month_day_string(
			begin_season_month_day_string,
			end_season_month_day_string,
			application_name,
			period ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: cannot fetch from SEASONS the season = (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				period );
			exit( 1 );
		}

		season_year_relationship =
			get_season_year_relationship(
				begin_season_month_day_string,
				end_season_month_day_string );
	}

	if ( output_only )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
		}

		return 1;
	}

	season_match_results =
		get_season_match_results(
			old_date,
			new_date,
			first_year,
			begin_season_month_day_string,
			end_season_month_day_string,
			season_year_relationship );

/*
fprintf( stderr, "got season_match_results = %s\n",
get_season_match_results_string( season_match_results ) );
*/

	if ( season_match_results == different_season )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}
		return 1;
	}

	if ( season_match_results == same_season_different_year )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}

		statistics_weighted =
			statistics_weighted_new_statistics_weighted();

		statistics_weighted->buffer = value_string;

		statistics_weighted_accumulate(
			&statistics_weighted->number_array,
			&statistics_weighted->weighted_input_count,
			&statistics_weighted->exceeded_max_numbers,
			&statistics_weighted->count,
			&statistics_weighted->count_non_zero,
			&statistics_weighted->weighted_count,
			&statistics_weighted->sum,
			&statistics_weighted->weighted_sum,
			statistics_weighted->buffer,
			-1 /* statistics_weighted->value_piece */,
			-1 /* statistics_weighted->weight_piece */,
			(char)0 /* delimiter */ );
		return 1;
	}

	/* Must be same_season_same_year */
	/* ----------------------------- */
	if ( !statistics_weighted )
	{
		statistics_weighted =
			statistics_weighted_new_statistics_weighted();
	}

	statistics_weighted->buffer = value_string;

	statistics_weighted_accumulate(
		&statistics_weighted->number_array,
		&statistics_weighted->weighted_input_count,
		&statistics_weighted->exceeded_max_numbers,
		&statistics_weighted->count,
		&statistics_weighted->count_non_zero,
		&statistics_weighted->weighted_count,
		&statistics_weighted->sum,
		&statistics_weighted->weighted_sum,
		statistics_weighted->buffer,
		-1 /* statistics_weighted->value_piece */,
		-1 /* statistics_weighted->weight_piece */,
		(char)0 /* delimiter */ );
	return 1;
} /* real_time2annual_seasonally_period() */

boolean is_same_year( JULIAN *new_date, JULIAN *old_date )
{
	int old_year = julian_get_year_number( old_date->current );
	int new_year = julian_get_year_number( new_date->current );

	return ( old_year == new_year );
} /* is_same_year() */

boolean get_seasonally_begin_end_month_day_string(
			char *begin_season_month_day_string,
			char *end_season_month_day_string,
			char *application_name,
			char *period )
{
	char sys_string[ 512 ];
	char *select = "begin_month,begin_day,end_month,end_day";
	char where[ 128 ];
	char buffer[ 128 ];
	char *results;
	int begin_month;
	int begin_day;
	int end_month;
	int end_day;

	sprintf( where, "season = '%s'", period );
	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=seasons			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	piece(	buffer,
		FOLDER_DATA_DELIMITER,
		results,
		0 );
	begin_month = atoi( buffer );

	piece(	buffer,
		FOLDER_DATA_DELIMITER,
		results,
		1 );
	begin_day = atoi( buffer );

	piece(	buffer,
		FOLDER_DATA_DELIMITER,
		results,
		2 );
	end_month = atoi( buffer );

	piece(	buffer,
		FOLDER_DATA_DELIMITER,
		results,
		3 );
	end_day = atoi( buffer );

	sprintf(	begin_season_month_day_string,
			"%.2d-%.2d",
			begin_month,
			begin_day );

	sprintf(	end_season_month_day_string,
			"%.2d-%.2d",
			end_month,
			end_day );

	return 1;
} /* get_seasonally_begin_end_month_day_string() */

int real_time2annual_dynamic_period(
				char *aggregate_statistic,
				JULIAN *old_date,
				JULIAN *new_date,
				char *value_string,
				int period_day,
				int ending_period_day,
				char delimiter,
				boolean output_only )
{
	static STATISTICS_WEIGHTED *statistics_weighted = {0};
	int new_year;
	int old_year;
	int new_day;
	enum within_range_results within_range_results = within_range_unknown;
	static boolean previous_within_range = -1;

	if ( output_only )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
		}
		return 1;
	}

	old_year = julian_get_year_number( old_date->current );
	new_year = julian_get_year_number( new_date->current );

	/* If skipped a year. */
	/* ------------------ */
	if ( new_year > old_year + 1 )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: the input skipped a year. This condition is not supported.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

/*
printf( "getting day number for date = (%s)\n", julian_display_yyyy_mm_dd( new_date->current ) );
*/

	new_day = julian_get_day_number( new_date->current );

	/* Two step process to set within_range_results */
	/* -------------------------------------------- */
	if ( period_day <= ending_period_day )
	{
		if ( new_day >= period_day && new_day <= ending_period_day )
		{
			within_range_results = within_range_first_time;
		}
		else
		{
			within_range_results = outside_range;
		}
	}
	else
	/* if ( period_day > ending_period_day ) */
	{
		if ( new_day >= period_day || new_day <= ending_period_day )
		{
/*
printf( "within_range_first_time: new_day = %d, period_day = %d, ending_period_day = %d\n",
new_day, period_day, ending_period_day );
exit( 0 );
*/
			within_range_results = within_range_first_time;
		}
		else
		{
			within_range_results = outside_range;
		}
	}


	/* Two step process to set within_range_results */
	/* -------------------------------------------- */
	if ( previous_within_range == -1
	&&   within_range_results == within_range_first_time )
	{
		/* stub: within_range_results = within_range_first_time; */
	}
	else
	if ( previous_within_range == 0
	&&   within_range_results == within_range_first_time )
	{
		within_range_results = was_outside_range_now_within;
	}
	else
	if ( previous_within_range == 1
	&&   within_range_results == within_range_first_time )
	{
		within_range_results = within_range_again;
	}

	if ( within_range_results == outside_range )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}
	}
	else
	if ( within_range_results == was_outside_range_now_within )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
		}

		statistics_weighted =
			statistics_weighted_new_statistics_weighted();

		statistics_weighted->buffer = value_string;

		statistics_weighted_accumulate(
			&statistics_weighted->number_array,
			&statistics_weighted->weighted_input_count,
			&statistics_weighted->exceeded_max_numbers,
			&statistics_weighted->count,
			&statistics_weighted->count_non_zero,
			&statistics_weighted->weighted_count,
			&statistics_weighted->sum,
			&statistics_weighted->weighted_sum,
			statistics_weighted->buffer,
			-1 /* statistics_weighted->value_piece */,
			-1 /* statistics_weighted->weight_piece */,
			(char)0 /* delimiter */ );
	}
	else
	if ( within_range_results == within_range_again
	||   within_range_results == within_range_first_time )
	{
		if ( !statistics_weighted )
		{
			statistics_weighted =
				statistics_weighted_new_statistics_weighted();
		}

		statistics_weighted->buffer = value_string;

		statistics_weighted_accumulate(
			&statistics_weighted->number_array,
			&statistics_weighted->weighted_input_count,
			&statistics_weighted->exceeded_max_numbers,
			&statistics_weighted->count,
			&statistics_weighted->count_non_zero,
			&statistics_weighted->weighted_count,
			&statistics_weighted->sum,
			&statistics_weighted->weighted_sum,
			statistics_weighted->buffer,
			-1 /* statistics_weighted->value_piece */,
			-1 /* statistics_weighted->weight_piece */,
			(char)0 /* delimiter */ );
	}
	else
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: unexpected within_range_results = %d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			within_range_results );
		exit( 1 );
	}

	if ( period_day <= ending_period_day )
	{
		if ( new_day >= period_day && new_day <= ending_period_day )
		{
			previous_within_range = 1;
		}
		else
		{
			previous_within_range = 0;
		}
	}

	return 1;
} /* real_time2annual_dynamic_period() */

char *get_season_match_results_string(
			enum season_match_results season_match_results )
{

	if ( season_match_results == different_season )
		return "different_season";
	else
	if ( season_match_results == same_season_different_year )
		return "same_season_different_year";
	else
	if ( season_match_results == same_season_same_year )
		return "same_season_same_year";
	else
		return "unknown";
} /* get_season_match_results_string() */

enum season_match_results get_season_match_results(
				JULIAN *old_date,
				JULIAN *new_date,
				int first_year,
				char *begin_season_month_day_string,
				char *end_season_month_day_string,
				enum season_year_relationship
					season_year_relationship )
{
	char *date_string;
	char *trimmed_date_string;
	int new_year = julian_get_year_number( new_date->current );

	date_string = julian_get_yyyy_mm_dd_string( new_date->current );
	trimmed_date_string = date_string + 5;

	if ( season_year_relationship == season_entire_year )
	{
		if ( old_date->current == new_date->current )
		{
			return same_season_same_year;
		}
		else
		if ( new_year != first_year
		&&   strcmp(	trimmed_date_string,
				begin_season_month_day_string ) == 0 )
		{
			return same_season_different_year;
		}
		else
		{
			return same_season_same_year;
		}
	}
	else
	if ( season_year_relationship == season_within_year )
	{
		if ( strcmp(	trimmed_date_string,
				begin_season_month_day_string ) >= 0
		&&   strcmp(	trimmed_date_string,
				end_season_month_day_string ) <= 0 )
		{
			return same_season_same_year;
		}
		else
		{
			return different_season;
		}
	}
	else
	if ( season_year_relationship == season_crosses_year )
	{
		if ( strcmp(	trimmed_date_string,
				begin_season_month_day_string ) >= 0 )
		{
			return same_season_same_year;
		}

		if ( strcmp(	trimmed_date_string,
				end_season_month_day_string ) <= 0 )
		{
			return same_season_same_year;
		}
	}
	else
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: invalid season_year_relationship of %d\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 season_year_relationship );
		exit( 1 );
	}

	return different_season;

} /* get_season_match_results() */

enum season_year_relationship get_season_year_relationship(
				char *begin_season_month_day_string,
				char *end_season_month_day_string )
{
	int begin_month;
	int end_month;
	char begin_season_date_string[ 16 ];
	char end_season_date_string[ 16 ];
	JULIAN *begin_season_julian;
	JULIAN *end_season_julian;
	enum season_year_relationship season_year_relationship;

	begin_month = atoi( begin_season_month_day_string );
	end_month = atoi( end_season_month_day_string );

	sprintf(	begin_season_date_string,
			"%s-%s",
			LEAP_YEAR,
			begin_season_month_day_string );
	begin_season_julian = julian_new_julian( 0 );
	julian_set_yyyy_mm_dd(	begin_season_julian,
				begin_season_date_string );

	sprintf(	end_season_date_string,
			"%s-%s",
			LEAP_YEAR,
			end_season_month_day_string );
	end_season_julian = julian_new_julian( 0 );
	julian_set_yyyy_mm_dd(	end_season_julian,
				end_season_date_string );

	end_season_julian->current =
		julian_increment_day( end_season_julian->current );

	if (	begin_season_julian->current ==
		end_season_julian->current )
	{
		season_year_relationship = season_entire_year;
	}
	else
	if ( end_month >= begin_month )
	{
		season_year_relationship = season_within_year;
	}
	else
	{
		season_year_relationship = season_crosses_year;
	}

	return season_year_relationship;

} /* get_season_year_relationship() */

