/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/hydrology.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hydrology.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"

HYDROLOGY *hydrology_new(	char *application_name,
				char *station_name )
{
	HYDROLOGY *h;

	if ( ! ( h = (HYDROLOGY *)calloc( 1, sizeof( HYDROLOGY ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	h->input.station =
		station_fetch_new(
			application_name,
			station_name );

	return h;

} /* hydrology_new() */

void hydrology_datatype_list_display(
				LIST *header_column_datatype_list )
{
} /* hydrology_datatype_list_display() */

LIST *hydrology_get_header_column_datatype_list(
				char *application_name,
				char *station,
				char *input_filename,
				int first_column_piece,
				LIST *input_datatype_list,
				LIST *input_units_list )
{
	char heading_string[ 2048 ];
	LIST *header_column_datatype_list;
	char column_heading[ 256 ];
	int column_piece;
	DATATYPE *datatype;

	if ( !timlib_get_first_line(	heading_string,
					input_filename ) )
	{
		return (LIST *)0;
	}

	header_column_datatype_list = list_new();

	for(	column_piece = first_column_piece;
		piece_quote_comma(
			column_heading,
			heading_string,
			column_piece );
		column_piece++ )
	{
		if ( ( datatype = 
			datatype_parse_new(
				application_name,
				station,
				column_heading,
				input_datatype_list,
				input_units_list ) ) )
		{
			datatype->column_piece = column_piece;

			list_append_pointer(
				header_column_datatype_list,
				datatype );
		}
	}

	return header_column_datatype_list;

} /* hydrology_get_header_column_datatype_list() */

char *hydrology_translate_datatype_name(
				LIST *station_datatype_list,
				char *datatype_name )
{
} /* hydrology_translate_datatype_name() */

