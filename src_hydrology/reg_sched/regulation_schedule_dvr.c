/* regulation_schedule_dvr.c */
/* ------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "column.h"
#include "timlib.h"
#include "regulation_schedule.h"

REGULATION_SCHEDULE *regulation_schedule_1_new( void );

int main( int argc, char **argv )
{
	char buffer[ 128 ];
	char date_offset_one_based[ 128 ];
	char value[ 128 ];
	REGULATION_SCHEDULE *regulation_schedule;

	regulation_schedule = regulation_schedule_1_new();

	if ( argc == 2 && strcmp( argv[ 1 ], "stdin" ) == 0 )
	{
		while( get_line( buffer, stdin ) )
		{
			column( date_offset_one_based, 0, buffer );

			if ( !column( value, 1, buffer ) )
			{
				fprintf( stderr,
					 "INPUT Warning: %s\n",
					 buffer );
				continue;
			}
			else
			{
				regulation_schedule_output(
					regulation_schedule,
					atoi( date_offset_one_based ),
					atof( value ) );
			}
		}
		exit( 0 );
	}

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s date_offset_one_based value\n",
			 argv[ 0 ] );
		fprintf( stderr,
			 "   or: %s stdin\n",
			 argv[ 0 ] );
		exit( 1 );
	}
	else
	{
		regulation_schedule_output(
			regulation_schedule,
			atoi( argv[ 1 ] ),
			atof( argv[ 2 ] ) );
	}
	return 0;
} /* main() */

REGULATION_SCHEDULE *regulation_schedule_1_new( void )
{
	REGULATION_SCHEDULE *regulation_schedule;

	LIST *zone_e1_inflection_list;
	LIST *zone_d_inflection_list;
	LIST *zone_c_inflection_list;
	LIST *zone_b_inflection_list;

	zone_e1_inflection_list = list_new();
	zone_inflection_append( zone_e1_inflection_list, 1, 10.5 );
	zone_inflection_append( zone_e1_inflection_list, 197, 9.25 );
	zone_inflection_append( zone_e1_inflection_list, 336, 10.25 );
	zone_inflection_append( zone_e1_inflection_list, 366, 10.25 );

	zone_d_inflection_list = list_new();
	zone_inflection_append( zone_d_inflection_list, 153, 9.5 );
	zone_inflection_append( zone_d_inflection_list, 277, 10.5 );
	zone_inflection_append( zone_d_inflection_list, 366, 10.5 );

	zone_c_inflection_list = list_new();
	zone_inflection_append( zone_c_inflection_list, 1, 10.75 );
	zone_inflection_append( zone_c_inflection_list, 46, 10.75 );
	zone_inflection_append( zone_c_inflection_list, 153, 10.0 );

	zone_b_inflection_list = list_new();
	zone_inflection_append( zone_b_inflection_list, 153, 10.0 );
	zone_inflection_append( zone_b_inflection_list, 271, 10.75 );
	zone_inflection_append( zone_b_inflection_list, 366, 10.75 );

/*
	regulation_schedule = regulation_schedule_new(
			(LIST *)0,
			zone_e_inflection_list,
			zone_d_inflection_list,
			zone_c_inflection_list,
			zone_b_inflection_list );
*/

	regulation_schedule = regulation_schedule_new(
			zone_e1_inflection_list,
			(LIST *)0 /* zone_e_inflection_list */,
			(LIST *)0 /* zone_d0_inflection_list */,
			zone_d_inflection_list,
			(LIST *)0 /* zone_c0_inflection_list */,
			zone_c_inflection_list,
			(LIST *)0 /* zone_b_inflection_list */,
			(LIST *)0 /* zone_a_inflection_list */ );

	return regulation_schedule;
} /* regulation_schedule_1_new() */

