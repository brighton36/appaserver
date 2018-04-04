/* $APPASERVER_HOME/library/distance.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "distance.h"

int distance_get_pythagorean_distance_integer(
					int point_a_x, int point_a_y,
					int point_b_x, int point_b_y )
{
	int x_difference;
	int y_difference;
	int results;

	x_difference = point_a_x - point_b_x;
	y_difference = point_a_y - point_b_y;

	results = (int)sqrt(
			( x_difference * x_difference ) + 
			( y_difference * y_difference ) );
	return results;

} /* distance_get_pythagorean_distance_integer() */

