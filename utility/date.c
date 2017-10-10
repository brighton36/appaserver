/* date.c */
/* ------ */

#include <stdio.h>
#include <stdlib.h>
#include "date.h"
#include "boolean.h"

int main( int argc, char **argv )
{
	float days_offset;
	boolean with_seconds = 0;
	DATE *d;

	if ( argc < 2 )
	{
		fprintf( stderr, "%s days_offset [seconds]\n", argv[ 0 ] );
		exit( 1 );
	}

	days_offset = atof( argv[ 1 ] );

	if ( argc == 3 )
	{
		with_seconds = ( strcmp( argv[ 2 ], "seconds" ) == 0 );
	}

	if ( !days_offset && !with_seconds )
	{
		printf( "%s\n", date_get_unix_now_string() );
	}
	else
	{
		d = date_get_today_new( HOURS_WEST_GMT );

		if ( days_offset )
		{
			date_increment_days( d, days_offset, HOURS_WEST_GMT );
			printf( "%s\n", date_display_yyyy_mm_dd( d ) );
		}
		else
		{
			if ( with_seconds )
			{
				printf( "%s:%s\n",
					date_display_yyyy_mm_dd( d ),
					date_display_hhmmss( d ) );
			}
			else
			{
				printf( "%s:%s\n",
					date_display_yyyy_mm_dd( d ),
					date_display_hhmm( d ) );
			}
		}
	}
	return 0;
} /* main() */

