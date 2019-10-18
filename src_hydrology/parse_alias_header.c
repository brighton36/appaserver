/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/parse_alias_header.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "query.h"
#include "date.h"
#include "basename.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "timlib.h"
#include "column.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "julian.h"
#include "boolean.h"
#include "process.h"
#include "dictionary.h"
#include "session.h"
#include "application.h"
#include "hydrology_library.h"
#include "station.h"
#include "datatype.h"
#include "units.h"
#include "hydrology.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void parse_alias_display(		LIST *datatype_list );

LIST *input_buffer_get_datatype_list(	char *application_name,
					char *station_name,
					char *heading_buffer,
					char *second_line,
					char *date_heading_label );

LIST *parse_alias_data_get_datatype_list(
					char *application_name,
					char *station_name,
					char *input_filespecification,
					char *date_heading_label,
					boolean two_lines );

int main( int argc, char **argv )
{
	char *application_name;
	char *input_filespecification;
	char *date_heading_label;
	char *station;
	LIST *datatype_list;
	boolean two_lines;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	if ( argc != 5 )
	{
		fprintf( stderr,
		 "Usage: %s filename station date_heading_label two_lines_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	input_filespecification = argv[ 1 ];
	station = argv[ 2 ];
	date_heading_label = argv[ 3 ];
	two_lines = (*argv[ 4 ] == 'y');

	if ( ( datatype_list =
			parse_alias_data_get_datatype_list(
				application_name,
				station,
				input_filespecification,
				date_heading_label,
				two_lines ) ) )
	{
		parse_alias_display( datatype_list );
	}


	return 0;

} /* main() */

void parse_alias_display( LIST *datatype_list )
{
	DATATYPE *datatype;

	if ( !list_rewind( datatype_list ) ) return;

	do {
		datatype = list_get_pointer( datatype_list );

		printf( "%s^%d\n",
			datatype->datatype_name,
			datatype->column_piece );

	} while ( list_next( datatype_list ) );

} /* parse_alias_display() */

LIST *parse_alias_data_get_datatype_list(
				char *application_name,
				char *station_name,
				char *input_filespecification,
				char *date_heading_label,
				boolean two_lines )
{
	FILE *input_file;
	char heading_buffer[ 1024 ];
	char second_line[ 1024 ];
	LIST *datatype_list = {0};

	if ( ! ( input_file = fopen( input_filespecification, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_filespecification );
		fflush( stderr );
		exit( 1 );
	}

	*second_line = '\0';
	timlib_reset_get_line_check_utf_16();

	while( timlib_get_line( heading_buffer, input_file, 1024 ) )
	{
		if ( instr( date_heading_label, heading_buffer, 1 ) > -1 )
		{
			if ( two_lines ) 
			{
				timlib_get_line(
					second_line,
					input_file,
					1024 );
			}

			fclose( input_file );
			timlib_reset_get_line_check_utf_16();

			datatype_list =
				input_buffer_get_datatype_list(
					application_name,
					station_name,
					heading_buffer,
					second_line,
					date_heading_label );

			return datatype_list;
		}
	}

	fclose( input_file );
	timlib_reset_get_line_check_utf_16();

	return (LIST *)0;

} /* parse_alias_data_get_datatype_list() */

/* --------------------------- */
/* Sets datatype->column_piece */
/* --------------------------- */
LIST *input_buffer_get_datatype_list(	char *application_name,
					char *station_name,
					char *heading_buffer,
					char *second_line,
					char *date_heading_label )
{
	LIST *datatype_list;
	DATATYPE *datatype;
	char datatype_heading_first_line[ 128 ];
	char datatype_heading_second_line[ 128 ];
	char two_line_datatype_heading[ 256 ];
	int column_piece;
	STATION *station;

	station =
		station_get_or_set_station(
			list_new() /* station_list */,
			application_name,
			station_name );

	datatype_list = list_new();

	for(	column_piece = 0;
		piece_quoted(	datatype_heading_first_line,
				',',
				heading_buffer,
				column_piece,
				'"' );
		column_piece++ )
	{
		if ( !*datatype_heading_first_line ) continue;

		if ( strcasecmp(datatype_heading_first_line,
				date_heading_label ) == 0 )
		{
			continue;
		}

		if ( *second_line )
		{
			piece_quoted(	datatype_heading_second_line,
					',',
					second_line,
					column_piece,
					'"' );
		}

		if ( *datatype_heading_second_line )
		{
			sprintf(two_line_datatype_heading,
			 	"%s %s",
			 	datatype_heading_first_line,
			 	datatype_heading_second_line );
		}
		else
		{
			strcpy( two_line_datatype_heading,
			 	datatype_heading_first_line );
		}

		trim( two_line_datatype_heading );

		if ( ! ( datatype =
				datatype_seek_phrase(
					station->station_datatype_list,
					station->station_name,
					two_line_datatype_heading ) ) )
		{
			continue;
		}

		datatype->column_piece = column_piece;
		list_append_pointer( datatype_list, datatype );
	}

	return datatype_list;

} /* input_buffer_get_datatype_list() */

