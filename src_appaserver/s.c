#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
	char buffer[ 4096 ];
	char delimiter;

	if ( argc == 2 )
		delimiter = *argv[ 1 ];
	else
		delimiter = '^';

	sprintf(	buffer,
			"grep \"^select \"			|"
			"grep -v appaserver_user		|"
			"sql.e '%c'				 ",
			delimiter );
	system( buffer );
	return 0;
}
