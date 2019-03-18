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

HYDROLOGY *hydrology_new( void )
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

	h->input.station_list = list_new();
	return h;

} /* hydrology_new() */

STATION *hydrology_get_or_set_station(
			LIST *input_station_list,
			char *application_name,
			char *station_name )
{
	STATION *station;

	if ( ( station =
		station_seek(
			station_name,
			input_station_list ) ) )
	{
		return station;
	}

	station = station_fetch_new( application_name, station_name );
	list_append_pointer( input_station_list, station );
	return station;

} /* hydrology_get_or_set_station() */

LIST *hydrology_get_header_column_datatype_list(
				char *application_name,
				char *station_name,
				char *input_filename,
				int first_column_piece )
{
	char heading_string[ 2048 ];
	LIST *header_column_datatype_list;
	char column_heading[ 256 ];
	int column_piece;
	HYDROLOGY *hydrology;
	STATION *station;
	DATATYPE *datatype;

	if ( !timlib_get_first_line(	heading_string,
					input_filename ) )
	{
		return (LIST *)0;
	}

	header_column_datatype_list = list_new();

	hydrology = hydrology_new();

	station =
		hydrology_get_or_set_station(
			hydrology->input.station_list,
			application_name,
			station_name );

	for(	column_piece = first_column_piece;
		piece_quote_comma(
			column_heading,
			heading_string,
			column_piece );
		column_piece++ )
	{
		if ( ( datatype =
			hydrology_datatype_seek_phrase(
				station->station_datatype_list,
				station->station_name,
				column_heading /* datatype_seek_phrase */ ) ) )
		{
			datatype->column_piece = column_piece;

			list_append_pointer(
				header_column_datatype_list,
				datatype );
		}
	}

	return header_column_datatype_list;

} /* hydrology_get_header_column_datatype_list() */

UNITS *hydrology_units_seek_phrase(
				LIST *station_datatype_list,
				char *units_seek_phrase )
{
	return station_datatype_list_seek_units(
				station_datatype_list,
				units_seek_phrase );
}

DATATYPE *hydrology_datatype_seek_phrase(
				LIST *station_datatype_list,
				char *station_name,
				char *datatype_seek_phrase )
{
	char *datatype_name;
	STATION_DATATYPE *station_datatype;

	if ( ! ( datatype_name =
			hydrology_translate_datatype_name(
				station_datatype_list,
				datatype_seek_phrase ) ) )
	{
		return (DATATYPE *)0;
	}

	if ( ! ( station_datatype =
			station_datatype_list_seek(
				station_datatype_list,
				station_name,
				datatype_name ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: cannot get station_datatype for datatype_name = (%s)\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
		 	datatype_name );
		exit( 1 );
	}

	return station_datatype->datatype;

} /* hydrology_datatype_seek_phrase() */

char *hydrology_translate_datatype_name(
				LIST *station_datatype_list,
				char *datatype_seek_phrase )
{
	STATION_DATATYPE *station_datatype;
	char *datatype_name;

	if ( !list_rewind( station_datatype_list ) )
		return (char *)0;

	do {
		station_datatype = list_get( station_datatype_list );

		if ( !station_datatype->datatype )
		{
			fprintf( stderr,
				 "Error in %s/%s()/%d: empty datatype.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		if ( ( datatype_name =
			station_datatype_translate_datatype_name(
				station_datatype->datatype,
				station_datatype->
					shef_upload_code,
				datatype_seek_phrase ) ) )
		{
			return datatype_name;
		}

	} while ( list_next( station_datatype_list ) );

	return (char *)0;

} /* hydrology_translate_datatype_name() */

