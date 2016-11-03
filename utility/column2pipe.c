/* column2pipe.c */
/* ------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "timlib.h"

char *column2pipe( char *d, char *s, int number_fields_to_pipe );

int main( int argc, char **argv )
{
	char buffer[ 1024 ];
	char input[ 1024 ];
	int number_fields_to_pipe;

	if ( argc != 2 )
	{
		fprintf( stderr, 
			 "usage: %s number_fields_to_pipe\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	number_fields_to_pipe = atoi( argv[ 1 ] );

	while( get_line( input, stdin ) )
	{
		printf( "%s\n", 
			column2pipe( buffer, input, number_fields_to_pipe ) );
	}
	return 0;
} /* main() */

char *column2pipe( char *d, char *s, int number_fields )
{
	char *anchor = d;
	int first_time = 1;

	*d = '\0';
	while( *s )
	{
		s = skip_spaces( s );

		if ( first_time )
			first_time = 0;
		else
			*d++ = '|';

		while( *s && !isspace( *s ) ) *d++ = *s++;
		*d = '\0';

		if ( number_fields == 0 )
		{
			strcpy( d, s );
			break;
		}
		number_fields--;
	}

	while( number_fields-- )
	{
		*d++ = '|';
		*d = '\0';
	}

	return anchor;

} /* column2pipe() */

