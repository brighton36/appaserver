/* utility/yesterday.c		       			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "date.h"

int main( int argc, char **argv )
{
	DATE *d;
	int days_to_minus = -1;

	if ( argc == 2 )
		days_to_minus = atoi( argv[ 1 ] );

	d = date_get_today_new( date_get_utc_offset() );
	date_increment_days( d, days_to_minus, date_get_utc_offset() );
	printf( "%s\n", date_display_yyyy_mm_dd( d ) );
	return 0;
}

