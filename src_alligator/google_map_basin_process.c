/* google_map_basin_process.c				*/
/* ---------------------------------------------------	*/
/* This is the output process to google map a basin.    */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "environ.h"
#include "google_map.h"
#include "piece.h"
#include "document.h"
#include "appaserver_library.h"
#include "appaserver_error.h"

void populate_point_list(
			LIST *point_list,
			char *application_name,
			char *basin );

int main( int argc, char **argv )
{
	GOOGLE_MAP *google_map;
	char *google_map_key_data;
	char *application_name;
	char *basin;
	char *database_string = {0};

	if ( argc != 3 )
	{
		fprintf( stderr, "Usage: %s application basin\n", argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	basin = argv[ 2 ];

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
	add_local_bin_to_path();
	add_relative_source_directory_to_path( application_name );

	google_map_key_data =
		google_map_get_map_key(
			application_name );

	document_output_content_type();

	google_map_output_heading(
				stdout,
				(char *)0 /* title */,
				google_map_key_data,
				application_name,
				(char *)0 /* balloon_click_parameter */,
				(char *)0 /* balloon_click_function */,
				0 /* absolute_position_top */,
				0 /* absolute_position_left */,
				GOOGLE_MAP_CENTER_LATITUDE,
				GOOGLE_MAP_CENTER_LONGITUDE,
				GOOGLE_MAP_STARTING_ZOOM,
				GOOGLE_MAP_WIDTH,
				GOOGLE_MAP_HEIGHT );

	google_map = google_map_new();

	populate_point_list(
			google_map->point_list,
			application_name,
			basin );

	google_map_output_point_list( stdout, google_map->point_list );

	google_map_output_heading_close( stdout );

	google_map_output_body(
			stdout,
			0 /* not with_table */,
			(char *)0 /* additional_javascript */ );

	document_close();
	return 0;
} /* main() */

void populate_point_list(
			LIST *point_list,
			char *application_name,
			char *basin )
{
	char sys_string[ 1024 ];
	char input_buffer[ 128 ];
	char label[ 128 ];
	char where[ 256 ];
	char cell_number[ 128 ];
	char transect_number[ 128 ];
	char utm_easting[ 128 ];
	char utm_northing[ 128 ];
	FILE *input_pipe;
	char *cell_table_name;
	char *transect_table_name;
	char select[ 256 ];

	cell_table_name = get_table_name( application_name, "cell" );
	transect_table_name = get_table_name( application_name, "transect" );

	sprintf(select,
		"cell_number,%s.transect_number,utm_easting,utm_northing",
		transect_table_name );

	sprintf(where,
		"%s.transect_number = %s.transect_number and basin = '%s'",
		cell_table_name,
		transect_table_name,
		basin );

	sprintf(sys_string,
		"get_folder_data	application=%s		"
		"			select=%s		"
		"			folder=cell,transect	"
		"			where=\"%s\"		",
		application_name,
		select,
		where );

	input_pipe = popen( sys_string, "r" );


	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	cell_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	transect_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	utm_easting,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	utm_northing,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		sprintf(label,
			"Cell: %s<br>Transect: %s<br>UTM: %s / %s",
			cell_number,
			transect_number,
			utm_easting,
			utm_northing );

		google_map_set_point(
				point_list,
				strdup( label ),
				0.0 /* latitude */,
				0.0 /* longitude */,
				atoi( utm_easting ),
				atoi( utm_northing ) );
	}
	pclose( input_pipe );

	google_map_convert_to_latitude_longitude( point_list );

} /* populate_point_list() */

