/* src_hydrology/annual_comparison.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdlib.h>
#include "annual_comparison.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"
#include "hydrology_library.h"
#include "aggregate_statistic.h"
#include "date.h"
#include "list.h"
#include "list_usage.h"

/* Prototypes */
/* ---------- */
boolean year_okay(		int year,
				LIST *number_list );

void parse_input_buffer(	char *measurement_date,
				char *measurement_value,
				char *input_buffer );

/* Operations */
/* ---------- */
ANNUAL_COMPARISON *annual_comparison_new(
				char *begin_month_day,
				char *end_month_day,
				int begin_year,
				int end_year )
{
	ANNUAL_COMPARISON *annual_comparison;

	if ( ! ( annual_comparison =
			(ANNUAL_COMPARISON *)
			calloc( 1, sizeof( ANNUAL_COMPARISON ) ) ) )
	{
		fprintf( stderr,
			 "Memory allocation error in %s/%s()/%d\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	annual_comparison->begin_month_day = begin_month_day;
	annual_comparison->end_month_day = end_month_day;
	annual_comparison->begin_year = begin_year;
	annual_comparison->end_year = end_year;

	annual_comparison->day_range =
		annual_comparison_get_day_range(
				&annual_comparison->begin_date,
				begin_month_day,
				end_month_day );

	if ( !annual_comparison->day_range )
		return (ANNUAL_COMPARISON *)0;

	annual_comparison->year_range = (end_year + 1) - begin_year;

	annual_comparison->value_array =
		annual_comparison_value_array_new(
			annual_comparison->year_range,
			annual_comparison->day_range );

	return annual_comparison;
} /* annual_comparison_new() */

int annual_comparison_get_day_range(
				DATE **begin_date,
				char *begin_month_day,
				char *end_month_day )
{
	DATE *end_date;
	char date_string[ 16 ];
	int day_range;

	sprintf(date_string,
		"%d-%s",
		ANNUAL_COMPARISON_DEFAULT_YEAR,
		begin_month_day );
	*begin_date = date_yyyy_mm_dd_new( strdup( date_string ) );

	sprintf(date_string,
		"%d-%s",
		ANNUAL_COMPARISON_DEFAULT_YEAR,
		end_month_day );
	end_date = date_yyyy_mm_dd_new( strdup( date_string ) );

	day_range = date_subtract_days( end_date, *begin_date ) + 1;

	if ( day_range < 0 )
		return 0;
	else
		return day_range;

} /* annual_comparison_get_day_range() */

char ***annual_comparison_value_array_new(
			int year_range,
			int day_range )
{
	char ***value_array;
	int day;

	value_array = (char ***) calloc( day_range, sizeof( void ** ) );
	if ( !value_array )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	for ( day = 0; day < day_range; day++ )
	{
		value_array[ day ] =
			(char **) calloc( year_range, sizeof( char * ) );
		if ( !value_array[ day ] )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	}
	return value_array;
} /* annual_comparison_value_array_new() */

boolean annual_comparison_set_value(
			char ***value_array,
			int *day_array_offset,
			int *year_array_offset,
			DATE *begin_date,
			int begin_year,
			int day_range,
			int year_range,
			char *date_yyyymmdd,
			char *value_string )
{
	int year;

	if ( !value_string )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: value_string is null. Perhaps memory allocation error in strdup() function.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	*day_array_offset =
		annual_comparison_get_day_array_offset(
			begin_date,
			date_yyyymmdd );

	if ( *day_array_offset < 0 || *day_array_offset >= day_range )
	{
		return 0;
	}

	year = atoi( date_yyyymmdd );
	*year_array_offset = year - begin_year;

	if ( *year_array_offset < 0 || *year_array_offset >= year_range )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid year_array_offset = %d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			*year_array_offset );
		exit( 1 );
	}

	value_array[ *day_array_offset ][ *year_array_offset ] = value_string;

	return 1;
} /* annual_comparison_set_value() */

int annual_comparison_get_day_array_offset(
			DATE *begin_date,
			char *date_yyyymmdd )
{
	char date_string[ 16 ];
	char end_month_day[ 16 ];
	DATE *local_date;
	DATE *end_date;
	int array_offset;

	local_date = date_yyyy_mm_dd_new( date_yyyymmdd );

	sprintf( end_month_day,
		 "%d-%d",
		 date_get_month_number( local_date ),
		 date_get_month_day_number( local_date ) );

	date_free( local_date );

	sprintf(date_string,
		"%d-%s",
		ANNUAL_COMPARISON_DEFAULT_YEAR,
		end_month_day );
	end_date = date_yyyy_mm_dd_new( date_string );

	array_offset = date_subtract_days( end_date, begin_date );

	date_free( end_date );
	return array_offset;
} /* annual_comparison_get_day_array_offset() */

void annual_comparison_output(
				char ***value_array,
				int day_range,
				int year_range,
				DATE *begin_date,
				int begin_year,
				LIST *number_list )
{
	int day;
	int year;

	for( day = 0; day < day_range; day++ )
	{
		printf( ".day %d-%d\n", 
			date_get_month_number( begin_date ),
			date_get_month_day_number( begin_date ) );
		fflush( stdout );
		date_increment( begin_date );

		for( year = 0; year < year_range; year++ )
		{
			if ( !year_okay( begin_year + year, number_list ) )
				continue;

			if ( value_array[ day ][ year ] )
			{
				printf( ".year %d,%s\n",
					begin_year + year,
					value_array[ day ][ year ] );
			}
			else
			{
				printf( ".year %d,\n",
					begin_year + year );
			}
		}
	}
} /* annual_comparison_output() */

void parse_input_buffer(	char *measurement_date,
				char *measurement_value,
				char *input_buffer )
{
	piece(	measurement_date,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		0 );

	piece(	measurement_value,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		2 );
}

int main( int argc, char **argv )
{
	ANNUAL_COMPARISON *annual_comparison;
	char *application_name;
	char *station_name;
	char *datatype_name;
	char *begin_month_day;
	char *end_month_day;
	char *year_expression;
	char *begin_year = {0};
	char *end_year = {0};
	char *aggregate_statistic_string;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char aggregation_process[ 1024 ];
	char measurement_date[ 16 ];
	char measurement_value[ 16 ];
	char where_clause[ 1024 ];
	enum aggregate_statistic aggregate_statistic;
	char end_date[ 16 ];
	LIST *number_list;
	char *select = "measurement_date,measurement_time,measurement_value";

	output_starting_argv_stderr( argc, argv );

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s application station datatype begin_day_month end_day_month year_expression aggregate_statistic\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	station_name = argv[ 2 ];
	datatype_name = argv[ 3 ];
	begin_month_day = argv[ 4 ];
	end_month_day = argv[ 5 ];
	year_expression = argv[ 6 ];
	aggregate_statistic_string = argv[ 7 ];

	hydrology_library_get_begin_end_year(
			&begin_year,
			&end_year,
			year_expression );

	number_list =
		list_usage_expression2number_list(
			year_expression );

	if ( ! ( annual_comparison =
			annual_comparison_new(
				begin_month_day,
				end_month_day,
				atoi( begin_year ),
				atoi( end_year ) ) ) )
	{
		exit( 0 );
	}

	sprintf( where_clause,
		 "station = '%s' and				"
		 "datatype = '%s' and				"
		 "measurement_date between '%s-%s' and '%s-%s'	",
		 station_name,
		 datatype_name,
		 begin_year,
		 begin_month_day,
		 end_year,
		 end_month_day );

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
			aggregate_statistic_string,
			daily );

	aggregate_statistic =
		based_on_datatype_get_aggregate_statistic(
			application_name,
			datatype_name,
			aggregate_statistic );

	sprintf( end_date, "%s-%s", end_year, end_month_day );

	sprintf( aggregation_process, 
		 "real_time2aggregate_value.e %s 0 1 2 '%c' daily n %s",
		 aggregate_statistic_get_string( aggregate_statistic ),
		 FOLDER_DATA_DELIMITER,
		 end_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=%s		 "
		 "			folder=measurement	 "
		 "			where=\"%s\"		|"
		 "%s						 ",
		 application_name,
		 select,
		 where_clause,
		 aggregation_process );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		parse_input_buffer(
				measurement_date,
				measurement_value,
				input_buffer );

		annual_comparison_set_value(
			annual_comparison->value_array,
			&annual_comparison->day_array_offset,
			&annual_comparison->year_array_offset,
			annual_comparison->begin_date,
			annual_comparison->begin_year,
			annual_comparison->day_range,
			annual_comparison->year_range,
			measurement_date,
			strdup( measurement_value ) );
	}

	pclose( input_pipe );

	annual_comparison_output(
			annual_comparison->value_array,
			annual_comparison->day_range,
			annual_comparison->year_range,
			annual_comparison->begin_date,
			annual_comparison->begin_year,
			number_list );

	return 0;
} /* main() */

boolean year_okay(	int year,
			LIST *number_list )
{
	int *number_ptr;

	if ( !list_rewind( number_list ) ) return 0;

	do {
		number_ptr = list_get_pointer( number_list );

		if ( *number_ptr == year ) return 1;

	} while( list_next( number_list ) );

	return 0;

} /* year_okay() */

