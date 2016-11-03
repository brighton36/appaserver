#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "regulation_schedule.h"

REGULATION_SCHEDULE *regulation_schedule_new(
			LIST *zone_e1_inflection_list,
                        LIST *zone_e_inflection_list,
                        LIST *zone_d0_inflection_list,
			LIST *zone_d_inflection_list,
                        LIST *zone_c0_inflection_list,
			LIST *zone_c_inflection_list,
			LIST *zone_b_inflection_list, 
			LIST *zone_a_inflection_list )

{
	REGULATION_SCHEDULE *a = (REGULATION_SCHEDULE *)
			calloc( 1, sizeof( REGULATION_SCHEDULE ) );

	a->zone_a_array = regulation_schedule_get_zone_array(
			zone_a_inflection_list );

	a->zone_b_array = regulation_schedule_get_zone_array(
			zone_b_inflection_list );

	a->zone_c_array = regulation_schedule_get_zone_array(
			zone_c_inflection_list );

	a->zone_c0_array = regulation_schedule_get_zone_array(
				zone_c0_inflection_list );

	a->zone_d_array = regulation_schedule_get_zone_array(
			zone_d_inflection_list );

	a->zone_d0_array = regulation_schedule_get_zone_array(
				zone_d0_inflection_list );

	a->zone_e_array = regulation_schedule_get_zone_array(
				zone_e_inflection_list );

	a->zone_e1_array = regulation_schedule_get_zone_array(
				zone_e1_inflection_list );

	return a;
} /* regulation_schedule_new() */

ZONE **regulation_schedule_get_zone_array( LIST *inflection_list )
{
	ZONE **zone_array;
	double double_array[ DAYS_PER_YEAR + 1 ] = {99999.99};
	int i;

	zone_array = (ZONE **)calloc( DAYS_PER_YEAR + 1, sizeof( ZONE *) );

	if ( !list_length( inflection_list ) )
	{
		return (ZONE **)0;
/*
		fprintf( stderr, "ERROR: got empty zone list.\n" );
		exit( 1 );
*/
	}

	regulation_schedule_interpolate_double_array(
				double_array,
				inflection_list );

	for( i = 1; i <= DAYS_PER_YEAR; i++ )
	{
		zone_array[ i ] = (ZONE *)calloc( 1, sizeof( ZONE ) );
		zone_array[ i ]->upper_boundry = double_array[ i ];
	}

	return zone_array;
} /* regulation_schedule_get_zone_array() */

void regulation_schedule_interpolate_double_array(
				double *double_array,
				LIST *inflection_list )
{
	INFLECTION *inflection;
	int x1, x2;
	double y1, y2;

	if ( list_rewind( inflection_list ) )
	{
		/* Get the first one */
		/* ----------------- */
		inflection = (INFLECTION *) list_get( inflection_list );
		x1 = inflection->date_offset_one_based;
		y1 = inflection->value;
	
		double_array[ x1 ] = y1;

		list_next( inflection_list );

		if ( !at_end( inflection_list ) )
		{
			do {
				inflection = (INFLECTION *) 
					list_get( inflection_list );
	
				x2 = inflection->date_offset_one_based;
				y2 = inflection->value;

				double_array[ x2 ] = y2;

				linear_interpolate(
						double_array,
						x1, x2 );

				x1 = x2;

			} while( list_next( inflection_list ) );
		}
	}

} /* regulation_schedule_interpolate_double_array() */

char *regulation_schedule_display( REGULATION_SCHEDULE *regulation_schedule )
{
	char buffer[ 65536 ];
	char *buffer_ptr = buffer;

	if ( regulation_schedule->zone_a_array )
	{
		buffer_ptr += sprintf(
			buffer_ptr,
			"%s",
			regulation_schedule_zone_display(
				buffer_ptr,
				regulation_schedule->zone_a_array, "zone_a" ) );
	}

	if ( regulation_schedule->zone_b_array )
	{
		buffer_ptr += sprintf(
			buffer_ptr,
			"%s",
			regulation_schedule_zone_display(
				buffer_ptr,
				regulation_schedule->zone_b_array, "zone_b" ) );
	}

	if ( regulation_schedule->zone_c_array )
	{
		buffer_ptr += sprintf(
			buffer_ptr,
			"%s",
			regulation_schedule_zone_display(
				buffer_ptr,
				regulation_schedule->zone_c_array, "zone_c" ) );
	}

	if ( regulation_schedule->zone_c0_array )
	{
		buffer_ptr += sprintf(
			buffer_ptr,
			"%s",
			regulation_schedule_zone_display(
				buffer_ptr,
				regulation_schedule->zone_c0_array, "zone_c0"));
	}

	if ( regulation_schedule->zone_d_array )
	{
		buffer_ptr += sprintf(
			buffer_ptr,
			"%s",
			regulation_schedule_zone_display(
				buffer_ptr,
				regulation_schedule->zone_d_array, "zone_d" ) );
	}

	if ( regulation_schedule->zone_d0_array )
	{
		buffer_ptr += sprintf(
			buffer_ptr,
			"%s",
			regulation_schedule_zone_display(
				buffer_ptr,
				regulation_schedule->zone_d0_array, "zone_d0"));
	}

	if ( regulation_schedule->zone_e_array )
	{
		buffer_ptr += sprintf(
			buffer_ptr,
			"%s",
			regulation_schedule_zone_display(
				buffer_ptr,
				regulation_schedule->zone_e_array, "zone_e"));
	}

	if ( regulation_schedule->zone_e1_array )
	{
		buffer_ptr += sprintf(
			buffer_ptr,
			"%s",
			regulation_schedule_zone_display(
				buffer_ptr,
				regulation_schedule->zone_e1_array, "zone_e1"));
	}

	return strdup( buffer );
} /* regulation_schedule_display() */

char *regulation_schedule_zone_display( char *buffer,
					ZONE **zone_array, 
					char *zone_name )
{
	int i;
	char *buffer_ptr = buffer;
	
	buffer_ptr += sprintf( buffer_ptr, "%s:\n", zone_name );

	for( i = 1; i <= DAYS_PER_YEAR; i++ )
	{
		buffer_ptr += sprintf(
				buffer_ptr,
				"%d = %lf\n",
				i,
				zone_array[ i ]->upper_boundry );	
	}
	sprintf( buffer_ptr, "\n" );
	return buffer;
} /* regulation_schedule_zone_display() */

void zone_inflection_append(
			LIST *zone_inflection_list,
			int date_offset_one_based,
			double value )
{
	INFLECTION *inflection;

	inflection = (INFLECTION *)calloc( 1, sizeof( INFLECTION ) );
	inflection->date_offset_one_based = date_offset_one_based;
	inflection->value = value;
	list_append( zone_inflection_list, inflection, sizeof( INFLECTION ) );
} /* zone_inflection_append() */

char *regulation_schedule_get(	REGULATION_SCHEDULE *regulation_schedule,
				int date_offset_one_based,
				double value )
{
	if ( date_offset_one_based < 1
	||   date_offset_one_based > DAYS_PER_YEAR )
	{
		return "NULL";
	}

	if ( regulation_schedule->zone_e1_array
	&&   value <= zone_array_upper_boundry_get(
			regulation_schedule->zone_e1_array,
			date_offset_one_based ) )
	{
		return "E1";
	}
	else
	if ( regulation_schedule->zone_e_array
	&&   value <= zone_array_upper_boundry_get(
			regulation_schedule->zone_e_array,
			date_offset_one_based ) )
	{
		return "E";
	}
	else
	if ( regulation_schedule->zone_d0_array
	&&   value <= zone_array_upper_boundry_get(
			regulation_schedule->zone_d0_array,
			date_offset_one_based ) )
	{
		return "D0";
	}
	else
	if ( regulation_schedule->zone_d_array
	&&   value <= zone_array_upper_boundry_get(
			regulation_schedule->zone_d_array,
			date_offset_one_based ) )
	{
		return "D";
	}
	else
	if ( regulation_schedule->zone_c0_array
	&&   value <= zone_array_upper_boundry_get(
			regulation_schedule->zone_c0_array,
			date_offset_one_based ) )
	{
		return "C0";
	}
	else
	if ( regulation_schedule->zone_c_array
	&&   value <= zone_array_upper_boundry_get(
			regulation_schedule->zone_c_array,
			date_offset_one_based ) )
	{
		return "C";
	}
	else
	if ( regulation_schedule->zone_b_array
	&&   value <= zone_array_upper_boundry_get(
			regulation_schedule->zone_b_array,
			date_offset_one_based ) )
	{
		return "B";
	}
	else
	if ( regulation_schedule->zone_a_array
	&&   value <= zone_array_upper_boundry_get(
			regulation_schedule->zone_a_array,
			date_offset_one_based ) )
	{
		return "A";
	}
	else
	{
		return "none";
	}
} /* regulation_schedule_get() */

double zone_array_upper_boundry_get(
			ZONE **zone_array,
			int date_offset_one_based )
{
	return zone_array[ date_offset_one_based ]->upper_boundry;
}


void regulation_schedule_output( REGULATION_SCHEDULE *regulation_schedule,
				 int date_offset_one_based,
				 double value )
{
	printf( "%d %.2lf %s\n",
		date_offset_one_based,
		value,
		regulation_schedule_get(
					regulation_schedule,
					date_offset_one_based,
					value ) );
}

