/* $APPASERVER_HOME/library/distance.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "distance.h"

unsigned int distance_get_pythagorean_distance_integer(
					unsigned int point_a_x,
					unsigned int point_a_y,
					unsigned int point_b_x,
					unsigned int point_b_y )
{
	unsigned int x_difference;
	unsigned int y_difference;
	unsigned int results;

	x_difference = point_a_x - point_b_x;
	y_difference = point_a_y - point_b_y;

	results = (unsigned int)sqrt(
			( x_difference * x_difference ) + 
			( y_difference * y_difference ) );
	return results;

} /* distance_get_pythagorean_distance_integer() */

