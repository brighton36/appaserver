/* ---------------------------------------------------	*/
/* src_waterquality/water_quality.c			*/
/* ---------------------------------------------------	*/
/* This is the library for the water quality project.	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "hashtbl.h"
#include "appaserver_library.h"
#include "water_quality.h"

char *water_quality_get_project_name(
				char *application_name,
				char *project_code )
{
	static LIST *water_project_record_list = {0};
	static char project_name[ 128 ] = {0};
	char local_project_code[ 16 ];
	char *record;

	if ( !water_project_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "project_code,project_name";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=water_project",
		application_name,
		select );

		water_project_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( water_project_record_list ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: empty water_project_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		record = list_get_pointer( water_project_record_list );

		piece(	local_project_code,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		piece(	project_name,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		if ( strcmp(	local_project_code,
				project_code ) == 0 )
		{
			return project_name;
		}

	} while( list_next( water_project_record_list ) );

	return (char *)0;

} /* water_quality_get_project_name() */

char *water_quality_get_parameter_name(
				char **matrix,
				char *application_name,
				char *parameter_code )
{
	static LIST *water_parameter_record_list = {0};
	static char parameter_name[ 128 ] = {0};
	static char local_matrix[ 128 ] = {0};
	char local_lab_test_code[ 16 ];
	char *record;

	if ( !water_parameter_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "lab_test_code,parameter,matrix";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=parameter",
		application_name,
		select );

		water_parameter_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( water_parameter_record_list ) ) return (char *)0;

	do {
		record = list_get_pointer( water_parameter_record_list );

		piece(	local_lab_test_code,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		if ( strcmp(	local_lab_test_code,
				parameter_code ) == 0 )
		{
			piece(	parameter_name,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			piece(	local_matrix,
				FOLDER_DATA_DELIMITER,
				record,
				2 );

			*matrix = local_matrix;

			return parameter_name;
		}

	} while( list_next( water_parameter_record_list ) );

	return (char *)0;

} /* water_quality_get_parameter_name() */

WATER_QUALITY *water_quality_new(	char *application_name,
					char *project_name )
{
	WATER_QUALITY *w;

	if ( ! ( w = calloc( 1, sizeof( WATER_QUALITY ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	w->input.parameter_name_list =
		water_fetch_parameter_name_list(
			application_name );

	w->input.unit_name_list =
		water_fetch_unit_name_list(
			application_name );

	w->input.parameter_alias_list =
		water_fetch_parameter_alias_list(
			application_name );

	w->input.unit_alias_list =
		water_fetch_unit_alias_list(
			application_name );

	w->parameter_unit_alias_list =
		water_get_parameter_unit_alias_list(
			w->input.parameter_name_list,
			w->input.unit_name_list,
			w->input.parameter_alias_list,
			w->input.unit_alias_list );

	w->input.water_project =
		water_project_new(	application_name,
					project_name );

	return w;

} /* water_quality_new() */

WATER_PROJECT *water_project_new(	char *application_name,
					char *project_name )
{
	WATER_PROJECT *w;

	if ( ! ( w = calloc( 1, sizeof( WATER_PROJECT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	w->parameter_unit_list =
		water_fetch_parameter_unit_list(
			application_name,
			project_name );

	w->station_list =
		water_fetch_station_list(
			application_name,
			project_name );

	return w;
} /* water_project_new() */

STATION_PARAMETER *water_new_station_parameter( void )
{
	STATION_PARAMETER *w;

	if ( ! ( w = calloc( 1, sizeof( STATION_PARAMETER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return w;
} /* water_new_station_parameter() */

UNIT_ALIAS *water_unit_alias_new( void )
{
	UNIT_ALIAS *w;

	if ( ! ( w = calloc( 1, sizeof( UNIT_ALIAS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return w;

} /* water_unit_alias_new() */

PARAMETER_UNIT_ALIAS *water_parameter_unit_alias_new(
				char *parameter_unit_alias_string,
				char *parameter_name,
				char *units )
{
	PARAMETER_UNIT_ALIAS *w;

	if ( ! ( w = calloc( 1, sizeof( PARAMETER_UNIT_ALIAS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	w->parameter_unit_alias = parameter_unit_alias_string;

	w->parameter_unit =
		water_parameter_unit_new(
			parameter_name,
			units );

	return w;

} /* water_parameter_unit_alias_new() */

PARAMETER_ALIAS *water_new_parameter_alias( void )
{
	PARAMETER_ALIAS *w;

	if ( ! ( w = calloc( 1, sizeof( PARAMETER_ALIAS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return w;
} /* water_new_parameter_alias() */

STATION *water_new_station( void )
{
	STATION *w;

	if ( ! ( w = calloc( 1, sizeof( STATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return w;
} /* water_new_station() */

COLLECTION *water_collection_new( void )
{
	COLLECTION *w;

	if ( ! ( w = calloc( 1, sizeof( COLLECTION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return w;
} /* water_collection_new() */

RESULTS *water_results_new( void )
{
	RESULTS *w;

	if ( ! ( w = calloc( 1, sizeof( RESULTS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return w;
} /* water_results_new() */

PARAMETER_UNIT *water_parameter_unit_new(	char *parameter_name,
						char *units )
{
	return water_new_parameter_unit( parameter_name, units );
}

PARAMETER_UNIT *water_new_parameter_unit(	char *parameter_name,
						char *units )
{
	PARAMETER_UNIT *w;

	if ( ! ( w = calloc( 1, sizeof( PARAMETER_UNIT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	w->parameter_name = parameter_name;
	w->units = units;
	return w;

} /* water_new_parameter_unit() */

LIST *water_fetch_station_list(		char *application_name,
					char *project_name )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char piece_buffer[ 128 ];
	FILE *input_pipe;
	char *select;
	char *folder;
	char where[ 512 ];
	char *join_where;
	STATION *station;
	LIST *station_list;

	select = "station.station,longitude,latitude";
	folder = "water_project_station,station";
	join_where = "water_project_station.station = station.station";

	sprintf(where,
		"%s and project_name = '%s'",
		join_where,
		project_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	station_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		station = water_new_station();

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );
		station->station_name = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );
		station->longitude = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );
		station->latitude = strdup( piece_buffer );

		station->station_parameter_list =
			water_fetch_station_parameter_list(
				application_name,
				station->station_name );

		list_append_pointer( station_list, station );
	}

	pclose( input_pipe );
	return station_list;

} /* water_fetch_station_list() */

LIST *water_fetch_station_parameter_list(
				char *application_name,
				char *station_name )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char parameter_name[ 128 ];
	char units[ 128 ];
	FILE *input_pipe;
	char *select;
	char *folder;
	char where[ 128 ];
	STATION_PARAMETER *station_parameter;
	LIST *station_parameter_list;

	select = "parameter,units";
	folder = "station_parameter";

	sprintf( where, "station = '%s'", station_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	station_parameter_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		station_parameter = water_new_station_parameter();

		piece(	parameter_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	units,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		station_parameter->parameter_unit =
			water_new_parameter_unit(
				strdup( parameter_name ),
				strdup( units ) );

		list_append_pointer(
			station_parameter_list,
			station_parameter );
	}

	pclose( input_pipe );
	return station_parameter_list;

} /* water_fetch_station_parameter_list() */

LIST *water_fetch_parameter_name_list(	char *application_name )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;

	select = "parameter";
	folder = "parameter";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	",
		 application_name,
		 select,
		 folder );

	return pipe2list( sys_string );

} /* water_fetch_parameter_name_list() */

LIST *water_fetch_unit_name_list(	char *application_name )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;

	select = "units";
	folder = "unit";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	",
		 application_name,
		 select,
		 folder );

	return pipe2list( sys_string );

} /* water_fetch_unit_name_list() */

LIST *water_fetch_parameter_unit_list(	char *application_name,
					char *project_name )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char parameter_name[ 128 ];
	char units[ 128 ];
	FILE *input_pipe;
	char *select;
	char *folder;
	char where[ 128 ];
	PARAMETER_UNIT *parameter_unit;
	LIST *parameter_unit_list;

	select = "parameter,units";
	folder = "water_project_unit";

	sprintf( where, "project_name = '%s'", project_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );
	parameter_unit_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	parameter_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	units,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		parameter_unit =
			water_new_parameter_unit(
				strdup( parameter_name ),
				strdup( units ) );

		list_append_pointer( parameter_unit_list, parameter_unit );
	}

	pclose( input_pipe );
	return parameter_unit_list;

} /* water_fetch_parameter_unit_list() */

LIST *water_fetch_unit_alias_list(	char *application_name )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char piece_buffer[ 128 ];
	FILE *input_pipe;
	char *select;
	char *folder;
	UNIT_ALIAS *unit_alias;
	LIST *unit_alias_list;

	select = "unit_alias,units";
	folder = "unit_alias";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	",
		 application_name,
		 select,
		 folder );

	input_pipe = popen( sys_string, "r" );
	unit_alias_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		unit_alias = water_unit_alias_new();

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );
		unit_alias->unit_alias = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );
		unit_alias->units = strdup( piece_buffer );

		list_append_pointer( unit_alias_list, unit_alias );
	}

	pclose( input_pipe );
	return unit_alias_list;

} /* water_fetch_unit_alias_list() */

LIST *water_fetch_parameter_alias_list(	char *application_name )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char piece_buffer[ 128 ];
	FILE *input_pipe;
	char *select;
	char *folder;
	PARAMETER_ALIAS *parameter_alias;
	LIST *parameter_alias_list;

	select = "parameter_alias,parameter";
	folder = "parameter_alias";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	",
		 application_name,
		 select,
		 folder );

	input_pipe = popen( sys_string, "r" );
	parameter_alias_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		parameter_alias = water_new_parameter_alias();

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );
		parameter_alias->parameter_alias = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );
		parameter_alias->parameter_name = strdup( piece_buffer );

		list_append_pointer( parameter_alias_list, parameter_alias );
	}

	pclose( input_pipe );
	return parameter_alias_list;

} /* water_fetch_parameter_alias_list() */

LOAD_COLUMN *water_new_load_column( void )
{
	LOAD_COLUMN *w;

	if ( ! ( w = calloc( 1, sizeof( LOAD_COLUMN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return w;
} /* water_new_load_column() */

LIST *water_fetch_load_column_list(
				char *error_message,
				char *load_table_filename,
				LIST *parameter_unit_list,
				LIST *parameter_alias_list,
				DICTIONARY *application_constants_dictionary )
{
	LIST *load_column_list;
	FILE *input_file;
	char heading_string[ 65536 ];
	char piece_buffer[ 128 ];
	int column_piece;
	PARAMETER_UNIT *parameter_unit;
	LOAD_COLUMN *load_column;
	char *message_pointer;
	LIST *collection_date_heading_list = {0};
	LIST *collection_time_heading_list = {0};
	LIST *collection_depth_meters_heading_list = {0};
	LIST *station_heading_list = {0};
	LIST *station_latitude_heading_list = {0};
	LIST *station_longitude_heading_list = {0};

	water_seek_application_constants_dictionary(
		&collection_date_heading_list,
		&collection_time_heading_list,
		&collection_depth_meters_heading_list,
		&station_heading_list,
		&station_latitude_heading_list,
		&station_longitude_heading_list,
		application_constants_dictionary );

	if ( ! ( input_file = fopen( load_table_filename, "r" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot open for read: %s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			load_table_filename );
		exit( 1 );
	}

	message_pointer = error_message;

	if ( !get_line( heading_string, input_file ) )
	{
		strcpy( message_pointer, "empty input file." );
		fclose( input_file );
		return (LIST *)0;
	}

	fclose( input_file );
	load_column_list = list_new();

	for(	column_piece = 0;
		piece_quote_comma(
			piece_buffer,
			heading_string,
			column_piece );
		column_piece++ )
	{
		if ( list_exists_string( station_heading_list, piece_buffer ) )
		{
			load_column = water_new_load_column();
			load_column->station_collection_attribute =
				"station";
			load_column->column_piece = column_piece;
			list_append_pointer( load_column_list, load_column );
			continue;
		}

		if ( list_exists_string(collection_date_heading_list,
					piece_buffer ) )
		{
			load_column = water_new_load_column();
			load_column->station_collection_attribute =
				"collection_date";
			load_column->column_piece = column_piece;
			list_append_pointer( load_column_list, load_column );
			continue;
		}

		if ( list_exists_string(collection_time_heading_list,
					piece_buffer ) )
		{
			load_column = water_new_load_column();
			load_column->station_collection_attribute =
				"collection_time";
			load_column->column_piece = column_piece;
			list_append_pointer( load_column_list, load_column );
			continue;
		}

		if ( list_exists_string(collection_depth_meters_heading_list,
					piece_buffer ) )
		{
			load_column = water_new_load_column();
			load_column->station_collection_attribute =
				"collection_depth_meters";
			load_column->column_piece = column_piece;
			list_append_pointer( load_column_list, load_column );
			continue;
		}

		if ( list_exists_string(station_latitude_heading_list,
					piece_buffer ) )
		{
			load_column = water_new_load_column();
			load_column->station_collection_attribute = "latitude";
			load_column->column_piece = column_piece;
			list_append_pointer( load_column_list, load_column );
			continue;
		}

		if ( list_exists_string(station_longitude_heading_list,
					piece_buffer ) )
		{
			load_column = water_new_load_column();
			load_column->station_collection_attribute = "longitude";
			load_column->column_piece = column_piece;
			list_append_pointer( load_column_list, load_column );
			continue;
		}

		if ( ( parameter_unit =
			water_seek_filename_parameter_unit(
				piece_buffer
					/* parameter_alias_string */,
				parameter_unit_list,
				parameter_alias_list ) ) )
		{
			load_column = water_new_load_column();
			load_column->parameter_unit = parameter_unit;
			load_column->column_piece = column_piece;
			list_append_pointer(
				load_column_list,
				load_column );
		}
		else
		{
			/* Unrecognized column. */
			/* -------------------- */
			if ( message_pointer != error_message )
			{
				message_pointer +=
					sprintf( message_pointer,
						 "; " );
			}

			if ( column_piece >= 26 )
			{
				message_pointer +=
					sprintf(
					 message_pointer,
					 "column %d, heading %s",
					 column_piece + 1,
					 piece_buffer );
			}
			else
			{
				message_pointer +=
					sprintf(
					 message_pointer,
					 "column %c, heading %s",
					 (column_piece + 1) + 64,
					 piece_buffer );
			}
		} /* if unrecognized item */

	} /* for each piece() */

	return load_column_list;

} /* water_fetch_load_column_list() */

PARAMETER_UNIT *water_seek_parameter_unit(
				char *parameter_name,
				LIST *parameter_unit_list )
{
	PARAMETER_UNIT *parameter_unit;

	if ( !list_rewind( parameter_unit_list ) )
		return (PARAMETER_UNIT *)0;

	do {
		parameter_unit = list_get_pointer( parameter_unit_list );

		if ( timlib_strcmp(
			parameter_unit->parameter_name,
			parameter_name ) == 0 )
		{
			return parameter_unit;
		}

	} while( list_next( parameter_unit_list ) );

	return (PARAMETER_UNIT *)0;
} /* water_seek_parameter_unit() */

PARAMETER_ALIAS *water_seek_parameter_alias(
			char *parameter_alias_string,
			LIST *parameter_alias_list )
{
	PARAMETER_ALIAS *parameter_alias;

	if ( !list_rewind( parameter_alias_list ) )
		return (PARAMETER_ALIAS *)0;

	do {
		parameter_alias = list_get_pointer( parameter_alias_list );

		if ( timlib_strcmp(
			parameter_alias->parameter_alias,
			parameter_alias_string ) == 0 )
		{
			return parameter_alias;
		}

	} while( list_next( parameter_alias_list ) );

	return (PARAMETER_ALIAS *)0;
} /* water_seek_parameter_alias() */

PARAMETER_UNIT *water_seek_filename_parameter_unit(
					char *parameter_alias_string,
					LIST *parameter_unit_list,
					LIST *parameter_alias_list )
{
	PARAMETER_ALIAS *parameter_alias;
	PARAMETER_UNIT *parameter_unit;

	if ( ! ( parameter_alias =
			water_seek_parameter_alias(
				parameter_alias_string,
				parameter_alias_list ) ) )
	{
		return (PARAMETER_UNIT *)0;
	}

	if ( ! ( parameter_unit =
			water_seek_parameter_unit(
				parameter_alias->parameter_name,
				parameter_unit_list ) ) )
	{
		return (PARAMETER_UNIT *)0;
	}

	return parameter_unit;

} /* water_seek_filename_parameter_unit() */

int water_get_station_collection_attribute_piece(
				char *station_collection_attribute,
				LIST *load_column_list )
{
	LOAD_COLUMN *load_column;

	if ( !list_rewind( load_column_list ) ) return -1;

	do {
		load_column = list_get( load_column_list );

		if ( timlib_strcmp(	load_column->
						station_collection_attribute,
					station_collection_attribute ) == 0 )
		{
			return load_column->column_piece;
		}

	} while( list_next( load_column_list ) );

	return -1;
} /* water_get_station_collection_attribute_piece() */

LOAD_COLUMN *water_get_next_load_column_parameter_unit(
				LIST *load_column_list )
{
	LOAD_COLUMN *load_column;

	if ( list_past_end( load_column_list ) ) return (LOAD_COLUMN *)0;

	do {
		load_column = list_get( load_column_list );

		if ( load_column->parameter_unit ) return load_column;

	} while( list_next( load_column_list ) );

	return (LOAD_COLUMN *)0;
} /* water_get_next_load_column_parameter_unit() */

void water_seek_application_constants_dictionary(
		LIST **collection_date_heading_list,
		LIST **collection_time_heading_list,
		LIST **collection_depth_meters_heading_list,
		LIST **station_heading_list,
		LIST **station_latitude_heading_list,
		LIST **station_longitude_heading_list,
		DICTIONARY *application_constants_dictionary )
{
	if ( ! ( *collection_date_heading_list =
			dictionary_seek_delimited_list(
				application_constants_dictionary,
				COLLECTION_DATE_HEADING_KEY ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: not in APPLICATION_CONSTANTS: %s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			COLLECTION_DATE_HEADING_KEY );
		exit( 1 );
	}

	if ( ! ( *collection_time_heading_list =
			dictionary_seek_delimited_list(
				application_constants_dictionary,
				COLLECTION_TIME_HEADING_KEY ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: not in APPLICATION_CONSTANTS: %s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			COLLECTION_TIME_HEADING_KEY );
		exit( 1 );
	}

/*
	if ( ! ( *collection_depth_meters_heading_list =
			dictionary_seek_delimited_list(
				application_constants_dictionary,
				COLLECTION_DEPTH_METERS_HEADING_KEY ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: not in APPLICATION_CONSTANTS: %s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			COLLECTION_DEPTH_METERS_HEADING_KEY );
		exit( 1 );
	}
*/

	*collection_depth_meters_heading_list =
			dictionary_seek_delimited_list(
				application_constants_dictionary,
				COLLECTION_DEPTH_METERS_HEADING_KEY );

/*
	if ( ! ( *station_heading_list =
			dictionary_seek_delimited_list(
				application_constants_dictionary,
				STATION_HEADING_KEY ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: not in APPLICATION_CONSTANTS: %s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			STATION_HEADING_KEY );
		exit( 1 );
	}
*/

	*station_heading_list =
			dictionary_seek_delimited_list(
				application_constants_dictionary,
				STATION_HEADING_KEY );

/*
	if ( ! ( *station_latitude_heading_list =
			dictionary_seek_delimited_list(
				application_constants_dictionary,
				STATION_LATITUDE_HEADING_KEY ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: not in APPLICATION_CONSTANTS: %s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			STATION_LATITUDE_HEADING_KEY );
		exit( 1 );
	}
*/
	*station_latitude_heading_list =
			dictionary_seek_delimited_list(
				application_constants_dictionary,
				STATION_LATITUDE_HEADING_KEY );

/*
	if ( ! ( *station_longitude_heading_list =
			dictionary_seek_delimited_list(
				application_constants_dictionary,
				STATION_LONGITUDE_HEADING_KEY ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: not in APPLICATION_CONSTANTS: %s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			STATION_LONGITUDE_HEADING_KEY );
		exit( 1 );
	}
*/

	*station_longitude_heading_list =
			dictionary_seek_delimited_list(
				application_constants_dictionary,
				STATION_LONGITUDE_HEADING_KEY );

} /* water_seek_application_constants_dictionary() */

LIST *water_get_parameter_unit_alias_list(
			LIST *parameter_name_list,
			LIST *unit_name_list,
			LIST *parameter_alias_list,
			LIST *unit_alias_list )
{
	PARAMETER_ALIAS *parameter_alias;
	UNIT_ALIAS *unit_alias;
	LIST *parameter_unit_alias_list;
	PARAMETER_UNIT_ALIAS *parameter_unit_alias;
	char *parameter_unit_alias_string;
	char *parameter_name;
	char *unit_name;

	if ( !list_length( parameter_alias_list )
	||   !list_length( unit_alias_list )
	||   !list_length( parameter_name_list )
	||   !list_length( unit_name_list ) )
	{
		return (LIST *)0;
	}

	parameter_unit_alias_list = list_new();

	/* PARAMETER and UNIT */
	/* ------------------ */
	list_rewind( parameter_name_list );
	list_rewind( unit_name_list );

	do {
		parameter_name = list_get_pointer( parameter_name_list );

		list_rewind( unit_name_list );

		unit_name = list_get_pointer( unit_name_list );

		parameter_unit_alias_string =
			water_get_parameter_alias_string(
				parameter_name,
				unit_name );

		parameter_unit_alias =
			water_parameter_unit_alias_new(
				parameter_unit_alias_string,
				parameter_name,
				unit_name );

		list_append_pointer(
			parameter_unit_alias_list,
			parameter_unit_alias );

	} while( list_next( parameter_name_list ) );

	/* PARAMETER_ALIAS and UNIT */
	/* ------------------------ */
	list_rewind( parameter_alias_list );
	list_rewind( unit_name_list );

	do {
		parameter_alias = list_get_pointer( parameter_alias_list );

		list_rewind( unit_name_list );

		unit_name = list_get_pointer( unit_name_list );

		parameter_unit_alias_string =
			water_get_parameter_alias_string(
				parameter_alias->parameter_alias,
				unit_name );

		parameter_unit_alias =
			water_parameter_unit_alias_new(
				parameter_unit_alias_string,
				parameter_alias->parameter_alias,
				unit_name );

		list_append_pointer(
			parameter_unit_alias_list,
			parameter_unit_alias );

	} while( list_next( parameter_alias_list ) );

	/* PARAMETER and UNIT_ALIAS */
	/* ------------------------ */
	list_rewind( parameter_name_list );
	list_rewind( unit_alias_list );

	do {
		parameter_name = list_get_pointer( parameter_name_list );

		list_rewind( unit_alias_list );

		unit_alias = list_get_pointer( unit_alias_list );

		parameter_unit_alias_string =
			water_get_parameter_alias_string(
				parameter_name,
				unit_alias->unit_alias );

		parameter_unit_alias =
			water_parameter_unit_alias_new(
				parameter_unit_alias_string,
				parameter_name,
				unit_alias->unit_alias );

		list_append_pointer(
			parameter_unit_alias_list,
			parameter_unit_alias );

	} while( list_next( parameter_alias_list ) );

	/* PARAMETER_ALIAS and UNIT_ALIAS */
	/* ------------------------------ */
	list_rewind( parameter_alias_list );
	list_rewind( unit_alias_list );

	do {
		parameter_alias = list_get_pointer( parameter_alias_list );

		list_rewind( unit_alias_list );

		unit_alias = list_get_pointer( unit_alias_list );

		parameter_unit_alias_string =
			water_get_parameter_alias_string(
				parameter_alias->parameter_alias,
				unit_alias->unit_alias );

		parameter_unit_alias =
			water_parameter_unit_alias_new(
				parameter_unit_alias_string,
				parameter_name,
				unit_alias->unit_alias );

		list_append_pointer(
			parameter_unit_alias_list,
			parameter_unit_alias );

	} while( list_next( parameter_alias_list ) );


	return parameter_unit_alias_list;

} /* water_get_parameter_unit_alias_list() */

char *water_get_parameter_alias_string(
				char *parameter_name,
				char *unit_name )
{
	char parameter_unit_alias_string[ 256 ];

	sprintf( parameter_unit_alias_string,
		 "%s (%s)",
		 parameter_name,
		 unit_name );

	return strdup( parameter_unit_alias_string );

} /* water_get_parameter_alias_string() */

