/* -------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/load_biscayne_ysi_data.c	*/
/* -------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "date_convert.h"
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
#include "julian.h"
#include "hydrology_library.h"
#include "appaserver_link_file.h"

/* Structures */
/* ---------- */
typedef struct
{
	char *datatype_name;
	int column_number;
} DATATYPE;

/* Constants */
/* --------- */
#define TEMPERATURE_HEADING		"Temp"
#define BOTTOM_TEMPERATURE_DATATYPE	"bottom_temperature"
#define SURFACE_TEMPERATURE_DATATYPE	"surface_temperature"
#define SALINITY_HEADING		"Sal"
#define SALINITY_DATATYPE		"salinity_raw"
#define CONDUCTIVITY_HEADING		"SpCond"
#define CONDUCTIVITY_DATATYPE		"conductivity"
#define DEPTH_HEADING			"Depth"
#define DEPTH_DATATYPE			"depth"
#define FEET_PER_METER			3.281
#define FILENAME_STEM			"load_ysi_bad"
#define PROMPT	"&lt;Left Click&gt; to view bad records or &lt;Right Click&gt; to save."

/* Prototypes */
/* ---------- */
boolean get_is_odd_station(		char *station );

LIST *with_input_buffer_get_datatype_list(
					char *input_buffer,
					boolean is_odd_station );

LIST *get_datatype_list(		char *input_filespecification,
					boolean is_odd_station );

DATATYPE *new_datatype(			char *datatype_name );

boolean datatype_exists(		char *application_name,
					char *datatype_name );

int load_ysi_filespecification(
					int *measurement_count,
					char *error_filespecification,
					char *application_name,
					char *input_filespecification,
					char *station,
					char really_yn,
					JULIAN *begin_date,
					JULIAN *end_time,
					char change_existing_data_yn );

boolean station_filename_synchronized(	char *station,
					char *input_filespecification );

int main( int argc, char **argv )
{
	char *application_name;
	char really_yn;
	char change_existing_data_yn;
	char *input_filespecification;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *input_directory;
	char *station;
	char *error_filespecification;
	char *error_ftp_filename;
	int line_count;
	int measurement_count = 0;
	char *process_name;
	int process_id = getpid();
	char *database_string = {0};
	char format_buffer[ 128 ];
	char *begin_date_string;
	char *begin_time_string;
	char *end_date_string;
	char *end_time_string;
	JULIAN *begin_date = {0};
	JULIAN *end_date = {0};
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( argc != 11 )
	{
		fprintf( stderr, 
"Usage: %s application process filename station begin_date begin_time end_date end_time change_existing_data_yn really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	input_filespecification = argv[ 3 ];
	station = argv[ 4 ];
	begin_date_string = argv[ 5 ];
	begin_time_string = argv[ 6 ];
	end_date_string = argv[ 7 ];
	end_time_string = argv[ 8 ];
	change_existing_data_yn = *argv[ 9 ];
	really_yn = *argv[ 10 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

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

	if ( !*station
	||   strcmp( station, "station" ) == 0
	||   strcmp( station, "biscayne_station" ) == 0 )
	{
		printf( "<h3>ERROR: Please choose a station.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !datatype_exists( application_name, BOTTOM_TEMPERATURE_DATATYPE ) )
	{
		printf(
"<h3>Error: the datatype bottom temperature has changed. It should be (%s).\n",
			BOTTOM_TEMPERATURE_DATATYPE );
		document_close();
		exit( 0 );
	}

	if ( !datatype_exists( application_name, SURFACE_TEMPERATURE_DATATYPE ))
	{
		printf(
"<h3>Error: the datatype bottom temperature has changed. It should be (%s).\n",
			SURFACE_TEMPERATURE_DATATYPE );
		document_close();
		exit( 0 );
	}

	if ( !datatype_exists( application_name, SALINITY_DATATYPE ) )
	{
		printf(
"<h3>Error: the datatype salinity has changed. It should be (%s).\n",
			SALINITY_DATATYPE );
		document_close();
		exit( 0 );
	}

	if ( !datatype_exists( application_name, DEPTH_DATATYPE ) )
	{
		printf(
"<h3>Error: the datatype depth has changed. It should be (%s).\n",
			DEPTH_DATATYPE );
		document_close();
		exit( 0 );
	}

	if ( !datatype_exists( application_name, CONDUCTIVITY_DATATYPE ) )
	{
		printf(
"<h3>Error: the datatype conductivity has changed. It should be (%s).\n",
			CONDUCTIVITY_DATATYPE );
		document_close();
		exit( 0 );
	}

	if ( !station_filename_synchronized(
					station,
					input_filespecification ) )
	{
		printf(
"<h3>Warning: the chosen station conflicts with the filename.</h3>\n" );
	}

	input_directory = basename_get_directory( input_filespecification );

	if ( !input_directory || !*input_directory )
	{
		printf(
"<h3>ERROR: Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
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

	if (	*begin_date_string
	&&	strcmp( begin_date_string, "begin_date" ) != 0 )
	{
		if ( !isdigit( *begin_time_string ) )
			begin_time_string = "0000";
		begin_date = 
			julian_yyyy_mm_dd_hhmm_new(	begin_date_string,
							begin_time_string );
	}

	if (	*end_date_string
	&&	strcmp( end_date_string, "end_date" ) != 0 )
	{
		if ( !isdigit( *end_time_string ) )
			end_time_string = "2359";
		end_date = 
			julian_yyyy_mm_dd_hhmm_new(	end_date_string,
							end_time_string );
	}

	line_count = load_ysi_filespecification(
			&measurement_count,
			error_filespecification,
			application_name,
			input_filespecification,
			station,
			really_yn,
			begin_date,
			end_date,
			change_existing_data_yn );

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

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

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
			JULIAN *begin_date,
			JULIAN *end_date,
			char change_existing_data_yn )
{
	FILE *input_pipe;
	char input_sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char format_buffer[ 128 ];
	FILE *error_file;
	int line_number = 0;
	FILE *insert_pipe = {0};
	FILE *update_pipe = {0};
	char *insert_field_list_string;
	char measurement_date_american[ 32 ];
	char measurement_date_international[ 32 ];
	char measurement_time[ 32 ];
	char measurement_value_string[ 32 ];
	double measurement_value;
	char sys_string[ 1024 ];
	LIST *datatype_list;
	DATATYPE *datatype;
	boolean is_odd_station;
	JULIAN *measurement_date_time = julian_new_julian( 0.0 );

	is_odd_station = get_is_odd_station( station );

	datatype_list = get_datatype_list(	input_filespecification,
						is_odd_station );

	if ( !list_length( datatype_list ) ) return 0;

	error_file = fopen( error_filespecification, "w" );
	if ( !error_file )
	{
		printf( "<p>ERROR: cannot open %s for write.\n",
			error_filespecification );
		document_close();
		exit( 1 );
	}

	insert_field_list_string =
"station,datatype,measurement_date,measurement_time,measurement_value";

	if ( really_yn == 'y' )
	{
		char *update_field_list_string;
		char *measurement_table_name;

		measurement_table_name =
			get_table_name(
				application_name,
				"measurement" );

		if ( change_existing_data_yn == 'y' )
		{
			sprintf( sys_string,
			 	"insert_statement.e %s %s '^'		|"
			 	"sql.e 2>&1				|"
			 	"grep -vi duplicate			|"
			 	"html_paragraph_wrapper.e	 	 ",
			 	measurement_table_name,
			 	insert_field_list_string );
		}
		else
		{
			sprintf( sys_string,
			 	"insert_statement.e %s %s '^'		|"
			 	"sql.e 2>&1				|"
			 	"cat >> %s			 	 ",
			 	measurement_table_name,
			 	insert_field_list_string,
				error_filespecification );
		}

		insert_pipe = popen( sys_string, "w" );

		if ( change_existing_data_yn == 'y' )
		{
			update_field_list_string =
			"station,datatype,measurement_date,measurement_time";

			sprintf( sys_string,
		 		"update_statement.e %s %s '^^'		|"
		 		"sql.e 2>&1				|"
				"html_paragraph_wrapper.e		 ",
		 		measurement_table_name,
		 		update_field_list_string );

			update_pipe = popen( sys_string, "w" );
		}
	}
	else
	{
		sprintf(sys_string,
	"queue_top_bottom_lines.e 20 | html_table.e '' \"%s\" '^' %s",
		 	insert_field_list_string,
		 	"left,left,left,left,right" );
		insert_pipe = popen( sys_string, "w" );
		fflush( stdout );
	}

	sprintf( input_sys_string,
		 "cat %s",
		 input_filespecification );

	input_pipe = popen( input_sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		line_number++;

		if ( !*input_buffer ) continue;
		if ( !isdigit( *input_buffer ) ) continue;

		/* Measurement date */
		/* ---------------- */
		column( measurement_date_american, 0, input_buffer );
		date_convert_source_american(
				measurement_date_international,
				international,
				measurement_date_american );

		if ( !date_convert_is_valid_international(
				measurement_date_international ) )
		{
			fprintf( error_file,
				 "Invalid date in line %d: %s\n",
				 line_number,
				 input_buffer );
			fflush( error_file );
			continue;
		}

		/* Measurement time */
		/* ---------------- */
		if ( !column( measurement_time, 1, input_buffer ) )
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
			measurement_adjust_time_to_sequence(
				measurement_date_time,
				VALID_FREQUENCY_TIME_SEQUENCE );

		if (	begin_date
		&&	measurement_date_time->current < begin_date->current )
		{
			fprintf( error_file,
				 "Ignored too early time in line %d: %s\n",
				 line_number,
				 input_buffer );
			fflush( error_file );
			continue;
		}

		if (	end_date
		&&	measurement_date_time->current > end_date->current )
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

			column(	measurement_value_string,
				datatype->column_number,
				input_buffer );

			measurement_value = atof( measurement_value_string );

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

			fprintf(	insert_pipe,
					"%s^%s^%s^%s^%.3lf\n",
					station,
					format_buffer,
					measurement_date_international,
					julian_display_hhmm(
						measurement_date_time->
							current ),
					measurement_value );

			if ( update_pipe )
			{
				fprintf( update_pipe,
				 	"%s^%s^%s^%s^measurement_value^%.3lf\n",
				 	station,
				 	datatype->datatype_name,
				 	measurement_date_international,
					julian_display_hhmm(
						measurement_date_time->
							current ),
				 	measurement_value );
			}

			(*measurement_count)++;

		} while( list_next( datatype_list ) );
	}

	pclose( insert_pipe );
	if ( update_pipe ) pclose( update_pipe );

	pclose( input_pipe );
	fclose( error_file );

	return line_number;

} /* load_ysi_filespecification() */

boolean station_filename_synchronized(
					char *station,
					char *input_filespecification )
{
	int station_number;
	int file_number;
	char *filename;
	char file_number_string[ 3 ];

	if ( instr( "BISC", station, 1 ) == -1 ) return 1;

	filename = basename_get_filename( input_filespecification );

	if ( strlen( filename ) < 3 ) return 1;

	*file_number_string = *filename;
	*(file_number_string + 1 ) = *(filename + 1);
	*(file_number_string + 2 ) = '\0';
	file_number = atoi( file_number_string );

	if ( !file_number ) return 1;

	station_number = atoi( station + 4 );

	if ( !station_number ) return 1;

	return ( station_number == file_number );
} /* station_filename_synchronized() */

boolean datatype_exists( char *application_name, char *datatype_name )
{
	char sys_string[ 1024 ];
	char where_clause[ 128 ];

	sprintf( where_clause, "datatype = '%s'", datatype_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=count			"
		 "			folder=datatype			"
		 "			where=\"%s\"			",
		 application_name,
		 where_clause );
	return atoi( pipe2string( sys_string ) );
} /* datatype_exists() */

DATATYPE *new_datatype( char *datatype_name )
{
	DATATYPE *datatype;

	if ( ! ( datatype = (DATATYPE *)calloc( 1, sizeof( DATATYPE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	datatype->datatype_name = datatype_name;
	return datatype;
} /* datatype_new() */

LIST *get_datatype_list(	char *input_filespecification,
				boolean is_odd_station )
{
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
		exit( 1 );
	}

	while( get_line( input_buffer, input_file ) )
	{
		if ( instr( "Date", input_buffer, 1 ) != -1 )
		{
			fclose( input_file );
			return with_input_buffer_get_datatype_list(
						input_buffer,
						is_odd_station );
		}
	}
	fclose( input_file );
	return (LIST *)0;
} /* get_datatype_list() */

LIST *with_input_buffer_get_datatype_list(	char *input_buffer,
						boolean is_odd_station )
{
	LIST *datatype_list = list_new();
	DATATYPE *datatype;
	char datatype_heading[ 128 ];
	int column_number;

	for(	column_number = 0;
		column(	datatype_heading,
			column_number,
			input_buffer );
		column_number++ )
	{
		if ( strcmp( datatype_heading, "Date" ) == 0 )
		{
			continue;
		}
		else
		if ( strcmp( datatype_heading, "Time" ) == 0 )
		{
			continue;
		}
		else
		if ( strcmp( datatype_heading, "Battery" ) == 0 )
		{
			continue;
		}
		else
		if ( strcmp(	datatype_heading,
				TEMPERATURE_HEADING ) == 0 )
		{
			if ( is_odd_station )
			{
				datatype =
					new_datatype(
					SURFACE_TEMPERATURE_DATATYPE );
			}
			else
			{
				datatype =
					new_datatype(
					BOTTOM_TEMPERATURE_DATATYPE );
			}
			datatype->column_number = column_number;
		}
		else
		if ( strcmp(	datatype_heading,
				DEPTH_HEADING ) == 0 )
		{
			datatype = new_datatype( DEPTH_DATATYPE );
			datatype->column_number = column_number;
		}
		else
		if ( strcmp(	datatype_heading,
				SALINITY_HEADING ) == 0 )
		{
			datatype = new_datatype( SALINITY_DATATYPE );
			datatype->column_number = column_number;
		}
		else
		if ( strcmp(	datatype_heading,
				CONDUCTIVITY_HEADING ) == 0 )
		{
			datatype = new_datatype( CONDUCTIVITY_DATATYPE );
			datatype->column_number = column_number;
		}
		else
		{
			printf(
			"<h3>Error: cannot recognize heading of %s.</h3>\n",
				datatype_heading );
			document_close();
			exit( 0 );
		}

		list_append_pointer( datatype_list, datatype );
	}
	return datatype_list;
} /* with_input_buffer_get_datatype_list() */

boolean get_is_odd_station( char *station )
{
	char last_character_string[ 2 ];
	int last_character_integer;
	int str_len = strlen( station );

	if ( !str_len ) return 0;

	*last_character_string = *(station + str_len - 1 );
	*(last_character_string + 1) = '\0';

	if ( isalpha( *last_character_string ) )
	{
		return 0;
	}
	else
	{
		last_character_integer = atoi( last_character_string );
		return (last_character_integer % 2);
	}

} /* get_is_odd_station() */

