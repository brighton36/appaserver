#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main()
{
	double results = time2day_percentage( 12, 15 );
	printf( "should be .50104 is %lf\n", results );
	return 0;
}
