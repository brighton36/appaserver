/* src_capitolpops/composition_venue_summary.c		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "dictionary.h"
#include "folder.h"
#include "timlib.h"
#include "query.h"
#include "piece.h"
#include "date.h"
#include "attribute.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "capitolpops.h"

/* Prototypes */
/* ---------- */
void composition_summary(	void );

char *get_composition_venue_where(
		 		char *composition,
				char *city,
				char *venue );

void composition_venue_summary(
				char *composition,
				char *city,
				char *venue );

int main( int argc, char **argv )
{
	char *composition;
	char *city;
	char *venue;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s composition city venue\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	composition = argv[ 1 ];
	city = argv[ 2 ];
	venue = argv[ 3 ];

	if ( strcmp( composition, "composition" ) == 0
	&&   strcmp( city, "city" ) == 0
	&&   strcmp( venue, "venue" ) == 0 )
	{
		composition_summary();
	}
	else
	{
		composition_venue_summary( composition, city, venue );
	}

	return 0;

} /* main() */

void composition_venue_summary(	char *composition,
				char *city,
				char *venue )
{
	char select[ 1024 ];
	char sys_string[ 1024 ];
	char input_buffer[ 512 ];
	FILE *input_pipe;
	char count_string[ 16 ];
	int count_total = 0;
	char *where;
	char order_process[ 32 ];

	where = get_composition_venue_where(
		 	composition,
			city,
			venue );

/*
	if ( strcmp( where, "1 = 1" ) == 0 )
	{
		strcpy( order_process, "cat" );
	}
	else
	{
		sprintf( order_process,
			 "sort -t'%c' -k5 -rn",
			 FOLDER_DATA_DELIMITER );
	}
*/
	sprintf( order_process,
		 "sort -t'%c' -k5 -rn",
		 FOLDER_DATA_DELIMITER );

	sprintf( select,
"select concert.city,concert.venue,composition,max( concert.date ), count(*) from concert,concert_program where concert.concert_title = concert_program.concert_title and concert.date = concert_program.date and %s and concert.date < '%s' group by concert.city,concert.venue,composition order by composition, concert.city, concert.venue;",
		 where,
		 date_get_today_yyyy_mm_dd() );

	sprintf( sys_string,
		 "echo \"%s\" | sql.e '%c' | %s",
		 select,
		 FOLDER_DATA_DELIMITER,
		 order_process );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( count_string, FOLDER_DATA_DELIMITER, input_buffer, 4 );

		count_total += atoi( count_string );

		printf( "%s%s\n", RECORD_LABEL, input_buffer );
	}

	pclose( input_pipe );

	printf( "%s%d\n", TOTAL_LABEL, count_total );

} /* composition_venue_summary() */

void composition_summary( void )
{
	char select[ 1024 ];
	char sys_string[ 1024 ];
	char input_buffer[ 512 ];
	FILE *input_pipe;
	char count_string[ 16 ];
	int count_total = 0;

	sprintf( select,
"select composition, max( concert.date ), count(*) from concert,concert_program where concert.concert_title = concert_program.concert_title and concert.date = concert_program.date and concert.date < '%s' group by composition order by count(*) desc;",
		 date_get_today_yyyy_mm_dd() );

	sprintf( sys_string,
		 "echo \"%s\" | sql.e '%c'",
		 select,
		 FOLDER_DATA_DELIMITER );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( count_string, FOLDER_DATA_DELIMITER, input_buffer, 2 );

		count_total += atoi( count_string );

		printf( "%s%s\n", RECORD_LABEL, input_buffer );
	}

	pclose( input_pipe );

	printf( "%s%d\n", TOTAL_LABEL, count_total );

} /* composition_summary() */

char *get_composition_venue_where(
		 		char *composition,
				char *city,
				char *venue )
{
	static char where[ 1024 ];

	if ( *composition && strcmp( composition, "composition" ) != 0
	&&   *venue && strcmp( venue, "venue" ) != 0 )
	{
		sprintf( where,
			 "composition = '%s' and city = '%s' and venue = '%s'",
			 composition,
			 city,
			 venue );
	}
	else
	if ( *composition && strcmp( composition, "composition" ) != 0
	&&   ( !*venue || strcmp( venue, "venue" ) == 0 ) )
	{
		sprintf( where,
			 "composition = '%s'",
			 escape_single_quote( composition ) );
	}
	else
	if ( ( !*composition || strcmp( composition, "composition" ) == 0 )
	&&   *venue && strcmp( venue, "venue" ) != 0 )
	{
		sprintf( where,
			 "city = '%s' and venue = '%s'",
			 city,
			 venue );
	}
	else
	{
		strcpy( where, "1 = 1" );
	}
 
	return where;

} /* get_composition_venue_where() */

