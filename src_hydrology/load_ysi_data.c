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
#include "appaserver_link_file.h"

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
#define FEET_PER_METER			3.281
#define FILENAME_STEM			"load_ysi_bad"
#define PROMPT	"&lt;Left Click&gt; to view bad records or &lt;Right Click&gt; to save."

/* Prototypes */
/* ---------- */
char *station_fetch(			char *application_name,
					char *input_filespecification );

char *station_label_fetch(		char *application_name,
					char *input_filespecification );

/* Check column piece=3 */
/* -------------------- */
char *station_column_fetch(		char *application_name,
					char *input_filespecification );

boolean get_file_begin_end_dates(	JULIAN **file_begin_date,
					JULIAN **file_end_date,
					char *input_filespecification );

LIST *input_buffer_get_datatype_list(	char *application_name,
					char *station_name,
					char *first_line,
					char *second_line );

LIST *get_datatype_list(		char **error_message,
					char *application_name,
					char *station_name,
					char *input_filespecification,
					boolean is_exo );

int load_ysi_filespecification(
					int *measurement_count,
					char *error_filespecification,
					char *application_name,
					char *input_filespecification,
					char *station,
					char really_yn,
					JULIAN *begin_date,
					JULIAN *end_time,
					LIST *datatype_list );

int main( int argc, char **argv )
{
	char *application_name;
	char is_exo_yn;
	char really_yn;
	char *input_filespecification;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *input_directory;
	char *station;
	char *error_filespecification;
	char *error_ftp_filename;
	int line_count = 0;
	int measurement_count = 0;
	char *process_name;
	int process_id = getpid();
	char format_buffer[ 128 ];
	char *begin_date_string;
	char *begin_time_string;
	char *end_date_string;
	char *end_time_string;
	JULIAN *file_begin_date = {0};
	JULIAN *file_end_date = {0};
	JULIAN *input_begin_date = {0};
	JULIAN *input_end_date = {0};
	LIST *datatype_list;
	char *error_message = {0};
	APPASERVER_LINK_FILE *appaserver_link_file;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 12 )
	{
		fprintf( stderr, 
"Usage: %s ignored process filename station begin_date begin_time end_date end_time ignored is_exo_yn really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	/* application_name = argv[ 1 ]; */
	process_name = argv[ 2 ];
	input_filespecification = argv[ 3 ];
	station = argv[ 4 ];
	begin_date_string = argv[ 5 ];
	begin_time_string = argv[ 6 ];
	end_date_string = argv[ 7 ];
	end_time_string = argv[ 8 ];
	/* change_existing_data_yn = *argv[ 9 ]; */
	is_exo_yn = *argv[ 10 ];
	really_yn = *argv[ 11 ];

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

	if ( ! ( datatype_list = get_datatype_list(
					&error_message,
					application_name,
					station,
					input_filespecification,
					( is_exo_yn == 'y' ) ) ) )
	{
		printf( "<h3>Error: %s</h3>\n", error_message );
		document_close();
		exit( 1 );
	}

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->
				document_root,
			FILENAME_STEM,
			application_name,
			process_id,
			(char *)0 /* session */,
			"txt" );

	error_filespecification =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	error_ftp_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	if ( !get_file_begin_end_dates(	&file_begin_date,
					&file_end_date,
					input_filespecification ) )
	{
		printf( "<h3>Error: invalid YSI file format.</h3>\n" );
		document_close();
		exit( 1 );
	}

	if (	*begin_date_string
	&&	strcmp( begin_date_string, "begin_date" ) != 0 )
	{
		if ( !isdigit( *begin_time_string ) )
			begin_time_string = "0000";

		input_begin_date = 
			julian_yyyy_mm_dd_hhmm_new(	begin_date_string,
							begin_time_string );
	}
	else
	{
		input_begin_date = file_begin_date;
	}

	if (	*end_date_string
	&&	strcmp( end_date_string, "end_date" ) != 0 )
	{
		if ( !isdigit( *end_time_string ) )
			end_time_string = "2359";

		input_end_date = 
			julian_yyyy_mm_dd_hhmm_new(	end_date_string,
							end_time_string );
	}
	else
	{
		input_end_date = file_end_date;
	}

	if ( ( !double_virtually_same(	input_begin_date->current,
					file_begin_date->current )
	||     !double_virtually_same(	input_end_date->current,
					file_end_date->current ) )
	&& ( input_begin_date->current < file_begin_date->current
	||   input_end_date->current > file_end_date->current ) )
	{
		printf(
"<h3>Error: the date range exceeds the file range of %s:%s and %s:%s</h3>\n",
			julian_display_yyyy_mm_dd( file_begin_date->current ),
			julian_display_hhmm( file_begin_date->current ),
			julian_display_yyyy_mm_dd( file_end_date->current ),
			julian_display_hhmm( file_end_date->current ) );
		document_close();
		exit( 1 );
	}

	line_count =
		load_ysi_filespecification(
			&measurement_count,
			error_filespecification,
			application_name,
			input_filespecification,
			station,
			really_yn,
			input_begin_date,
			input_end_date,
			datatype_list );

	if ( really_yn == 'y' )
		printf( "<p>Processed %d lines.<p>Processed %d measurements.\n",
			line_count,
			measurement_count );
	else
		printf( "<p>Did not process %d lines.\n", line_count );

	if ( timlib_file_populated( error_filespecification ) )
	{
		printf( "<br>" );
		appaserver_library_output_ftp_prompt(
				error_ftp_filename, 
				PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}

	if ( really_yn == 'y' )
	{
		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}

	document_close();

	exit( 0 );
} /* main() */

int load_ysi_filespecification(
			int *measurement_count,
			char *error_filespecification,
			char *application_name,
			char *input_filespecification,
			char *station,
			char really_yn,
			JULIAN *input_begin_date,
			JULIAN *input_end_date,
			LIST *datatype_list )
{
	FILE *input_file;
	char input_buffer[ 1024 ];
	char format_buffer[ 128 ];
	FILE *error_file;
	int line_number = 0;
	FILE *measurement_insert_pipe = {0};
	FILE *station_datatype_insert_pipe = {0};
	char *measurement_insert_field_list_string;
	char *station_datatype_insert_field_list_string;
	char measurement_date_american[ 32 ];
	char measurement_date_international[ 32 ];
	char measurement_time[ 32 ];
	char measurement_value_string[ 32 ];
	double measurement_value;
	char sys_string[ 1024 ];
	DATATYPE *datatype;
	JULIAN *measurement_date_time = julian_new_julian( 0.0 );

	if ( !list_length( datatype_list ) ) return 0;

	error_file = fopen( error_filespecification, "w" );
	if ( !error_file )
	{
		printf( "<p>ERROR: cannot open %s for write.\n",
			error_filespecification );
		document_close();
		exit( 1 );
	}

	measurement_insert_field_list_string =
"station,datatype,measurement_date,measurement_time,measurement_value";

	station_datatype_insert_field_list_string =
"station,datatype";

	if ( really_yn == 'y' )
	{
		char *table_name;

		table_name =
			get_table_name(
				application_name,
				"measurement" );

		sprintf(sys_string,
	"insert_statement.e table=%s field=%s del='^' replace=n	|"
		 	"sql.e 2>&1				|"
		 	"cat >> %s			 	 ",
		 	table_name,
		 	measurement_insert_field_list_string,
			error_filespecification );

		measurement_insert_pipe = popen( sys_string, "w" );

		table_name =
			get_table_name(
				application_name,
				"station_datatype" );

		sprintf(sys_string,
		 	"sort -u				|"
			"insert_statement.e %s %s '^'		|"
		 	"sql.e >/dev/null 2>&1 			 ",
		 	table_name,
		 	station_datatype_insert_field_list_string );

		station_datatype_insert_pipe = popen( sys_string, "w" );

	}
	else
	{
		sprintf(sys_string,
	"queue_top_bottom_lines.e 20 | html_table.e '' \"%s\" '^' %s",
		 	measurement_insert_field_list_string,
		 	"left,left,left,left,right" );
		measurement_insert_pipe = popen( sys_string, "w" );
		fflush( stdout );
	}

	input_file = fopen( input_filespecification, "r" );

	timlib_reset_get_line_check_utf_16();

	while( timlib_get_line( input_buffer, input_file, 1024 ) )
	{
		line_number++;

		if ( !*input_buffer ) continue;

		if ( !isdigit( *input_buffer )
		&&   *input_buffer != '"' )
		{
			continue;
		}

		if ( !timlib_character_exists( input_buffer, ',' ) )
		{
			fprintf( error_file,
				 "No comma in line %d: %s\n",
				 line_number,
				 input_buffer );
			fflush( error_file );
			continue;
		}

		/* Measurement date */
		/* ---------------- */
		piece_quoted(	measurement_date_american,
				',',
				input_buffer,
				0,
				'"' );

		date_convert_source_american(
				measurement_date_international,
				international,
				measurement_date_american );

		if ( !date_convert_is_valid_international(
				measurement_date_international ) )
		{
			fprintf( error_file,
				 "Ignoring line %d: %s\n",
				 line_number,
				 input_buffer );
			fflush( error_file );
			continue;
		}

		/* Measurement time */
		/* ---------------- */
		if ( ! piece_quoted(	measurement_time,
					',',
					input_buffer,
					1,
					'"' ) )
		{
			fprintf( error_file,
				 "Invalid time in line %d: %s\n",
				 line_number,
				 input_buffer );
			fflush( error_file );
			continue;
		}

		piece_inverse( measurement_time, ':', 2 );
		trim_character( measurement_time, ':', measurement_time );

		if ( strlen( measurement_time ) != 4 )
		{
			fprintf( error_file,
				 "Invalid time in line %d: %s\n",
				 line_number,
				 input_buffer );
			fflush( error_file );
			continue;
		}

		julian_set_yyyy_mm_dd_hhmm(
					measurement_date_time,
					measurement_date_international,
					measurement_time );

		measurement_date_time =
			hydrology_library_adjust_time_to_sequence(
				measurement_date_time,
				VALID_FREQUENCY_TIME_SEQUENCE );

		if (	input_begin_date
		&&      !double_virtually_same(
				measurement_date_time->current,
				input_begin_date->current )
		&&	measurement_date_time->current <
			input_begin_date->current )
		{
			fprintf( error_file,
				 "Ignored too early time in line %d: %s\n",
				 line_number,
				 input_buffer );
			fflush( error_file );
			continue;
		}

		if (	input_end_date
		&&      !double_virtually_same(
				measurement_date_time->current,
				input_end_date->current )
		&&	measurement_date_time->current >
			input_end_date->current )
		{
			fprintf( error_file,
				 "Ignored too late time in line %d: %s\n",
				 line_number,
				 input_buffer );
			fflush( error_file );
			continue;
		}

		list_rewind( datatype_list );

		do {
			datatype = list_get_pointer( datatype_list );

			if ( !piece(	measurement_value_string,
					',',
					input_buffer,
					datatype->column_piece ) )
			{
				fprintf( error_file,
				 "Cannot extract datatype %s in line %d: %s\n",
				 	 datatype->datatype_name,
				 	 line_number,
				 	 input_buffer );
				fflush( error_file );
				continue;
			}

			measurement_value = atof( measurement_value_string );

			if ( datatype->set_negative_values_to_zero
			&&   measurement_value < 0.0 )
			{
				measurement_value = 0.0;
			}

			if ( really_yn == 'y' )
			{
				strcpy(	format_buffer,
					datatype->datatype_name );
			}
			else
			{
				format_initial_capital(
					format_buffer,
					datatype->datatype_name );
			}

			fprintf(	measurement_insert_pipe,
					"%s^%s^%s^%s^%.3lf\n",
					station,
					format_buffer,
					measurement_date_international,
					julian_display_hhmm(
						measurement_date_time->
							current ),
					measurement_value );

			if ( really_yn == 'y' )
			{
				fprintf(
					station_datatype_insert_pipe,
					"%s^%s\n",
					station,
					datatype->datatype_name );
			}

			(*measurement_count)++;

		} while( list_next( datatype_list ) );
	}

	timlib_reset_get_line_check_utf_16();

	pclose( measurement_insert_pipe );

	if ( station_datatype_insert_pipe )
		pclose( station_datatype_insert_pipe );

	fclose( input_file );

	timlib_reset_get_line_check_utf_16();

	fclose( error_file );

	return line_number;

} /* load_ysi_filespecification() */

LIST *get_datatype_list(	char **error_message,
				char *application_name,
				char *station_name,
				char *input_filespecification,
				boolean is_exo )
{
	FILE *input_file;
	char first_line[ 1024 ];
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
		exit( 1 );
	}

	*second_line = '\0';

	timlib_reset_get_line_check_utf_16();

	while( timlib_get_line( first_line, input_file, 1024 ) )
	{
		if ( instr( "Date", first_line, 1 ) != -1 )
		{
			if ( !is_exo )
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
					first_line,
					second_line );

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

} /* get_datatype_list() */

/* --------------------------- */
/* Sets datatype->column_piece */
/* --------------------------- */
LIST *input_buffer_get_datatype_list(	char *application_name,
					char *station_name,
					char *first_line,
					char *second_line )
{
	LIST *datatype_list;
	DATATYPE *datatype;
	char datatype_heading_first_line[ 128 ];
	char datatype_heading_second_line[ 128 ];
	char two_line_datatype_heading[ 256 ];
	int column_piece;
	HYDROLOGY *hydrology;
	STATION *station;

	hydrology = hydrology_new();

	station =
		station_get_or_set_station(
			hydrology->input.station_list,
			application_name,
			station_name );

	datatype_list = list_new();
	*datatype_heading_second_line = '\0';

	for(	column_piece = 0;
		piece_quoted(	datatype_heading_first_line,
				',',
				first_line,
				column_piece,
				'"' );
		column_piece++ )
	{
		if ( !*datatype_heading_first_line ) continue;

		if ( strcmp( datatype_heading_first_line, "Date" ) == 0 )
		{
			continue;
		}
		else
		if ( strcmp( datatype_heading_first_line, "Time" ) == 0 )
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

			*file_begin_date =
				hydrology_library_adjust_time_to_sequence(
					*file_begin_date,
					VALID_FREQUENCY_TIME_SEQUENCE );
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

			*file_end_date =
				hydrology_library_adjust_time_to_sequence(
					*file_end_date,
					VALID_FREQUENCY_TIME_SEQUENCE );

			return_value = 1;
		}

	}

	fclose( input_file );

	timlib_reset_get_line_check_utf_16();

	sprintf( sys_string, "rm -f %s", tmp_filename );
	if ( system( sys_string ) ) {};

	return return_value;

} /* get_file_begin_end_dates() */

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

