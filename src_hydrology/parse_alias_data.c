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
LIST *parse_alias_data_get_datatype_list(
					char *station_name,
					char *input_filespecification,
					char *date_heading_label,
					boolean two_lines );

void parse_alias_display(
					char *station,
					char *input_filespecification,
					char *date_heading_label,
					LIST *datatype_list,
					boolean time_column );

int main( int argc, char **argv )
{
	char *input_filespecification;
	char *date_heading_label;
	char *station;
	LIST *datatype_list;
	boolean two_lines;
	boolean time_column;

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s filename station date_heading_label two_lines_yn time_column_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	input_filespecification = argv[ 1 ];
	station = argv[ 2 ];
	date_heading_label = argv[ 3 ];
	two_lines = ( *argv[ 4 ] == 'y' );
	time_column = ( *argv[ 5 ] == 'y' );

	if ( ( datatype_list =
			parse_alias_data_get_datatype_list(
				station,
				input_filespecification,
				date_heading_label,
				two_lines ) ) )
	{
		parse_alias_display(
			station,
			input_filespecification,
			date_heading_label,
			datatype_list,
			time_column );
	}

	return 0;

} /* main() */

void parse_alias_display(
			char *station,
			char *input_filespecification,
			char *date_heading_label,
			LIST *datatype_list,
			boolean time_column )
{
	FILE *input_file;
	char input_buffer[ 2048 ];
	int line_number = 0;
	double measurement_value;
	DATATYPE *datatype;
	char measurement_date_string[ 128 ];
	char measurement_time_string[ 128 ];
	char measurement_value_string[ 128 ];
	boolean got_heading = 0;

	if ( !list_length( datatype_list ) ) return;

	input_file = fopen( input_filespecification, "r" );

/*
	JULIAN *measurement_date_time_julian;
	DATE *measurement_date_time;
	measurement_date_time_julian = julian_new_julian( 0.0 );
	measurement_date_time = date_calloc();
*/

	*measurement_time_string = '\0';
	timlib_reset_get_line_check_utf_16();

/* Sample Input:
"2017-12-01 11:00:00",0,2017,335,1100,14.21,0.1,0,0,0,5.182,23.33,23.42,23.25,24.51,24.72,24.3
*/

	while( timlib_get_line( input_buffer, input_file, 1024 ) )
	{
		line_number++;

		if ( !*input_buffer ) continue;

		if ( !timlib_character_exists( input_buffer, ',' ) )
		{
			continue;
		}

		if ( !got_heading )
		{
			if ( instr(	date_heading_label,
					input_buffer,
					1 ) > -1 )
			{
				got_heading = 1;
			}
			continue;
		}

		/* Measurement Date */
		/* ---------------- */
		piece_quoted(	measurement_date_string,
				',',
				input_buffer,
				0,
				'"' );

		if ( !isdigit( *measurement_date_string ) )
			continue;

		/* Measurement Time */
		/* ---------------- */
		if ( time_column )
		{
			piece_quoted(	measurement_time_string,
					',',
					input_buffer,
					1,
					'"' );

		}
		else
		{
			char buffer[ 128 ];

			strcpy( buffer, measurement_date_string );
			column( measurement_date_string, 0, buffer );
			column( measurement_time_string, 1, buffer );
		}

/*
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
*/

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
			||   ( !isdigit( *measurement_value_string )
			&&     *measurement_value_string != '-' ) )
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

/*
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
*/
			printf(		"%s^%s^%s^%s^%.3lf\n",
					station,
					datatype->datatype_name,
					measurement_date_string,
					measurement_time_string,
					measurement_value );

		} while( list_next( datatype_list ) );
	}

	fclose( input_file );
	timlib_reset_get_line_check_utf_16();

} /* parse_alias_display() */

LIST *parse_alias_data_get_datatype_list(
				char *station_name,
				char *input_filespecification,
				char *date_heading_label,
				boolean two_lines )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	LIST *datatype_list = list_new();
	char datatype_name[ 128 ];
	char column_piece[ 128 ];
	DATATYPE *datatype;

	sprintf( sys_string,
		 "parse_alias_header \"%s\" \"%s\" \"%s\" %c",
		 input_filespecification,
		 station_name,
		 date_heading_label,
		 (two_lines) ? 'y' : 'n' ); 

	input_pipe = popen( sys_string, "r" );

	/* Expect: datatype^column_piece */
	/* ----------------------------- */
	while ( timlib_get_line( input_buffer, input_pipe, 1024 ) )
	{
		piece( datatype_name, '^', input_buffer, 0 );
		piece( column_piece, '^', input_buffer, 1 );

		datatype = datatype_new( strdup( datatype_name ) );
		datatype->column_piece = atoi( column_piece );
		list_append_pointer( datatype_list, datatype );
	}

	pclose( input_pipe );

	return datatype_list;

} /* parse_alias_data_get_datatype_list() */


