/* delete_measurements_day.c */
/* ------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"

int main( int argc, char **argv )
{
	char *entity;
	char input_line[ 1024 ];
	char station[ 128 ], datatype[ 128 ], measurement_date[ 128 ];
	int station_piece, datatype_piece, date_piece;
	char *table_name;
	int really_yn;

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s entity station_piece datatype_piece date_piece really_yn\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	entity = argv[ 1 ];
	station_piece = atoi( argv[ 2 ] );
	datatype_piece = atoi( argv[ 3 ] );
	date_piece = atoi( argv[ 4 ] );
	really_yn = *argv[ 5 ];

	table_name = get_table_name( entity, "measurement" );
	/* p = popen( "sql", "w" ); */

	while( get_line( input_line, stdin ) )
	{
		piece( station, '|', input_line, station_piece );
		piece( datatype, '|', input_line, datatype_piece );
		piece( measurement_date, '|', input_line, date_piece );

		if ( really_yn == 'y' )
		{
			char sys_string[ 1024 ];

			sprintf( sys_string,
		"echo \"delete from %s					"
		"	where station = '%s'				"
		"  	and datatype = '%s'				"
		"  	and measurement_date = '%s';\" | sql		",
			table_name,
			station,
			datatype,
			measurement_date );

			system( sys_string );

/*
Doesn't work because of pipelining.

			fprintf( p,
		"	delete from %s					"
		"	where station = '%s'				"
		"  	and datatype = '%s'				"
		"  	and measurement_date = '%s';			",
			table_name,
			station,
			datatype,
			measurement_date );

			fflush( p );
*/
		}

		printf( "%s\n", input_line );
	}
	/* pclose( p ); */
	exit( 0 );
}
