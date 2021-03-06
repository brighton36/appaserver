/* google_map_dvr.c					*/
/* ---------------------------------------------------	*/
/* This is the test driver program for the google map   */
/* ADT.							*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "google_map.h"
#include "piece.h"
#include "appaserver_library.h"
#include "appaserver_error.h"

void test1(		void );

void populate_point_list(
			LIST *point_list,
			char *application_name,
			char *discovery_date );

int main( void )
{
	test1();
	return 0;
}

void test1( void )
{
	GOOGLE_MAP *google_map;
	char *google_map_key_data;

	google_map_key_data =
		google_map_get_map_key(
			"hydrology" );

	google_map_key_data = "AIzaSyClAql1VupuPCHmrvC1k_jc2zFwx0a0d10";
	google_map_output_heading(
				stdout,
				(char *)0 /* title */,
				google_map_key_data,
				(char *)0 /* balloon_click_parameter */,
				(char *)0 /* balloon_click_function */,
				"hydrology" /* application_name */,
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
			"alligator",
			"2006-07-13" );

	google_map_output_point_list( stdout, google_map->point_list );

	google_map_output_heading_close( stdout );

	google_map_output_body(
			stdout,
			0 /* not with_table */,
			(char *)0 /* additional_javascript */ );

	printf( "</body>\n" );
	printf( "</html>\n" );
} /* test1() */

void populate_point_list(
			LIST *point_list,
			char *application_name,
			char *discovery_date )
{
	char sys_string[ 1024 ];
	char input_buffer[ 128 ];
	char click_message[ 128 ];
	char where[ 256 ];
	char utm_easting[ 128 ];
	char utm_northing[ 128 ];
	FILE *input_pipe;
	char *select = "utm_easting,utm_northing";

	sprintf(where,
		"discovery_date = '%s' and utm_easting is not null",
		discovery_date );

	sprintf(sys_string,
		"get_folder_data	application=%s		"
		"			select=%s		"
		"			folder=nest		"
		"			where=\"%s\"		",
		application_name,
		select,
		where );

	input_pipe = popen( sys_string, "r" );


	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	utm_easting,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	utm_northing,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		sprintf( click_message, "%s/%s", utm_easting, utm_northing );

		google_map_set_point(
				point_list,
				strdup( click_message ),
				0.0 /* latitude */,
				0.0 /* longitude */,
				atoi( utm_easting ),
				atoi( utm_northing ) );
	}
	pclose( input_pipe );

	google_map_convert_to_latitude_longitude( point_list );

} /* populate_point_list() */

