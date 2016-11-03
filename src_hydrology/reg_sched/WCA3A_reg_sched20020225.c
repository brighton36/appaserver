/* WCA3A_reg_sched_20020225.c */
/* -------------------------- */

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

	printf( "day\tB\t\tC\t\tD\t\tE\t\tE1\t\tC0\t\tD0\n" );
	for( i = 1; i <= 366; i++ )
	{
	 	printf( "%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n",
		i,
		regulation_schedule->zone_b_array[ i ]->upper_boundry,
		regulation_schedule->zone_c_array[ i ]->upper_boundry,
		regulation_schedule->zone_d_array[ i ]->upper_boundry,
		regulation_schedule->zone_e_array[ i ]->upper_boundry,
		regulation_schedule->zone_e1_array[ i ]->upper_boundry,
		regulation_schedule->zone_c0_array[ i ]->upper_boundry,
		regulation_schedule->zone_d0_array[ i ]->upper_boundry );
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
	LIST *zone_c0_inflection_list;
	LIST *zone_d0_inflection_list;

	zone_e1_inflection_list = list_new();
	zone_inflection_append( zone_e1_inflection_list, 31, 10.25 );
	zone_inflection_append( zone_e1_inflection_list, 32, 9.75 );
	zone_inflection_append( zone_e1_inflection_list, 197, 8.75 );
	zone_inflection_append( zone_e1_inflection_list, 198, 9.30 );

	zone_e_inflection_list = list_new();
	zone_inflection_append( zone_e_inflection_list, 147, 9.5 );
	zone_inflection_append( zone_e_inflection_list, 189, 9.25 );
	zone_inflection_append( zone_e_inflection_list, 336, 10.25 );
	zone_inflection_append( zone_e_inflection_list, 366, 10.25 );

	zone_d_inflection_list = list_new();
	zone_inflection_append( zone_d_inflection_list, 147, 9.5 );
	zone_inflection_append( zone_d_inflection_list, 301, 10.5 );
	zone_inflection_append( zone_d_inflection_list, 366, 10.5 );

	zone_c_inflection_list = list_new();
	zone_inflection_append( zone_c_inflection_list, 1, 10.75 );
	zone_inflection_append( zone_c_inflection_list, 42, 10.75 );
	zone_inflection_append( zone_c_inflection_list, 148, 10.0 );
	zone_inflection_append( zone_c_inflection_list, 148, 9.5 );

	zone_b_inflection_list = list_new();
	zone_inflection_append( zone_b_inflection_list, 147, 9.5 );
	zone_inflection_append( zone_b_inflection_list, 147, 10.0 );
	zone_inflection_append( zone_b_inflection_list, 266, 10.75 );
	zone_inflection_append( zone_b_inflection_list, 301, 10.75 );
	zone_inflection_append( zone_b_inflection_list, 301, 10.5 );

	zone_c0_inflection_list = list_new();
	zone_inflection_append( zone_c0_inflection_list, 301, 10.5 );
        zone_inflection_append( zone_c0_inflection_list, 301, 10.75 );
        zone_inflection_append( zone_c0_inflection_list, 366, 10.75 );

	zone_d0_inflection_list = list_new();
	zone_inflection_append( zone_d0_inflection_list, 1, 10.25 );
	zone_inflection_append( zone_d0_inflection_list, 41, 10.25 );
	zone_inflection_append( zone_d0_inflection_list, 42, 10.25 );

	regulation_schedule = regulation_schedule_new(
			zone_e1_inflection_list,
			zone_e_inflection_list,
			zone_d0_inflection_list,
			zone_d_inflection_list,
			zone_c0_inflection_list,
			zone_c_inflection_list,
			zone_b_inflection_list, 
			(LIST *)0 /* zone_a_inflection_list */ );
 
	return regulation_schedule;
} /* regulation_schedule_1_new() */
