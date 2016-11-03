#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( void )
{
	char buffer[ 65536 ];
	while( get_line( buffer, stdin ) );
	return 0;
}
