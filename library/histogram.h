/* histogram.h */
/* -------------------------------------------------------------------- */
/* This is the appaserver histogram ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#define HISTOGRAM_MINIMUM_SEGMENTS	10
#define HISTOGRAM_MAXIMUM_SEGMENTS	30

#include "list.h"
#include "boolean.h"

typedef struct {
	double bottom_bound;
	double top_bound;
	int count;
	boolean soft_delete;
} HISTOGRAM_SEGMENT;

typedef struct {
	int number_of_segments;
	LIST *segment_list;
	LIST *number_double_list;
} HISTOGRAM;

HISTOGRAM *histogram_new_histogram(	void );
HISTOGRAM_SEGMENT *histogram_new_segment(
					void );
void histogram_free(			HISTOGRAM * );
void histogram_set_number_double(	LIST *number_double_list,
					double number_double );
LIST *histogram_get_segment_list(	int *number_of_segments_pointer,
					LIST *number_double_list );
void histogram_segment_list_display(
					LIST *segment_list );

void histogram_count(			LIST *segment_list,
					LIST *number_double_list );
void histogram_soft_delete_empty_edges(	int *number_of_segments,
					LIST *segment_list );
double histogram_get_lowest_top_bound(
					LIST *segment_list );
double histogram_get_highest_top_bound(
					LIST *segment_list );
#endif
