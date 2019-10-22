/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/load_ysi_data.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "date_convert.h"
#include "julian.h"
#include "query.h"
#include "basename.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "column.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "date.h"
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
typedef struct
{
	int special_code;
	char *replacement_string;
} SPECIAL_CODE_STRUCTURE;

/* Constants */
/* --------- */
#define STATION_LABEL			"Site"

/* Prototypes */
/* ---------- */
boolean get_file_begin_end_dates(	JULIAN **file_begin_date,
					JULIAN **file_end_date,
					char *input_filespecification );

/* Returns measurement_count */
/* ------------------------- */
int load_ysi_filespecification(
					char *input_filespecification,
					char *station,
					char execute_yn,
					char is_exo_yn,
					char *begin_date_string,
					char *begin_time_string,
					char *end_date_string,
					char *end_time_string,
					JULIAN *file_begin_date,
					JULIAN *file_end_date,
					char *appaserver_data_directory );

char *station_fetch(			char *application_name,
					char *input_filespecification );

char *station_label_fetch(		char *application_name,
					char *input_filespecification );

/* Check column piece=3 */
/* -------------------- */
char *station_column_fetch(		char *application_name,
					char *input_filespecification );

int main( int argc, char **argv )
{
	char *application_name;
	char is_exo_yn;
	char execute_yn;
	char *input_filespecification;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *input_directory;
	char *station;
	int measurement_count = 0;
	char *process_name;
	char format_buffer[ 256 ];
	char *begin_date_string;
	char *begin_time_string;
	char *end_date_string;
	char *end_time_string;
	JULIAN *file_begin_date = {0};
	JULIAN *file_end_date = {0};

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 10 )
	{
		fprintf( stderr, 
"Usage: %s process filename station begin_date begin_time end_date end_time is_exo_yn execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	input_filespecification = argv[ 2 ];
	station = argv[ 3 ];
	begin_date_string = argv[ 4 ];
	begin_time_string = argv[ 5 ];
	end_date_string = argv[ 6 ];
	end_time_string = argv[ 7 ];
	is_exo_yn = *argv[ 8 ];
	execute_yn = *argv[ 9 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
		document->application_name,
		document->onload_control_string );

	printf( "<h1>%s</h1>\n",
	 	format_initial_capital( format_buffer, process_name ) );

	input_directory = basename_get_directory( input_filespecification );

	if ( !input_directory || !*input_directory )
	{
		printf( "<h3>ERROR: Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !*station
	||   strcmp( station, "station" ) == 0 )
	{
		if ( ! ( station =
				station_fetch(
					application_name,
					input_filespecification ) ) )
		{
			printf(
			"<h3>ERROR: Could not identify the station.</h3>\n" );
			document_close();
			exit( 1 );
		}
	}

	if (	!*begin_date_string
	||	strcmp( begin_date_string, "begin_date" ) == 0 )
	{
		printf(
		"<h3>Please enter in a begin date.</h3>\n" );
		document_close();
		exit( 1 );
	}

	if ( !isdigit( *begin_time_string ) ) begin_time_string = "0000";

	if (	!*end_date_string
	||	strcmp( end_date_string, "end_date" ) == 0 )
	{
		printf(
		"<h3>Please enter in an end date.</h3>\n" );
		document_close();
		exit( 1 );
	}

	if ( !isdigit( *end_time_string ) ) end_time_string = "2359";

	if ( !get_file_begin_end_dates(	&file_begin_date,
					&file_end_date,
					input_filespecification ) )
	{
		printf( "<h3>Error: invalid YSI file format.</h3>\n" );
		document_close();
		exit( 1 );
	}

	measurement_count =
		load_ysi_filespecification(
			input_filespecification,
			station,
			execute_yn,
			is_exo_yn,
			begin_date_string,
			begin_time_string,
			end_date_string,
			end_time_string,
			file_begin_date,
			file_end_date,
			appaserver_parameter_file->
				appaserver_data_directory );

	if ( execute_yn == 'y' )
		printf( "<p>Inserted %d measurements.\n",
			measurement_count );
	else
		printf( "<p>Did not insert %d measurements.\n",
			measurement_count );

	if ( execute_yn == 'y' )
	{
		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}

	document_close();

	return 0;

} /* main() */

/* Returns measurement_count */
/* ------------------------- */
int load_ysi_filespecification(
			char *input_filespecification,
			char *station,
			char execute_yn,
			char is_exo_yn,
			char *begin_date_string,
			char *begin_time_string,
			char *end_date_string,
			char *end_time_string,
			JULIAN *file_begin_date,
			JULIAN *file_end_date,
			char *appaserver_data_directory )
{
	char sys_string[ 2048 ];
	char bad_parse[ 128 ];
	char bad_time[ 128 ];
	char bad_range[ 128 ];
	char bad_frequency[ 128 ];
	char bad_insert[ 128 ];
	char *two_lines;
	char *date_heading_label;
	pid_t pid;

	pid = getpid();

	sprintf( bad_parse, "%s/parse_%d.dat", appaserver_data_directory, pid );
	sprintf( bad_time, "%s/time_%d.dat", appaserver_data_directory, pid );
	sprintf( bad_range, "%s/range_%d.dat", appaserver_data_directory, pid );
	sprintf( bad_frequency, "%s/reject_%d.dat", appaserver_data_directory, pid );
	sprintf( bad_insert, "%s/insert_%d.dat", appaserver_data_directory, pid );

	if ( is_exo_yn != 'y' )
		two_lines = "yes";
	else
		two_lines = "no";

	date_heading_label = "Date";

	sprintf( sys_string,
"spreadsheet_parse file=\"%s\" station=\"%s\" date=%s two=%s 2>%s		|"
"measurement_adjust_time_to_sequence 2>%s					|"
"measurement_date_within_range	begin_date=%s					 "
"				begin_time=%s					 "
"				end_date=%s					 "
"				end_time=%s 2>%s				|"
"measurement_frequency_reject begin=%s end=%s 2>%s				|"
"measurement_insert '^' y %s %s %c 2>%s						|"
"cat										 ",
		 input_filespecification,
		 station,
		 date_heading_label,
		 two_lines,
		 bad_parse,
		 bad_time,
		 begin_date_string,
		 begin_time_string,
		 end_date_string,
		 end_time_string,
		 bad_range,
		 julian_display_yyyy_mm_dd( file_begin_date->current ),
		 julian_display_yyyy_mm_dd( file_end_date->current ),
		 bad_frequency,
		 julian_display_yyyy_mm_dd( file_begin_date->current ),
		 julian_display_yyyy_mm_dd( file_end_date->current ),
		 execute_yn,
		 bad_insert );

	if ( system( sys_string ) ) {};

	return 0;

} /* load_ysi_filespecification() */

char *station_fetch(		char *application_name,
				char *input_filespecification )
{
	char *station;

	if ( ( station =
		station_label_fetch(
			application_name,
			input_filespecification ) ) )
	{
		return station;
	}
	else
	{
		/* Check column piece=3 */
		/* -------------------- */
		return station_column_fetch(
			application_name,
			input_filespecification );
	}

} /* station_fetch() */

/* Check column piece=3 */
/* -------------------- */
char *station_column_fetch(	char *application_name,
				char *input_filespecification )
{
	char station[ 128 ];
	FILE *input_file;
	char input_buffer[ 1024 ];
	boolean found_header = 0;

	if ( ! ( input_file = fopen( input_filespecification, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_filespecification );
		return 0;
	}

	timlib_reset_get_line_check_utf_16();

	while( timlib_get_line( input_buffer, input_file, 1024 ) )
	{
		if ( !piece( station, ',', input_buffer, 3 ) )
		{
			continue;
		}

		if ( found_header )
		{
			fclose( input_file );
			timlib_reset_get_line_check_utf_16();

			if ( !station_exists(
				application_name,
				station ) )
			{
				printf(
		"<h3>Error: station = (%s) doesn't exist in STATION.</h3>\n",
					station );

				return (char *)0;
			}
			else
			{
				return strdup( station );
			}
		}

		if ( timlib_strncmp( station, STATION_LABEL ) == 0 )
		{
			found_header = 1;
			continue;
		}
	}

	fclose( input_file );
	timlib_reset_get_line_check_utf_16();

	return (char *)0;

} /* station_column_fetch() */

char *station_label_fetch(	char *application_name,
				char *input_filespecification )
{
	char station[ 128 ];
	FILE *input_file;
	char input_buffer[ 1024 ];

	if ( ! ( input_file = fopen( input_filespecification, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_filespecification );
		return 0;
	}

	timlib_reset_get_line_check_utf_16();

	while( timlib_get_line( input_buffer, input_file, 1024 ) )
	{
		if ( timlib_strncmp( input_buffer, STATION_LABEL ) != 0 )
			continue;

		fclose( input_file );

		timlib_reset_get_line_check_utf_16();

		if ( timlib_character_exists(
			input_buffer,
			',' ) )
		{
			piece( station, ',', input_buffer, 1 );
		}
		else
		{
			strcpy(	station,
				input_buffer + strlen( STATION_LABEL ) + 1 );

			trim( station );
		}

		if ( !station_exists(
				application_name,
				station ) )
		{
			printf(
		"<h3>Error: station = (%s) doesn't exist in STATION.</h3>\n",
				station );

			fclose( input_file );
			timlib_reset_get_line_check_utf_16();

			return (char *)0;
		}

		return strdup( station );
	}

	fclose( input_file );

	timlib_reset_get_line_check_utf_16();

	return (char *)0;

} /* station_label_fetch() */

boolean get_file_begin_end_dates(	JULIAN **file_begin_date,
					JULIAN **file_end_date,
					char *input_filespecification )
{
	char measurement_date_american[ 128 ];
	char measurement_time[ 16 ];
	char measurement_date_international[ 128 ];
	FILE *input_file;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char tmp_filename[ 128 ];
	char sys_string[ 1024 ];
	int return_value = 0;
	char input_label[ 128 ];

	if ( ! ( input_file = fopen( input_filespecification, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_filespecification );
		return 0;
	}

	sprintf( tmp_filename, "/tmp/load_ysi_data_%d.tmp", getpid() );
	sprintf( sys_string,
		 "date_min_max.e 0 ',' mysql > %s",
		 tmp_filename);
	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_file ) )
	{
		if ( isdigit( *input_buffer ) )
		{
			/* Measurement time */
			/* ---------------- */
			piece(	measurement_date_american,
				',',
				input_buffer,
				0 );

			date_convert_source_american(
					measurement_date_international,
					international,
					measurement_date_american );

			if ( !date_convert_is_valid_international(
					measurement_date_international ) )
			{
				continue;
			}
	
			/* Measurement time */
			/* ---------------- */
			if ( !piece( measurement_time, ',', input_buffer, 1 ) )
			{
				continue;
			}
	
			piece_inverse( measurement_time, ':', 2 );
			trim_character(	measurement_time,
					':',
					measurement_time );
	
			if ( strlen( measurement_time ) != 4 )
			{
				continue;
			}

			fprintf(	output_pipe,
					"%s:%s\n",
					measurement_date_international,
					measurement_time );
		}
	}

	fclose( input_file );

	timlib_reset_get_line_check_utf_16();

	pclose( output_pipe );

	if ( ! ( input_file = fopen( tmp_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 tmp_filename );
		return 0;
	}

	while( get_line( input_buffer, input_file ) )
	{
		piece( input_label, ':', input_buffer, 0 );

		if ( strcmp( input_label, DATE_MINIMUM ) == 0 )
		{
			piece(	measurement_date_international,
				':',
				input_buffer,
				1 );

			if ( !*measurement_date_international )
			{
				*file_begin_date =
					julian_yyyy_mm_dd_hhmm_new(
						"0000-00-00",
						"0000" );
			}
			else
			{
				piece(	measurement_time,
					':',
					input_buffer,
					2 );

				*file_begin_date =
					julian_yyyy_mm_dd_hhmm_new(
						measurement_date_international,
						measurement_time );
			}
		}

		if ( strcmp( input_label, DATE_MAXIMUM ) == 0 )
		{
			piece(	measurement_date_international,
				':',
				input_buffer,
				1 );

			if ( !*measurement_date_international )
			{
				*file_end_date =
					julian_yyyy_mm_dd_hhmm_new(
						"2999-12-31",
						"0000" );
			}
			else
			{
				piece(	measurement_time,
					':',
					input_buffer,
					2 );

				*file_end_date =
					julian_yyyy_mm_dd_hhmm_new(
						measurement_date_international,
						measurement_time );
			}

			return_value = 1;
			break;
		}
	}

	fclose( input_file );

	timlib_reset_get_line_check_utf_16();

	sprintf( sys_string, "rm -f %s", tmp_filename );
	if ( system( sys_string ) ) {};

	return return_value;

} /* get_file_begin_end_dates() */

