#include <stdio.h>

int main( int argc, char **argv )
{
	printf( "should be 0 is %d\n", 0 && !1 );
	return 0;
}
