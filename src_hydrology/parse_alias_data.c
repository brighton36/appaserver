/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/parse_alias_data.c	*/
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
LIST *input_buffer_get_datatype_list(	char *application_name,
					char *station_name,
					char *input_buffer,
					char *date_heading_label );

LIST *parse_alias_data_get_datatype_list(
					char **error_message,
					char *application_name,
					char *station_name,
					char *input_filespecification,
					char *date_heading_label );

void parse_alias_filespecification(
					char *input_filespecification,
					char *station,
					LIST *datatype_list,
					char *date_heading_label );

int main( int argc, char **argv )
{
	char *application_name;
	char *input_filespecification;
	char *date_heading_label;
	char *station;
	LIST *datatype_list;
	char *error_message = {0};

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s filename station date_heading_label\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	input_filespecification = argv[ 1 ];
	station = argv[ 2 ];
	date_heading_label = argv[ 3 ];

	if ( ! ( datatype_list =
			parse_alias_data_get_datatype_list(
				&error_message,
				application_name,
				station,
				input_filespecification,
				date_heading_label ) ) )
	{
		fprintf( stderr, "Error: %s\n", error_message );
		fflush( stderr );
		exit( 1 );
	}

	parse_alias_filespecification(
		input_filespecification,
		station,
		datatype_list,
		date_heading_label );

	return 0;

} /* main() */

void parse_alias_filespecification(
			char *input_filespecification,
			char *station,
			LIST *datatype_list,
			char *date_heading_label )
{
	FILE *input_file;
	char input_buffer[ 2048 ];
	int line_number = 0;
	double measurement_value;
	DATATYPE *datatype;
	char measurement_date_time_string[ 1024 ];
	char measurement_value_string[ 32 ];
	JULIAN *measurement_date_time_julian;
	DATE *measurement_date_time;

	if ( !list_length( datatype_list ) ) return;

	input_file = fopen( input_filespecification, "r" );

	timlib_reset_get_line_check_utf_16();
	measurement_date_time_julian = julian_new_julian( 0.0 );
	measurement_date_time = date_calloc();

/* Sample Input:
"2017-12-01 11:00:00",0,2017,335,1100,14.21,0.1,0,0,0,5.182,23.33,23.42,23.25,24.51,24.72,24.3
*/

	while( timlib_get_line( input_buffer, input_file, 1024 ) )
	{
		line_number++;

		if ( !*input_buffer || *input_buffer != '"' )
		{
			continue;
		}

		if ( instr( date_heading_label, input_buffer, 1 ) != -1 )
		{
			continue;
		}

		if ( !timlib_character_exists( input_buffer, ',' ) )
		{
			fprintf( stderr,
				 "No comma in line %d: %s\n",
				 line_number,
				 input_buffer );
			fflush( stderr );
			continue;
		}

		/* Measurement date/time */
		/* --------------------- */
		piece_quoted(	measurement_date_time_string,
				',',
				input_buffer,
				0,
				'"' );

		if ( !date_set_yyyy_mm_dd_hh_mm_ss_colon(
				measurement_date_time,
				measurement_date_time_string ) )
		{
			fprintf( stderr,
				 "Ignoring bad date/time in line %d: %s\n",
				 line_number,
				 input_buffer );
			fflush( stderr );
			continue;
		}

		julian_set_yyyy_mm_dd_hhmm(
			measurement_date_time_julian,
			date_display_yyyy_mm_dd( measurement_date_time ),
			date_display_hhmm( measurement_date_time ) );

		measurement_date_time_julian =
			hydrology_library_adjust_time_to_sequence(
				measurement_date_time_julian,
				VALID_FREQUENCY_TIME_SEQUENCE );

		list_rewind( datatype_list );

		do {
			datatype = list_get_pointer( datatype_list );

			if ( !piece(	measurement_value_string,
					',',
					input_buffer,
					datatype->column_piece ) )
			{
				fprintf( stderr,
				 "Cannot extract datatype %s in line %d: %s\n",
				 	 datatype->datatype_name,
				 	 line_number,
				 	 input_buffer );
				fflush( stderr );
				continue;
			}

			if ( !*measurement_value_string
			||   timlib_strcmp(
				measurement_value_string,
				"nan" ) == 0 )
			{
				fprintf( stderr,
			"Datatype %s has an empty value in line %d: %s\n",
				 	 datatype->datatype_name,
				 	 line_number,
				 	 input_buffer );
				fflush( stderr );
				continue;
			}

			measurement_value = atof( measurement_value_string );

			if ( datatype->set_negative_values_to_zero
			&&   measurement_value < 0.0 )
			{
				measurement_value = 0.0;
			}

			printf(		"%s^%s^%s^%s^%.3lf\n",
					station,
					datatype->datatype_name,
					julian_display_yyyy_mm_dd(
						measurement_date_time_julian->
							current ),
					julian_display_hhmm(
						measurement_date_time_julian->
							current ),
					measurement_value );

		} while( list_next( datatype_list ) );
	}

	fclose( input_file );
	timlib_reset_get_line_check_utf_16();

} /* parse_alias_filespecification() */

LIST *parse_alias_data_get_datatype_list(
				char **error_message,
				char *application_name,
				char *station_name,
				char *input_filespecification,
				char *date_heading_label )
{
	FILE *input_file;
	char input_buffer[ 1024 ];
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

	timlib_reset_get_line_check_utf_16();

	while( timlib_get_line( input_buffer, input_file, 1024 ) )
	{
		if ( instr( date_heading_label, input_buffer, 1 ) != -1 )
		{
			fclose( input_file );

			timlib_reset_get_line_check_utf_16();

			datatype_list =
				input_buffer_get_datatype_list(
					application_name,
					station_name,
					input_buffer,
					date_heading_label );

			if ( !list_length( datatype_list ) )
			{
				*error_message =
				"None of the headings matched a datatype.";
				return (LIST *)0;
			}

			return datatype_list;
		}
	}

	fclose( input_file );

	timlib_reset_get_line_check_utf_16();

	*error_message = "No date header found in input file.";

	return (LIST *)0;

} /* parse_alias_data_get_datatype_list() */

LIST *input_buffer_get_datatype_list(	char *application_name,
					char *station_name,
					char *input_buffer,
					char *date_heading_label )
{
	LIST *datatype_list;
	DATATYPE *datatype;
	char datatype_heading[ 128 ];
	int column_piece;
	HYDROLOGY *hydrology;
	STATION *station;

	hydrology = hydrology_new();

	station =
		hydrology_get_or_set_station(
			hydrology->input.station_list,
			application_name,
			station_name );

	datatype_list = list_new();

	for(	column_piece = 0;
		piece_quoted(	datatype_heading,
				',',
				input_buffer,
				column_piece,
				'"' );
		column_piece++ )
	{
		if ( !*datatype_heading ) continue;

		if ( strcmp(	datatype_heading,
				date_heading_label ) == 0 )
		{
			continue;
		}

		if ( ! ( datatype =
				hydrology_datatype_seek_phrase(
					station->station_datatype_list,
					station->station_name,
					datatype_heading ) ) )
		{
			continue;
		}

		datatype->column_piece = column_piece;
		list_append_pointer( datatype_list, datatype );
	}

	return datatype_list;

} /* input_buffer_get_datatype_list() */

