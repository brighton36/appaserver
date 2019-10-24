/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/spreadsheet_parse.c	*/
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
#include "date_convert.h"
#include "station.h"
#include "datatype.h"
#include "units.h"
#include "hydrology.h"
#include "name_arg.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void setup_arg(		NAME_ARG *arg, int argc, char **argv );

void fetch_parameters(	char **filename,
			char **station,
			char **date_heading_label,
			char **two_lines_yn,
			char **time_column_yn,
			NAME_ARG *arg );

LIST *spreadsheet_parse_datatype_list(
					char *application_name,
					char *station_name,
					char *input_filespecification,
					char *date_heading_label,
					boolean two_lines );

void spreadsheet_parse_display(
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
	char *two_lines_yn;
	char *time_column_yn;
	boolean two_lines;
	boolean time_column;
	NAME_ARG *arg;
	char *application_name;
	char *begin_measurement_date = {0};
	char *end_measurement_date = {0};

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	arg = name_arg_new( argv[ 0 ] );

	setup_arg( arg, argc, argv );

	fetch_parameters(
			&input_filespecification,
			&station,
			&date_heading_label,
			&two_lines_yn,
			&time_column_yn,
			arg );

	two_lines = ( *two_lines_yn == 'y' );
	time_column = ( *time_column_yn == 'y' );

	hydrology_parse_begin_end_dates(
		&begin_measurement_date,
		&end_measurement_date,
		input_filespecification,
		date_heading_label );

	if ( begin_measurement_date
	&&   end_measurement_date
	&&   ( datatype_list =
			spreadsheet_parse_datatype_list(
				application_name,
				station,
				input_filespecification,
				date_heading_label,
				two_lines ) ) )
	{
		spreadsheet_parse_display(
			station,
			input_filespecification,
			date_heading_label,
			datatype_list,
			time_column );
	}

	return 0;

} /* main() */

void spreadsheet_parse_display(
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
	JULIAN *measurement_date_time_julian;
	DATE *measurement_date_time;

	measurement_date_time_julian = julian_new_julian( 0.0 );
	measurement_date_time = date_calloc();

	if ( !list_length( datatype_list ) ) return;

	if ( ! ( input_file = fopen( input_filespecification, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_filespecification );
		exit( 1 );
	}


	*measurement_time_string = '\0';
	timlib_reset_get_line_check_utf_16();

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

		strcpy( measurement_date_string,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			hydrology_format_measurement_date(
				/* ------------------------ */
				/* Out: assume stack memory */
				/* ------------------------ */
				measurement_time_string,
				/* -- */
				/* In */
				/* -- */
				measurement_date_string
					/* measurement_date_time_string */ ) );

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

		if ( !date_set_yyyy_mm_dd(
				measurement_date_time,
				measurement_date_string ) )
		{
			fprintf( stderr,
				 "Ignoring bad date in line %d: %s\n",
				 line_number,
				 input_buffer );
			fflush( stderr );
			continue;
		}

		if ( !date_set_time_hhmm(
				measurement_date_time,
				measurement_time_string ) )
		{
			fprintf( stderr,
				 "Ignoring bad time in line %d: %s\n",
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
			measurement_adjust_time_to_sequence(
				measurement_date_time_julian,
				VALID_FREQUENCY_TIME_SEQUENCE );

		list_rewind( datatype_list );

		do {
			datatype = list_get_pointer( datatype_list );

			if ( datatype->column_piece < 0 ) continue;

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

			fprintf(	stdout,
					"%s^%s^%s^%s^%.3lf\n",
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

} /* spreadsheet_parse_display() */

LIST *spreadsheet_parse_datatype_list(
				char *application_name,
				char *station_name,
				char *input_filespecification,
				char *date_heading_label,
				boolean two_lines )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	LIST *datatype_list;
	char datatype_name[ 128 ];
	char column_piece[ 128 ];
	DATATYPE *datatype;

	datatype_list = datatype_fetch_list( application_name );

	sprintf( sys_string,
"spreadsheet_parse_header file=\"%s\" station=\"%s\" date_heading_label=\"%s\" two_lines=%c",
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

		if ( ! ( datatype = datatype_list_seek(
				datatype_list,
				datatype_name ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot seek datatype_name = [%s]\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 datatype_name );
			exit( 1 );
		}

		datatype->column_piece = atoi( column_piece );
	}

	pclose( input_pipe );

	return datatype_list;

} /* spreadsheet_parse_datatype_list() */

void fetch_parameters(	char **filename,
			char **station,
			char **date_heading_label,
			char **two_lines_yn,
			char **time_column_yn,
			NAME_ARG *arg )
{
	*filename = fetch_arg( arg, "filename" );
	*station = fetch_arg( arg, "station" );
	*date_heading_label = fetch_arg( arg, "date_heading_label" );
	*two_lines_yn = fetch_arg( arg, "two_lines" );
	*time_column_yn = fetch_arg( arg, "time_column" );

} /* fetch_parameters() */

void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

        ticket = add_valid_option( arg, "filename" );
        ticket = add_valid_option( arg, "station" );

        ticket = add_valid_option( arg, "date_heading_label" );
        set_default_value( arg, ticket, "date" );

        ticket = add_valid_option( arg, "two_lines" );
	add_valid_value( arg, ticket, "yes" );
	add_valid_value( arg, ticket, "no" );
        set_default_value( arg, ticket, "no" );

        ticket = add_valid_option( arg, "time_column" );
	add_valid_value( arg, ticket, "yes" );
	add_valid_value( arg, ticket, "no" );
        set_default_value( arg, ticket, "yes" );

        ins_all( arg, argc, argv );

} /* setup_arg() */

