/* date2week.c */
/* ----------- */

#include <stdio.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "date.h"

int main( int argc, char **argv )
{
	int date_piece;
	char buffer[ 4096 ];
	char date_wo_week[ 128 ];
	char date_with_week[ 128 ];
	char *year_string;
	DATE *date;

	if ( argc != 3 )
	{
		fprintf( stderr, "Usage: %s YYYY date_piece\n", argv[ 0 ] );
		exit( 1 );
	}

	year_string = argv[ 1 ];
	date_piece = atoi( argv[ 2 ] );

	date = date_populate_week_of_year_dictionary( year_string );

	while( get_line( buffer, stdin ) )
	{
		if ( !piece( date_wo_week, '|', buffer, date_piece ) )
		{
			fprintf(stderr, 
				"Warning in %s/%s(): cannot get date in (%s)\n",
				__FILE__,
				__FUNCTION__,
				buffer );
			continue;
		}
		sprintf( date_with_week, 
			 "%s-%.2d",
			 date_wo_week, 
			 date_get_week_of_year( date, date_wo_week ) );
		piece_replace( buffer, '|', date_with_week, date_piece );
		printf( "%s\n", buffer );
	}
	return 0;
}
