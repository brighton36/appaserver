/* ------------------------------------------------------*/
/* $APPASERVER_HOME/src_vegetation/update_cell_stage.c	 */
/* ------------------------------------------------------*/
/* Freely available software: see Appaserver.org	 */
/* ------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "list.h"
#include "boolean.h"
#include "distance.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define HYDROLOGY_DATABASE "hydrology"

/* Structures */
/* ---------- */
typedef struct {
	char *station;
	unsigned int utm_x;
	unsigned int utm_y;
	double measurement_value;
	unsigned int distance;
} STATION;

/* Prototypes */
/* ---------- */
void update_statement_cell_stage(
			char *application_name,
			LIST *station_list,
			boolean execute );

STATION *update_cell_stage_calculate_nearest_station(
			unsigned int utm_x,
			unsigned int utm_y,
			LIST *station_list );

LIST *update_cell_stage_get_station_list(
			char *measurement_date,
			char *appaserver_mount_point );

STATION *update_cell_stage_station_calloc(
			void );

STATION *update_cell_stage_station_parse(
			char *input_buffer );

void update_cell_stage(	char *application_name,
			int days_ago,
			boolean execute );

int main( int argc, char **argv )
{
	char *application_name;
	int days_ago;
	boolean execute;
	char *database_string = {0};

	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s application days_ago execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	days_ago = atoi( argv[ 2 ] );
	execute = (*argv[ 3 ] == 'y');

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	update_cell_stage( application_name, days_ago, execute );

	return 0;

} /* main() */

STATION *update_cell_stage_station_calloc( void )
{
	STATION *s;

	if ( ! ( s = (STATION *)calloc( 1, sizeof( STATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return s;
}

STATION *update_cell_stage_station_parse( char *input_buffer )
{
	char buffer[ 128 ];
	STATION *station;

	/* WW^495617.47038^2830002.98326^1.1400 */
	/* ------------------------------------ */

	station = update_cell_stage_station_calloc();

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	station->station = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	station->utm_x = atoi( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	station->utm_y = atoi( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	station->measurement_value = atof( buffer );

	return station;

} /* update_cell_stage_station_parse() */

LIST *update_cell_stage_get_station_list(
			char *measurement_date,
			char *appaserver_mount_point )
{
	LIST *station_list;
	char sys_string[ 1024 ];
	STATION *station;
	FILE *input_pipe;
	char input_buffer[ 128 ];

	sprintf( sys_string,
		 "%s/src_%s/station_utm_stage_list.sh %s %s	|"
		 "cat						 ",
		 appaserver_mount_point,
		 HYDROLOGY_DATABASE,
		 HYDROLOGY_DATABASE,
		 measurement_date );

	input_pipe = popen( sys_string, "r" );

	station_list = list_new();

	while( timlib_get_line( input_buffer, input_pipe, 128 ) )
	{
		station = update_cell_stage_station_parse( input_buffer );
		list_append_pointer( station_list, station );
	}

	pclose( input_pipe );
	return station_list;

} /* update_cell_stage_get_station_list() */

STATION *update_cell_stage_calculate_nearest_station(
					unsigned int utm_x,
					unsigned int utm_y,
					LIST *station_list )
{
	unsigned int nearest_distance = INT_MAX;
	unsigned int distance;
	STATION *nearest_station = {0};
	STATION *station;

	if ( !list_rewind( station_list ) ) return (STATION *)0;

	do {
		station = list_get( station_list );

		distance =
			distance_get_pythagorean_distance_integer(
				station->utm_x  /* point_a_x */,
				station->utm_y /* point_a_y */,
				utm_x /* point_b_x */,
				utm_y /* point_b_y */ );

		if ( distance < nearest_distance )
		{
			nearest_distance = distance;
			nearest_station = station;
			nearest_station->distance = distance;
		}
	} while( list_next( station_list ) );

	return nearest_station;

} /* update_cell_stage_calculate_nearest_station() */

void update_statement_cell_stage(
			char *application_name,
			LIST *station_list,
			boolean execute )
{
	char sys_string[ 1024 ];
	STATION *station;
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 128 ];
	char utm_x[ 64 ];
	char utm_y[ 64 ];
	char *key;
	char *output_string;

	key = "UTMX,UTMY";

	if ( execute )
		output_string = "sql.e";
	else
		output_string = "cat";

	sprintf( sys_string,
		 "update_statement.e table=cell key=%s carrot=y |"
		 "%s						 ",
		 key,
		 output_string );

	output_pipe = popen( sys_string, "w" );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=UTMX,UTMY	"
		 "			folder=cell		",
		 application_name );

	input_pipe = popen( sys_string, "r" );

	while( timlib_get_line( input_buffer, input_pipe, 128 ) )
	{
		piece( utm_x, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( utm_y, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		if ( ! ( station =
				update_cell_stage_calculate_nearest_station(
					atoi( utm_x ),
					atoi( utm_y ),
					station_list ) ) )
		{
			fprintf( stderr,
	"Warning in %s/%s()/%d: cannot calculate nearest station for (%s/%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 utm_x,
				 utm_y );
			continue;
		}

/*
printf( "cell = %s/%s, station = %s, utm_x = %d, utm_y = %d, distance = %d\n",
utm_x,
utm_y,
station->station,
station->utm_x,
station->utm_y,
station->distance );
*/

		fprintf( output_pipe,
		"%s^%s^stage_NAVD88_ft^%.2lf\n",
			 utm_x,
			 utm_y,
			 station->measurement_value );

		fprintf( output_pipe,
		"%s^%s^nearest_hydrology_station^%s\n",
			 utm_x,
			 utm_y,
			 station->station );
	}

	pclose( input_pipe );
	pclose( output_pipe );

} /* update_statement_cell_stage() */

void update_cell_stage(	char *application_name,
			int days_ago,
			boolean execute )
{
	LIST *station_list;
	char *measurement_date;
	char *appaserver_mount_point;
	char sys_string[ 128 ];

	sprintf( sys_string,
		 "now.sh ymd -%d",
		 days_ago );

	measurement_date = pipe2string( sys_string );

	appaserver_mount_point =
		appaserver_parameter_file_get_appaserver_mount_point();

	station_list =
		update_cell_stage_get_station_list(
			measurement_date,
			appaserver_mount_point );

	if ( !list_length( station_list ) ) return;

	update_statement_cell_stage(
		application_name,
		station_list,
		execute );

} /* update_cell_stage() */

