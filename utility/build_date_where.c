/* utility/build_date_where.c */
/* -------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "query.h"

int main( int argc, char **argv )
{
	char *date_column_name;
	char *time_column_name;
	char *begin_date;
	char *begin_time;
	char *end_date;
	char *end_time;

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s date_column_name time_column_name begin_date begin_time end_date end_time\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	date_column_name = argv[ 1 ];
	time_column_name = argv[ 2 ];
	begin_date = argv[ 3 ];
	begin_time = argv[ 4 ];
	end_date = argv[ 5 ];
	end_time = argv[ 6 ];

	printf( "and %s\n",
		query_get_between_date_time_where(
					date_column_name,
					time_column_name,
					begin_date,
					begin_time,
					end_date,
					end_time,
					(char *)0 /* application */,
					(char *)0 /* folder_name */ ) );
	return 0;
} /* main() */

