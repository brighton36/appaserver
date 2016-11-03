#include <stdio.h>
#include "list.h"
#include "alligator.h"

int main( void )
{
	TRANSECT *transect;
	int nest_utm_easting;
	int nest_utm_northing;
	double distance;
	char transect_boundary_should_be_yn;

	transect = alligator_transect_new( 0.0 );
	transect->east_easting =	546308;
	transect->east_northing =	2836193;
	transect->west_easting =	502509;
	transect->west_northing =	2836113;

	nest_utm_easting =		530334;
	nest_utm_northing =		2835285;

	transect_boundary_should_be_yn =
		alligator_get_in_transect_boundary_should_be_yn(
				&distance,
				"alligator" /* application_name */,
				61 /* transect */,
				nest_utm_northing,
				nest_utm_easting );

	printf( "for transect 61, got transect_boundary_should_be_yn = %c\n",
		transect_boundary_should_be_yn );

	printf( "for transect 61, got distance = %.1lf\n", distance );

	return 0;
}

