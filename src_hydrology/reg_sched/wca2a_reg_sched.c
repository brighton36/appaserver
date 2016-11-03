/* wca2a_print.c */
/* ------------- */

#include <stdio.h>
#include <stdlib.h>
#include "column.h"
#include "regulation_schedule.h"

REGULATION_SCHEDULE *regulation_schedule_1_new( void );

int main( void )
{
	REGULATION_SCHEDULE *regulation_schedule;
	int i;

	regulation_schedule = regulation_schedule_1_new();

	printf( "day\tB\t\tC\t\tD\t\tE\t\tE1\n" );
	for( i = 1; i <= 366; i++ )
	{
	 	printf( "%d\t%lf\t%lf\t%lf\t%lf\t%lf\n",
		i,
		regulation_schedule->zone_b_array[ i ]->upper_boundry,
		regulation_schedule->zone_c_array[ i ]->upper_boundry,
		regulation_schedule->zone_d_array[ i ]->upper_boundry,
		regulation_schedule->zone_e_array[ i ]->upper_boundry,
		regulation_schedule->zone_e1_array[ i ]->upper_boundry );
	}

	return 0;
} /* main() */

REGULATION_SCHEDULE *regulation_schedule_1_new( void )
{
	REGULATION_SCHEDULE *regulation_schedule;
	LIST *zone_e1_inflection_list;
	LIST *zone_e_inflection_list;
	LIST *zone_d_inflection_list;
	LIST *zone_c_inflection_list;
	LIST *zone_b_inflection_list;

	zone_e1_inflection_list = list_new();
	zone_inflection_append( zone_e1_inflection_list, 31, 10.25 );
	zone_inflection_append( zone_e1_inflection_list, 188, 9.25 );
	zone_inflection_append( zone_e1_inflection_list, 196, 9.30 );

	zone_e_inflection_list = list_new();
	zone_inflection_append( zone_e_inflection_list, 1, 10.25 );
	zone_inflection_append( zone_e_inflection_list, 31, 10.25 );
	zone_inflection_append( zone_e_inflection_list, 32, 9.75 );
	zone_inflection_append( zone_e_inflection_list, 196, 8.72 );
	zone_inflection_append( zone_e_inflection_list, 197, 9.30 );
	zone_inflection_append( zone_e_inflection_list, 340, 10.25 );
	zone_inflection_append( zone_e_inflection_list, 366, 10.25 );

	zone_d_inflection_list = list_new();
	zone_inflection_append( zone_d_inflection_list, 1, 10.5 );
	zone_inflection_append( zone_d_inflection_list, 31, 10.25 );
	zone_inflection_append( zone_d_inflection_list, 32, 0.0 );
	zone_inflection_append( zone_d_inflection_list, 151, 0.0 );
	zone_inflection_append( zone_d_inflection_list, 152, 9.5 );
	zone_inflection_append( zone_d_inflection_list, 305, 10.5 );
	zone_inflection_append( zone_d_inflection_list, 366, 10.5 );

	zone_c_inflection_list = list_new();
	zone_inflection_append( zone_c_inflection_list, 1, 10.75 );
	zone_inflection_append( zone_c_inflection_list, 34, 10.75 );
	zone_inflection_append( zone_c_inflection_list, 151, 10.0 );
	zone_inflection_append( zone_c_inflection_list, 152, 0.0 );
	zone_inflection_append( zone_c_inflection_list, 304, 0.0 );
	zone_inflection_append( zone_c_inflection_list, 305, 10.75 );
	zone_inflection_append( zone_c_inflection_list, 366, 10.75 );

	zone_b_inflection_list = list_new();
	zone_inflection_append( zone_b_inflection_list, 152, 10.0 );
	zone_inflection_append( zone_b_inflection_list, 267, 10.75 );
	zone_inflection_append( zone_b_inflection_list, 366, 10.75 );

	regulation_schedule = regulation_schedule_new(
			zone_e1_inflection_list,
			zone_e_inflection_list,
			(LIST *)0 /* zone_d0_inflection_list */,
			zone_d_inflection_list,
			(LIST *)0 /* zone_c0_inflection_list */,
			zone_c_inflection_list,
			zone_b_inflection_list,
			(LIST *)0 /* zone_a_inflection_list */ );

	return regulation_schedule;
} /* regulation_schedule_1_new() */
