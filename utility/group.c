/* utility/group.c */
/* --------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"

void group(	char delimiter,
		char *operation,
		char *argv_0 );

int main( int argc, char **argv )
{
	char delimiter = {0};
	char *operation = "count";

	if ( argc >= 2 ) delimiter = *argv[ 1 ];
	if ( argc == 3 ) operation = argv[ 2 ];

	if ( strcmp( operation, "count" ) != 0
	&&   strcmp( operation, "sum" ) != 0 )
	{
		fprintf( stderr,
			 "Usage: %s [delimiter] [count|sum]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	group( delimiter, operation, argv[ 0 ] );
	
	return 0;

} /* main() */


void group( char delimiter, char *operation, char *argv_0 )
{
	char input_buffer[ 1024 ];
	char compare_buffer[ 1024 ];
	char old_compare_buffer[ 1024 ];
	char old_input_buffer[ 1024 ];
	char value_buffer[ 1024 ] = {0};
	int count = 0;
	double sum = 0.0;
	int first_time = 1;

	*old_compare_buffer = '\0';

	while( get_line( input_buffer, stdin ) )
	{
		if ( delimiter )
		{
			piece( compare_buffer, delimiter, input_buffer, 0 );

			if ( !piece(	value_buffer,
					delimiter,
					input_buffer,
					1 ) )
			{
				fprintf( stderr,
				"Warning in %s: cannot piece(%c) in (%s)\n",
					 argv_0,
					 delimiter,
					 input_buffer );
				*value_buffer = '\0';
			}
		}
		else
		{
			strcpy( compare_buffer, input_buffer );
		}

		if ( first_time )
		{
			first_time = 0;	
			count = 1;
			strcpy( old_compare_buffer, compare_buffer );
			strcpy( old_input_buffer, input_buffer );

			if ( *value_buffer ) sum = atof( value_buffer );

			continue;
		}

		if ( strcmp( compare_buffer, old_compare_buffer ) == 0 )
		{
			count++;
			if ( *value_buffer ) sum += atof( value_buffer );
		}
		else
		{
			if ( strcmp( operation, "count" ) == 0 )
			{
				printf( "%d|%s\n", count, old_input_buffer );
			}
			else
			if ( strcmp( operation, "sum" ) == 0 )
			{
				printf( "%s%c%.2lf\n",
					old_compare_buffer,
					delimiter,
					sum );
			}

			count = 1;
			sum = atof( value_buffer );
		}

		strcpy( old_compare_buffer, compare_buffer );
		strcpy( old_input_buffer, input_buffer );
	}

	if ( strcmp( operation, "count" ) == 0 )
	{
		if ( count ) printf( "%d|%s\n", count, old_input_buffer );
	}
	else
	if ( strcmp( operation, "sum" ) == 0 )
	{
		if ( count )
		{
			printf( "%s%c%.2lf\n",
				old_compare_buffer,
				delimiter,
				sum );
		}
	}

} /* group() */

