#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main()
{
	char *s = "0|1|2|3";
	double *f_array = (double *)calloc( 3, sizeof( double ) );

	atof_array( f_array, s );
	printf( "should be 0 is %.2lf\n", f_array[ 0 ] );
	printf( "should be 1 is %.2lf\n", f_array[ 1 ] );
	printf( "should be 2 is %.2lf\n", f_array[ 2 ] );
	printf( "should be 3 is %.2lf\n", f_array[ 3 ] );
	return 0;
}
