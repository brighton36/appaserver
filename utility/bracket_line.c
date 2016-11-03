#include <stdio.h>

main( int argc, char **argv )
{
	char buffer[ 2048 ];
	char open = '[';
	char close = ']';

	if ( argc == 3 )
	{
		open = *argv[ 1 ];
		close = *argv[ 2 ];
	}

	while( fgets( buffer, 2047, stdin ) )
	{
		buffer[ strlen( buffer ) - 1 ] = '\0';
		printf( "%c%s%c\n", open, buffer, close );
	}
}
