/* make_pleasant.c */
/* --------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "timlib.h"

char *make_pleasant( char *s );

int main( int argc, char **argv )
{
	char buffer[ 2048 ];

	if ( argc == 2 )
	{
		printf( "%s\n", make_pleasant( argv[ 1 ] ) );
	}
	else
	while( get_line( buffer, stdin ) )
		printf( "%s\n", make_pleasant( buffer ) );

	return 0;
} /* main() */

char *make_pleasant( char *s )
{
	static char buffer[ 2048 ]; 
	char *buf_ptr = buffer;
	int beginning = 1;
	char c;

	while( ( c = *s++ ) )
	{
		if ( beginning )
		{
			*buf_ptr++ = toupper( c );
			beginning = 0;
		}
		else
		{
			if ( c == '_' )
			{
				*buf_ptr++ = ' ';
				beginning = 1;
			}
			else
			{
				if ( !isalnum( c ) )
				{
					*buf_ptr++ = c;
					beginning = 1;
				}
				else
				{
					*buf_ptr++ = tolower( c );
				}
			}
		}
	}

	return buffer;

} /* make_pleasant() */
