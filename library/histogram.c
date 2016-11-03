/* histogram.c */
/* -------------------------------------------------------------------- */
/* This is the appaserver histogram ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
/* #include <values.h> */
#include "timlib.h"
#include "histogram.h"

HISTOGRAM *histogram_new_histogram( void )
{
	HISTOGRAM *histogram = (HISTOGRAM *)calloc( 1, sizeof( HISTOGRAM ) );
	if ( !histogram )
	{
		fprintf(stderr,
			"%s/%s() cannot allocate memory\n",
			__FILE__,
			__FUNCTION__ );
		exit( 1 );
	}
	histogram->number_double_list = list_new_list();
	return histogram;
}

void histogram_set_number_double(	LIST *number_double_list,
					double number_double )
{
	double *double_pointer = calloc( 1, sizeof( double ) );

	if ( !double_pointer )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}
	*double_pointer = number_double;
	list_append_pointer( number_double_list, double_pointer );
} /* histogram_set_number_double() */

void histogram_free( HISTOGRAM *s )
{
	free( s );
}

HISTOGRAM_SEGMENT *histogram_new_segment( void )
{
	HISTOGRAM_SEGMENT *segment;

	segment =	(HISTOGRAM_SEGMENT *)
				calloc( 1, sizeof( HISTOGRAM_SEGMENT ) );
	return segment;
} /* histogram_new_segment() */

void histogram_segment_list_display( LIST *segment_list )
{
	HISTOGRAM_SEGMENT *segment;

	if ( list_rewind( segment_list ) )
	{
		do {
			segment = list_get_pointer( segment_list );
			fprintf(stderr,
				"bottom_bound = %.2lf\n", 
				segment->bottom_bound );
			fprintf(stderr,
				"count = %d\n",
				segment->count );
			fprintf(stderr,
				"top_bound = %.2lf\n", 
				segment->top_bound );
		} while( list_next( segment_list ) );
	}

} /* histogram_segment_list_display() */

LIST *histogram_get_segment_list(	int *number_of_segments_pointer,
					LIST *number_double_list )
{
	int number_of_segments;
	double *double_pointer;
	LIST *segment_list = list_new_list();
	int i;
	double lowest_bound;
	double highest_bound;
	HISTOGRAM_SEGMENT *segment;
	double segment_bound_increment;
/*
	double lowest_number = MAXDOUBLE;
	double highest_number = MINDOUBLE;
	double lowest_number = DBL_MAX;
	double highest_number = DBL_MIN;
*/
	double lowest_number = DOUBLE_MAXIMUM;
	double highest_number = DOUBLE_MINIMUM;

	if ( list_rewind( number_double_list ) )
	{
		do {
			double_pointer =
				list_get_pointer( number_double_list );

			if ( *double_pointer < lowest_number )
				lowest_number = *double_pointer;

			if ( *double_pointer > highest_number )
				highest_number = *double_pointer;
		} while( list_next( number_double_list ) );
	}

	number_of_segments =
		(int)(highest_number + 1) - (int)(lowest_number);

	if ( list_length( number_double_list ) < 50 )
		number_of_segments = HISTOGRAM_MINIMUM_SEGMENTS;
	else
	if ( number_of_segments < HISTOGRAM_MINIMUM_SEGMENTS )
		number_of_segments = HISTOGRAM_MINIMUM_SEGMENTS;
	else
	if ( number_of_segments > HISTOGRAM_MAXIMUM_SEGMENTS )
		number_of_segments = HISTOGRAM_MAXIMUM_SEGMENTS;

	lowest_bound = (int)(lowest_number - 0.99999999);
	highest_bound = (int)(highest_number + 0.99999999);

	segment_bound_increment =
		(highest_bound - lowest_bound) / number_of_segments;

	for( i = 0 ; i < number_of_segments; i++ )
	{
		segment = histogram_new_segment();
		segment->bottom_bound = lowest_bound;
		lowest_bound += segment_bound_increment;
		segment->top_bound = lowest_bound;
		list_append_pointer( segment_list, segment );
	}

	*number_of_segments_pointer = number_of_segments;
	return segment_list;
} /* histogram_get_segment_list() */

void histogram_count(			LIST *segment_list,
					LIST *number_double_list )
{
	double *double_pointer;
	HISTOGRAM_SEGMENT *segment;

	if ( !list_length( segment_list ) ) return;

	if ( !list_rewind( number_double_list ) ) return;

	do {
		double_pointer = list_get_pointer( number_double_list );

		list_rewind( segment_list );
		do {
			segment = list_get_pointer( segment_list );

			if ( *double_pointer >= segment->bottom_bound &&
			     *double_pointer < segment->top_bound )
			{
				segment->count++;
				break;
			}
		} while( list_next( segment_list ) );
	} while( list_next( number_double_list ) );
} /* histogram_count() */

void histogram_soft_delete_empty_edges( int *number_of_segments,
					LIST *segment_list )
{
	HISTOGRAM_SEGMENT *segment;
	int local_number_of_segments = *number_of_segments;

	if ( !list_rewind( segment_list ) ) return;

	do {
		segment = list_get_pointer( segment_list );

		if ( segment->count )
			break;
		else
		{
			segment->soft_delete = 1;
			local_number_of_segments--;
		}
	} while( list_next( segment_list ) );

	if ( !list_go_tail( segment_list ) ) return;

	do {
		segment = list_get_pointer( segment_list );

		if ( segment->count )
			break;
		else
		{
			segment->soft_delete = 1;
			local_number_of_segments--;
		}
	} while( list_previous( segment_list ) );

	*number_of_segments = local_number_of_segments;
} /* histogram_soft_delete_empty_edges() */

double histogram_get_lowest_top_bound( LIST *segment_list )
{
	HISTOGRAM_SEGMENT *segment;

	segment = list_get_first_pointer( segment_list );
	return segment->top_bound;
} /* histogram_get_lowest_top_bound() */

double histogram_get_highest_top_bound( LIST *segment_list )
{
	HISTOGRAM_SEGMENT *segment;

	segment = list_get_last_pointer( segment_list );
	return segment->top_bound;
} /* histogram_get_highest_top_bound() */

int histogram_get_length( LIST *segment_list )
{
	HISTOGRAM_SEGMENT *segment;
	int length = 0;

	if ( !list_rewind( segment_list ) ) return 0;

	do {
		segment = list_get_pointer( segment_list );

		if ( !segment->soft_delete ) length++;
	} while( list_next( segment_list ) );

	return length;
} /* histogram_get_length() */

