/* $APPASERVER_HOME/utility/exceed_standard_deviation.c	*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "statistics_weighted.h"

#define DEFAULT_DELIMITER	'^'

void exceed_standard_deviation_stdin(
			double number_of_standard_deviations,
			int piece_offset,
			char delimiter,
			char *key_string,
			double value );

int main( int argc, char **argv )
{
	char *key_string;
	char delimiter = DEFAULT_DELIMITER;
	double number_of_standard_deviations;
	int piece_offset = 0;
	double value;

	if ( argc < 4 )
	{
		fprintf(stderr,
"Usage: %s key_string number_of_standard_deviations value [piece] [delimiter]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	key_string = argv[ 1 ];
	number_of_standard_deviations = atof( argv[ 2 ] );
	value = atof( argv[ 3 ] );

	if ( argc >= 5 )
		piece_offset = atoi( argv[ 4 ] );

	if ( argc >= 6 )
		delimiter = *argv[ 5 ];

	exceed_standard_deviation_stdin(
		number_of_standard_deviations,
		piece_offset,
		delimiter,
		key_string,
		value );

	return 0;

} /* main() */

void exceed_standard_deviation_stdin(
			double number_of_standard_deviations,
			int piece_offset,
			char delimiter,
			char *key_string,
			double value )
{
	STATISTICS_WEIGHTED *statistics_weighted;
	char input_buffer[ 1024 ];
	double minimum_value;
	double maximum_value;

	if ( ! ( statistics_weighted =
			statistics_weighted_new() ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: statistics_weighted_new() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	while( timlib_get_line( input_buffer, stdin, 1024 ) )
	{
		if ( !*input_buffer ) continue;
		if ( *input_buffer == '#' ) continue;

		statistics_weighted->buffer = input_buffer;

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
			( piece_offset ) ? piece_offset : -1
				/* statistics_weighted->value_piece */,
			-1 /* statistics_weighted->weight_piece */,
			delimiter );

	}

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

	minimum_value = statistics_weighted->average -
				( statistics_weighted->standard_deviation *
				  number_of_standard_deviations );

	maximum_value = statistics_weighted->average +
				( statistics_weighted->standard_deviation *
				  number_of_standard_deviations );

	if ( value < minimum_value || value > maximum_value )
	{
		printf( "%s^%.3lf\n",
			key_string,
			value );
	}
/*
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

	statistics_weighted_free( statistics_weighted );
*/

	return;

} /* exceed_standard_deviation_stdin() */

