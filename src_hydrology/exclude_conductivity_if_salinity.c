/* src_everglades/exclude_conductivify_if_salinity.c    */
/* ---------------------------------------------------- */
/* This process is embedded in the custom_chart process	*/
/* to filter out the conductivity if salinity exists.	*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "boolean.h"

LIST *get_station_record_list( int station_piece, char delimiter );

int main( int argc, char **argv )
{
	int datatype_piece;
	int station_piece;
	char *station_record;
	char datatype_buffer[ 512 ];
	char delimiter;
	LIST *station_record_list;
	boolean got_salinity;

	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s delimiter datatype_piece station_piece\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	datatype_piece = atoi( argv[ 2 ] );
	station_piece = atoi( argv[ 3 ] );

	while( ( station_record_list =
			get_station_record_list( station_piece, delimiter ) ) )
	{
		if ( !list_rewind( station_record_list ) ) continue;
		got_salinity = 0;

		do {
			station_record =
				list_get_pointer(
					station_record_list );

			if ( !piece(	datatype_buffer,
					delimiter,
				 	station_record,
				 	datatype_piece ) )
			{
				continue;
			}

			if ( strncmp( datatype_buffer, "salinity", 8 ) == 0 )
				got_salinity = 1;

		} while( list_next( station_record_list ) );

		list_rewind( station_record_list );

		do {
			station_record =
				list_get_pointer(
					station_record_list );

			if ( !piece(	datatype_buffer,
					delimiter,
				 	station_record,
				 	datatype_piece ) )
			{
				continue;
			}

			if ( strncmp( datatype_buffer, "conductivity", 12 ) == 0
			&&   got_salinity )
			{
				continue;
			}

			printf( "%s\n", station_record );
		} while( list_next( station_record_list ) );
		list_free_string_list( station_record_list );
	}
	exit( 0 );
} /* main() */

LIST *get_station_record_list( int station_piece, char delimiter )
{
	char input_buffer[ 1024 ];
	char station_buffer[ 512 ];
	char old_station_buffer[ 512 ];
	LIST *station_record_list = {0};
	boolean first_time = 1;

	while( get_line_queue( input_buffer, stdin ) )
	{
		if ( !piece(	station_buffer,
				delimiter,
			 	input_buffer,
			 	station_piece ) )
		{
			continue;
		}

		if ( first_time )
		{
			strcpy( old_station_buffer, station_buffer );
			first_time = 0;
		}

		if ( strcmp( old_station_buffer, station_buffer ) != 0 )
		{
			unget_line_queue( input_buffer );
			return station_record_list;
		}

		if ( !station_record_list )
			station_record_list = list_new_list();

		list_append_pointer(	station_record_list,
					strdup( input_buffer ) );
	}
	return station_record_list;

} /* get_station_record_list() */

