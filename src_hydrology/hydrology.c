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
#include "date.h"
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

	if ( ! ( datatype_name =
			hydrology_translate_datatype_name(
				station_datatype_list,
				datatype_units_seek_phrase ) ) )
	{
		char *decoded_datatype;

		/* Special codes include [mu] and [deg] */
		/* ------------------------------------ */
		decoded_datatype =
			units_search_replace_special_codes(
				datatype_units_seek_phrase );

		if ( ! ( datatype_name =
				hydrology_translate_datatype_name(
					station_datatype_list,
					decoded_datatype ) ) )
		{
			return (DATATYPE *)0;
		}
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

	/* Last character must be the closing paren ')'. */
	/* --------------------------------------------- */
	str_len = strlen( datatype_seek_phrase );

	if ( * ( datatype_seek_phrase + (str_len - 1) ) != ')' )
	{
		return;
	}

	piece( datatype_phrase, '(', datatype_seek_phrase, 0 );

	str_len = strlen( datatype_phrase );

	right_trim( datatype_phrase );

	piece( units_phrase, ')', datatype_seek_phrase + str_len + 2, 0 );
								/* ^ */
								/* | */
					/* Plus the space plus the paren. */
					/* ------------------------------ */

} /* hydrology_parse_datatype_units_phrase() */

MEASUREMENT *hydrology_extract_measurement(
				char *input_string,
				int column_piece )
{
	MEASUREMENT *measurement;
	char measurement_value[ 32 ];

	if ( !column_piece ) return (MEASUREMENT *)0;

	if ( !piece_quote_comma(
		measurement_value,
		input_string,
		column_piece ) )
	{
		return (MEASUREMENT *)0;
	}

	if ( !*measurement_value ) return (MEASUREMENT *)0;

	measurement = measurement_calloc();
	measurement->measurement_value = atof( measurement_value );

	return measurement;

} /* hydrology_extract_measurement() */

void hydrology_set_measurement(
			LIST *station_datatype_list,
			char *input_filename,
			int date_time_piece )
{
	FILE *input_file;
	char input_string[ 4096 ];
	int line_number = 0;
	STATION_DATATYPE *station_datatype;
	MEASUREMENT *measurement;
	char american_date_time[ 128 ];
	char measurement_date[ 32 ];
	char measurement_time[ 32 ];
	char *error_message = {0};

	if ( !list_length( station_datatype_list ) ) return;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_filename );
		exit( 1 );
	}

	while( timlib_get_line( input_string, input_file, 4096 ) )
	{
		line_number++;

		if ( line_number == 1 ) continue;

		trim( input_string );
		if ( !*input_string ) continue;

		if ( !piece_quote_comma(
			american_date_time,
			input_string,
			date_time_piece ) )
		{
			continue;
		}

		if ( !date_parse_american_date_time(
				measurement_date /* date_international */,
				measurement_time /* time_hhmm */,
				&error_message,
				american_date_time ) )
		{
			continue;
		}

		list_rewind( station_datatype_list );

		do {
			station_datatype = list_get( station_datatype_list );

			if ( !station_datatype->datatype
			||   !station_datatype->datatype->column_piece )
			{
				continue;
			}

			if ( ! ( measurement =
					hydrology_extract_measurement(
						input_string,
						station_datatype->
							datatype->
							column_piece ) ) )
			{
				continue;
			}

			measurement->measurement_date =
				strdup( measurement_date );

			measurement->measurement_time =
				strdup( measurement_time );

			if ( !station_datatype->measurement_list )
			{
				station_datatype->measurement_list = list_new();
			}

			list_append_pointer(
				station_datatype->measurement_list,
				measurement );

		} while ( list_next( station_datatype_list ) );
	}

	fclose( input_file );

	if ( error_message )
	{
		printf( "<h3>Errors</h3>\n" );
		printf( "<p>%s\n", error_message );
	}

} /* hydrology_set_measurement() */

void hydrology_summary_table_display(
				char *station_name,
				LIST *station_datatype_list )
{
	STATION_DATATYPE *station_datatype;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *heading_string;
	char *justify_string;
	LIST *measurement_list;

	if ( !list_rewind( station_datatype_list ) ) return;

	heading_string = "datatype,units,column,measurement_count";

	justify_string = "left,left,right";

	sprintf( sys_string,
		 "html_table.e 'Summary for station %s' '%s' '^' '%s'	 ",
		 station_name,
		 heading_string,
		 justify_string );

	output_pipe = popen( sys_string, "w" );

	do {
		station_datatype = list_get( station_datatype_list );

		if ( !station_datatype->datatype
		||   !station_datatype->datatype->column_piece )
		{
			continue;
		}

		measurement_list = station_datatype->measurement_list;

		fprintf( output_pipe,
			 "%s^%s^%d^%d\n",
			 station_datatype->datatype->datatype_name,
			 (station_datatype->datatype->units)
			 	? station_datatype->datatype->units->units_name
				: "unknown",
			 station_datatype->datatype->column_piece + 1,
			 list_length( measurement_list ) );

	} while ( list_next( station_datatype_list ) );

	pclose( output_pipe );

} /* hydrology_summary_table_display() */

int hydrology_measurement_insert(
				char *station_name,
				LIST *station_datatype_list )
{
	STATION_DATATYPE *station_datatype;
	MEASUREMENT *measurement;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	LIST *measurement_list;
	char *field;
	char *table;
	int load_count = 0;

	if ( !list_rewind( station_datatype_list ) ) return 0;

	table = "measurement";

	field =
"station,datatype,measurement_date,measurement_time,measurement_value";

	/* Can't have both replace=y and compress=y */
	/* ---------------------------------------- */
	sprintf( sys_string,
	"insert_statement.e table=%s field=%s del='^' replace=y	compress=n |"
	"sql.e 2>&1							   |"
	"html_paragraph_wrapper.e					    ",
		 table,
		 field );

	output_pipe = popen( sys_string, "w" );

	do {
		station_datatype = list_get( station_datatype_list );

		if ( !station_datatype->datatype ) continue;

		measurement_list = station_datatype->measurement_list;

		if ( !list_rewind( measurement_list ) ) continue;

		do {
			measurement = list_get( measurement_list );

			fprintf( output_pipe,
				 "%s^%s^%s^%s^%.3lf\n",
				 station_name,
				 station_datatype->datatype->datatype_name,
				 measurement->measurement_date,
				 measurement->measurement_time,
				 measurement->measurement_value );

			load_count++;

		} while ( list_next( measurement_list ) );

	} while ( list_next( station_datatype_list ) );

	pclose( output_pipe );
	return load_count;

} /* hydrology_measurement_insert() */

int hydrology_measurement_table_display(
				char *station_name,
				LIST *station_datatype_list )
{
	STATION_DATATYPE *station_datatype;
	MEASUREMENT *measurement;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *heading_string;
	char *justify_string;
	LIST *measurement_list;
	int load_count;

	if ( !list_rewind( station_datatype_list ) ) return 0;

	heading_string =
"datatype,measurement_date,measurement_time,measurement_value";

	justify_string = "left,left,left,right";

	sprintf( sys_string,
	"queue_top_bottom_lines.e 500					|"
	"html_table.e 'Measurements for station %s' '%s' '^' '%s'	 ",
		 station_name,
		 heading_string,
		 justify_string );

	output_pipe = popen( sys_string, "w" );

	load_count = 0;

	do {
		station_datatype = list_get( station_datatype_list );

		if ( !station_datatype->datatype ) continue;

		measurement_list = station_datatype->measurement_list;

		if ( !list_rewind( measurement_list ) ) continue;

		do {
			measurement = list_get( measurement_list );

			fprintf( output_pipe,
				 "%s^%s^%s^%.3lf\n",
				 station_datatype->datatype->datatype_name,
				 measurement->measurement_date,
				 measurement->measurement_time,
				 measurement->measurement_value );

			load_count++;

		} while ( list_next( measurement_list ) );

	} while ( list_next( station_datatype_list ) );

	pclose( output_pipe );
	return load_count;

} /* hydrology_measurement_table_display() */

