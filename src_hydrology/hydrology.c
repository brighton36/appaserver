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

STATION *hydrology_set_or_get_station(
			LIST *input_station_list,
			char *application_name,
			char *station_name )
{
	return hydrology_get_or_set_station(
			input_station_list,
			application_name,
			station_name );
}

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

char *hydrology_units_name_seek_phrase(
				LIST *station_datatype_list,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase )
{
	UNITS *units;

	if ( ! ( units =
			station_datatype_list_seek_units(
				station_datatype_list,
				datatype_units_seek_phrase ) ) )
	{
		return (char *)0;
	}

	return units->units_name;

} /* hydrology_units_name_seek_phrase() */

char *hydrology_datatype_name_seek_phrase(
				LIST *station_datatype_list,
				char *station_name,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase )
{
	DATATYPE *datatype;

	if ( ! ( datatype =
			hydrology_datatype_seek_phrase(
				station_datatype_list,
				station_name,
				datatype_units_seek_phrase ) ) )
	{
		return (char *)0;
	}

	return datatype->datatype_name;

} /* hydrology_datatype_name_seek_phrase() */

DATATYPE *hydrology_datatype_seek_phrase(
				LIST *station_datatype_list,
				char *station_name,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase )
{
	char *datatype_name;
	STATION_DATATYPE *station_datatype;
	char datatype_phrase[ 128 ];
	char units_phrase[ 128 ];

	*datatype_phrase = '\0';
	*units_phrase = '\0';

	hydrology_parse_datatype_units_phrase(
		datatype_phrase /* in/out */,
		units_phrase	/* in/out */,
		datatype_units_seek_phrase );

	if ( ! ( datatype_name =
			hydrology_translate_datatype_name(
				station_datatype_list,
				datatype_phrase ) ) )
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

	if ( !station_datatype->datatype )
	{
		fprintf( stderr,
	"Error in %s/%s()/%d: empty datatype for datatype_name = (%s)\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
		 	datatype_name );
		exit( 1 );
	}

	station_datatype->datatype->units =
		station_datatype_list_seek_units(
			station_datatype_list,
			units_phrase
				/* datatype_units_seek_phrase */ );

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

LIST *hydrology_get_header_column_datatype_list(
				LIST *station_datatype_list,
				char *station_name,
				char *input_filename,
				int first_column_piece )
{
	DATATYPE *datatype;
	LIST *return_list;
	char header_row[ 1024 ];
	char column_heading[ 256 ];
	int column_piece;

	if ( !timlib_get_first_line(
			header_row,
			input_filename ) )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: cannot get header_row in input_filename=%s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_filename );

		return (LIST *)0;
	}

	return_list = list_new();

	for(	column_piece = first_column_piece;
		piece_quote_comma(
			column_heading,
			header_row,
			column_piece );
		column_piece++ )
	{
		if ( ( datatype =
			hydrology_datatype_seek_phrase(
				station_datatype_list,
				station_name,
				column_heading
					/* datatype_units_seek_phrase */ ) ) )
		{
			datatype->column_piece = column_piece;

			list_append_pointer(
				return_list,
				datatype );
		}
	}

	return return_list;

} /* hydrology_get_header_column_datatype_list() */

void hydrology_parse_datatype_units_phrase(
				char *datatype_phrase	/* in/out */,
				char *units_phrase	/* in/out */,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_seek_phrase )
{
	int str_len;

	if ( !character_exists( datatype_seek_phrase, '(' ) )
	{
		strcpy( datatype_phrase, datatype_seek_phrase );
		return;
	}

	piece( datatype_phrase, '(', datatype_seek_phrase, 0 );
	str_len = strlen( datatype_phrase );
	right_trim( datatype_phrase );

	piece( units_phrase, ')', datatype_seek_phrase + str_len, 0 );

} /* hydrology_parse_datatype_units_phrase() */

