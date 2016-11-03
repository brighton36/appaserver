/* google_map_project_process.c				*/
/* ---------------------------------------------------	*/
/* This is the output process to google map a		*/
/* project.						*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "environ.h"
#include "process.h"
#include "appaserver_parameter_file.h"
#include "google_map.h"
#include "piece.h"
#include "document.h"
#include "appaserver_library.h"
#include "appaserver_error.h"

void populate_point_list(
			LIST *point_list,
			char *application_name,
			char *project_name );

int main( int argc, char **argv )
{
	GOOGLE_MAP *google_map;
	char *google_map_key_data;
	char *application_name;
	char *process_name;
	char *project_name;
	char *database_string = {0};

	if ( argc != 4 )
	{
		fprintf( stderr,
		"Usage: %s application process project_name\n", argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	project_name = argv[ 3 ];

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

	if ( strcmp( project_name, "project_name" ) == 0 )
	{
		APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

		appaserver_parameter_file = new_appaserver_parameter_file();

		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<h3>ERROR: please select a water project.</h3>\n" );
		document_close();
		exit( 0 );
	}

	google_map_key_data =
		google_map_get_map_key(
			application_name );

	document_output_content_type();

	google_map_output_heading(
				stdout,
				(char *)0 /* title */,
				google_map_key_data,
				(char *)0 /* balloon_click_parameter */,
				(char *)0 /* balloon_click_function */,
				application_name,
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
			project_name );

	google_map_output_point_list( stdout, google_map->point_list );

	google_map_output_heading_close( stdout );

	google_map_output_body(
			stdout,
			0 /* not with_table */,
			(char *)0 /* additional_javascript */ );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void populate_point_list(
			LIST *point_list,
			char *application_name,
			char *project_name )
{
	char sys_string[ 1024 ];
	char input_buffer[ 128 ];
	char label[ 128 ];
	char where[ 256 ];
	char station[ 128 ];
	char latitude[ 128 ];
	char longitude[ 128 ];
	FILE *input_pipe;
	char *water_project_station_table_name;
	char *station_table_name;
	char select[ 256 ];

	water_project_station_table_name =
		get_table_name( application_name, "water_project_station" );
	station_table_name =
		get_table_name( application_name, "station" );

	sprintf(select,
		"%s.station,latitude,longitude",
		station_table_name );

	sprintf(where,
		"%s.station = %s.station and		"
		"project_name = '%s' and		"
		"latitude is not null and		"
		"longitude is not null			",
		station_table_name,
		water_project_station_table_name,
		project_name );

	sprintf(sys_string,
		"get_folder_data	application=%s			    "
		"			select=%s			    "
		"			folder=water_project_station,station"
		"			where=\"%s\"			    ",
		application_name,
		select,
		where );

	input_pipe = popen( sys_string, "r" );


	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	station,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	latitude,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	longitude,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		sprintf(label,
			"Station: %s<br>Latitude/Longitude: %s / %s",
			station,
			latitude,
			longitude );

		google_map_set_point(
				point_list,
				strdup( label ),
				atof( latitude ),
				atof( longitude ),
				0 /* utm_easting */,
				0 /* utm_northing */ );
	}
	pclose( input_pipe );

} /* populate_point_list() */

