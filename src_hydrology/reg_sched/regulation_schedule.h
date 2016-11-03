#ifndef REGULATION_SCHEDULE_H
#define REGULATION_SCHEDULE_H

#include <stdio.h>
#include "list.h"

#define DAYS_PER_YEAR		366

typedef struct
{
	int date_offset_one_based;

	/* Note: for no-inflection-zone, use max float for value */
	/* ----------------------------------------------------- */
	double value;
} INFLECTION;

typedef struct
{
	double upper_boundry;
} ZONE;

typedef struct
{
	ZONE **zone_a_array;
	ZONE **zone_b_array;
	ZONE **zone_c_array;
	ZONE **zone_c0_array;
	ZONE **zone_d_array;
	ZONE **zone_d0_array;
	ZONE **zone_e_array;
	ZONE **zone_e1_array;

} REGULATION_SCHEDULE;

/* Operations */
/* ---------- */
REGULATION_SCHEDULE *regulation_schedule_new(
			LIST *zone_e1_inflection_list,
			LIST *zone_e_inflection_list,
			LIST *zone_d0_inflection_list,
			LIST *zone_d_inflection_list,
			LIST *zone_c0_inflection_list,
			LIST *zone_c_inflection_list,
			LIST *zone_b_inflection_list,
			LIST *zone_a_inflection_list );
char *regulation_schedule_get_zone_string(
			REGULATION_SCHEDULE *regulation_schedule,
			int date_offset_one_based );
ZONE *regulation_schedule_allocate_zone_array( void );

ZONE **regulation_schedule_get_zone_array( LIST *zone_inflection_list );
void regulation_schedule_interpolate_double_array(
				double *double_array,
				LIST *inflection_list );
char *regulation_schedule_display( 
			REGULATION_SCHEDULE *regulation_schedule );
char *regulation_schedule_zone_display(
			char *buffer,
			ZONE **zone_array, 
			char *zone_name );
void zone_inflection_append(
			LIST *zone_inflection_list,
			int date_offset_one_based,
			double value );
char *regulation_schedule_get(	
			REGULATION_SCHEDULE *regulation_schedule,
			int date_offset_one_based,
			double value );
double zone_array_upper_boundry_get(
			ZONE **zone_array,
			int date_offset_one_based );
void regulation_schedule_output(
			REGULATION_SCHEDULE *regulation_schedule,
			int date_offset_one_based,
			double value );
#endif
