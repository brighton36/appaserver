#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"

int main()
{
	char buffer[ 1024 ];

	while( get_line( buffer, stdin ) )
	{
		if ( strncmp( buffer, "20", 2 ) != 0
		&&   strncmp( buffer, "19", 2 ) != 0 )
			printf( "19%s\n", buffer );
		else
			printf( "%s\n", buffer );
	}
	return 0;
}
