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

void load_biscayne_ysi_data(
					char *application_name,
					char *filename,
					char *station,
					char *begin_date_string,
					char *begin_time_string,
					char *end_date_string,
					char *end_time_string,
					boolean execute );

boolean station_filename_synchronized(	char *station,
					char *filename );

int main( int argc, char **argv )
{
	char *application_name;
	boolean execute;
	char *filename;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *station;
	char *process_name;
	char format_buffer[ 128 ];
	char *begin_date_string;
	char *begin_time_string;
	char *end_date_string;
	char *end_time_string;

	/* Exits if fails */
	/* -------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 9 )
	{
		fprintf( stderr, 
"Usage: %s process filename station begin_date begin_time end_date end_time execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	filename = argv[ 2 ];
	station = argv[ 3 ];
	begin_date_string = argv[ 4 ];
	begin_time_string = argv[ 5 ];
	end_date_string = argv[ 6 ];
	end_time_string = argv[ 7 ];
	execute = ( *argv[ 8 ] == 'y' );

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	printf( "<h1>%s</h1>\n",
	 	format_initial_capital( format_buffer, process_name ) );

	if ( !*station
	||   strcmp( station, "station" ) == 0
	||   strcmp( station, "biscayne_station" ) == 0 )
	{
		printf( "<h3>Error: Please choose a station.</h3>\n" );
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
		filename ) )
	{
		printf(
"<h3>Warning: the chosen station conflicts with the filename.</h3>\n" );
	}

	if ( !*filename || strcmp( filename, "filename" ) == 0 )
	{
		printf(
"<h3>Error: Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	load_biscayne_ysi_data(
			application_name,
			filename,
			station,
			begin_date_string,
			begin_time_string,
			end_date_string,
			end_time_string,
			execute );

	if ( execute )
	{
		printf( "<p>Process complete.\n" );

		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}
	else
	{
		printf( "<p>Process not executed.\n" );
	}

	document_close();

	return 0;

} /* main() */

void load_biscayne_ysi_data(
			char *application_name,
			char *filename,
			char *station,
			char *begin_date_string,
			char *begin_time_string,
			char *end_date_string,
			char *end_time_string,
			boolean execute )
{

} /* void load_biscayne_ysi_data */

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

