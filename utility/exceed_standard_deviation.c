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

/* Constants */
/* --------- */
#define DEFAULT_DELIMITER	'^'

/* Prototypes */
/* ---------- */
void exceed_standard_deviation_key_value_file(
			char *key_value_file,
			char delimiter,
			double minimum_value,
			double average,
			double maximum_value );

void exceed_standard_deviation_stdin(
			double *minimum_value,
			double *maximum_value,
			double *average,
			double number_of_standard_deviations );

void exceed_standard_deviation_output(
			double minimum_value,
			double maximum_value,
			double average,
			char *key_string,
			double value,
			char delimiter,
			char *key_value_file );

void exceed_standard_deviation_stdout(
			char *key_string,
			char delimiter,
			double value,
			double minimum_value,
			double average,
			double maximum_value );

int main( int argc, char **argv )
{
	char delimiter = DEFAULT_DELIMITER;
	double number_of_standard_deviations;
	double value;
	char *key_string;
	char *key_value_file;
	double minimum_value = 0.0;
	double average = 0.0;
	double maximum_value = 0.0;

	if ( argc != 6 )
	{
		fprintf(stderr,
"Usage: %s number_of_standard_deviations key_string value delimiter key_value_file\n",
			argv[ 0 ] );

		fprintf(stderr,
"Note: either [key_string value] or [key_value_file]. The key_value_file\n"
"allows for multiple checks.\n" );

		exit( 1 );
	}

	number_of_standard_deviations = atof( argv[ 1 ] );
	key_string = argv[ 2 ];
	value = atof( argv[ 3 ] );

	if ( strcmp( argv[ 4 ], "delimiter" ) != 0 )
		delimiter = *argv[ 4 ];

	key_value_file = argv[ 5 ];

	exceed_standard_deviation_stdin(
		&minimum_value,
		&maximum_value,
		&average,
		number_of_standard_deviations );

	exceed_standard_deviation_output(
			minimum_value,
			maximum_value,
			average,
			key_string,
			value,
			delimiter,
			key_value_file );

	return 0;

} /* main() */

void exceed_standard_deviation_stdin(
			double *minimum_value,
			double *maximum_value,
			double *average,
			double number_of_standard_deviations )
{
	STATISTICS_WEIGHTED *statistics_weighted;
	char input_buffer[ 1024 ];
	double standard_deviation_radius;

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

	*minimum_value = 0.0;
	*maximum_value = 0.0;
	*average = 0.0;

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
			-1 /* statistics_weighted->value_piece */,
			-1 /* statistics_weighted->weight_piece */,
			(char)0 /* delimiter */ );

	}

	statistics_weighted->average =
		statistics_weighted_compute_average(
				statistics_weighted->weighted_count,
				statistics_weighted->weighted_sum );

/*
	statistics_weighted_sort_number_array(
			&statistics_weighted->number_array );
*/

	statistics_weighted->standard_deviation =
			statistics_weighted_compute_standard_deviation(
				&statistics_weighted->coefficient_of_variation,
				&statistics_weighted->standard_error_of_mean,
				statistics_weighted->average,
				&statistics_weighted->number_array );

	standard_deviation_radius =
		statistics_weighted->standard_deviation *
		number_of_standard_deviations;

	*minimum_value =
		statistics_weighted->average -
		standard_deviation_radius;

	*maximum_value =
		statistics_weighted->average +
		standard_deviation_radius;

	*average = statistics_weighted->average;

} /* exceed_standard_deviation_stdin() */

void exceed_standard_deviation_output(
			double minimum_value,
			double maximum_value,
			double average,
			char *key_string,
			double value,
			char delimiter,
			char *key_value_file )
{
	if ( !*key_value_file
	||   strcmp( key_value_file, "key_value_file" ) == 0 )
	{
		exceed_standard_deviation_stdout(
			key_string,
			delimiter,
			value,
			minimum_value,
			average,
			maximum_value );
	}
	else
	{
		exceed_standard_deviation_key_value_file(
			key_value_file,
			delimiter,
			minimum_value,
			average,
			maximum_value );
	}

} /* exceed_standard_deviation_output() */

void exceed_standard_deviation_stdout(
			char *key_string,
			char delimiter,
			double value,
			double minimum_value,
			double average,
			double maximum_value )
{
	/* All good */
	/* -------- */
	if ( minimum_value <= value && value <= maximum_value ) return;

	if ( value < minimum_value )
	{
		printf( "%s%c%.3lf%ctoo_low%c",
			key_string,
			delimiter,
			value,
			delimiter,
			delimiter );
	}
	else
	if ( value > maximum_value )
	{
		printf( "%s%c%.3lf%ctoo_high%c",
			key_string,
			delimiter,
			value,
			delimiter,
			delimiter );
	}

	printf( "min=%.2lf%cavg=%.2lf%cmax=%.2lf\n",
			minimum_value,
			delimiter,
			average,
			delimiter,
			maximum_value );

} /* exceed_standard_deviation_stdout() */

void exceed_standard_deviation_key_value_file(
			char *key_value_file,
			char delimiter,
			double minimum_value,
			double average,
			double maximum_value )
{
	FILE *input_file;
	char input_buffer[ 1024 ];
	char key_string[ 512 ];
	char value[ 512 ];

	if ( ! ( input_file = fopen( key_value_file, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 key_value_file );
	}

	while( timlib_get_line( input_buffer, input_file, 1024 ) )
	{
		piece( key_string, delimiter, input_buffer, 0 );

		if ( !piece( value, delimiter, input_buffer, 1 ) )
			continue;

		exceed_standard_deviation_stdout(
			key_string,
			delimiter,
			atof( value ),
			minimum_value,
			average,
			maximum_value );
	}

	fclose( input_file );

} /* exceed_standard_deviation_key_value_file() */

