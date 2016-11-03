/* library/waypoint.c					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "waypoint.h"
#include "timlib.h"
#include "column.h"
#include "distance.h"
#include "list.h"

/* Functions */
/* --------- */
WAYPOINT *waypoint_new(	void )
{
	WAYPOINT *waypoint;

	if ( ! ( waypoint = calloc( 1, sizeof( WAYPOINT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return waypoint;

} /* waypoint_new() */

WAYPOINT_TRIP *waypoint_trip_new( WAYPOINT *starting_waypoint )
{
	WAYPOINT_TRIP *waypoint_trip;

	if ( ! ( waypoint_trip = calloc( 1, sizeof( WAYPOINT_TRIP ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	waypoint_trip->starting_waypoint = starting_waypoint;
	waypoint_trip->visit_waypoint_list = list_new();
	waypoint_trip->optimal_distance = INT_MAX;
	return waypoint_trip;

} /* waypoint_trip_new() */

boolean waypoint_convert_degrees_float(
			double *latitude_longitude,
			char *latitude_longitude_degrees_float )
{
	char degrees[ 16 ];
	char minutes_float[ 16 ];

	if ( !latitude_longitude_degrees_float
	||   !*latitude_longitude_degrees_float )
	{
		return 0;
	}

	if ( character_count( ' ', latitude_longitude_degrees_float ) == 0 )
	{
		if ( isdigit( *latitude_longitude_degrees_float )
		||   *latitude_longitude_degrees_float == '-' )
		{
			*latitude_longitude = atof( 
				latitude_longitude_degrees_float );
			return 1;
		}
	}

	if ( character_count( ' ', latitude_longitude_degrees_float ) != 1 )
	{
		return 0;
	}

	column( degrees, 0, latitude_longitude_degrees_float );
	column( minutes_float, 1, latitude_longitude_degrees_float );

	if ( !*degrees ) return 0;

	if ( *degrees == '-' )
	{
		*latitude_longitude = atof( degrees ) -
		          (atof( minutes_float ) / 60.0 );
	}
	else
	{
		*latitude_longitude = atof( degrees ) +
		          (atof( minutes_float ) / 60.0 );
	}

	return 1;

} /* waypoint_convert_degrees_float() */

WAYPOINT *waypoint_new_degrees_float(
			char *latitude_degrees_float,
			char *longitude_degrees_float )
{
	WAYPOINT *waypoint = waypoint_new();
	char buffer[ 128 ];

	waypoint->latitude_degrees_float = latitude_degrees_float;
	waypoint->longitude_degrees_float = longitude_degrees_float;

	if ( !waypoint_convert_degrees_float_latitude_longitude(
			&waypoint->latitude,
			&waypoint->longitude,
			waypoint->latitude_degrees_float,
			waypoint->longitude_degrees_float ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot convert degrees float: (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 waypoint->latitude_degrees_float,
			 waypoint->longitude_degrees_float );
		exit( 1 );
	}

	sprintf( buffer, "%.5lf", waypoint->latitude );
	waypoint->latitude_string = strdup( buffer );
	sprintf( buffer, "%.5lf", waypoint->longitude );
	waypoint->longitude_string = strdup( buffer );

	return waypoint;

} /* waypoint_new_degrees_float() */

boolean waypoint_convert_degrees_float_latitude_longitude(
			double *latitude,
			double *longitude,
			char *latitude_degrees_float,
			char *longitude_degrees_float )
{
	if ( !waypoint_convert_degrees_float(
			latitude,
			latitude_degrees_float ) )
	{
		return 0;
	}

	return waypoint_convert_degrees_float(
			longitude,
			longitude_degrees_float );
} /* waypoint_convert_degrees_float_latitude_longitude() */

boolean waypoint_set_visit(
			LIST *visit_waypoint_list,
			WAYPOINT *waypoint )
{
	return list_append_pointer(
			visit_waypoint_list,
			waypoint );

} /* waypoint_set_visit() */

boolean waypoint_convert_to_utm_write_temp_file(
			char *temp_filename,
			WAYPOINT *starting_waypoint,
			LIST *waypoint_list )
{
	char sys_string[ 1024 ];
	WAYPOINT *waypoint;
	FILE *output_pipe;

	sprintf(	sys_string,
			"cs2cs_lonlat.sh > %s",
			temp_filename );
	output_pipe = popen( sys_string, "w" );

	if ( starting_waypoint )
	{
		fprintf(	output_pipe,
				"%.5lf %.5lf\n",
				starting_waypoint->longitude,
				starting_waypoint->latitude );
	}

	if ( list_rewind( waypoint_list ) )
	{
		do {
			waypoint = list_get_pointer( waypoint_list );

			if ( waypoint && waypoint->longitude )
			{
				fprintf(	output_pipe,
						"%.5lf %.5lf\n",
						waypoint->longitude,
						waypoint->latitude );
			}
			else
			{
				fprintf( output_pipe, "\n" );
			}

		} while( list_next( waypoint_list ) );
	}

	pclose( output_pipe );

	return 1;

} /* waypoint_convert_to_utm_write_temp_file() */

boolean waypoint_convert_to_utm(
			WAYPOINT *starting_waypoint,
			LIST *visit_waypoint_list )
{
	char *temp_filename = waypoint_get_temp_filename();

	if ( ! waypoint_convert_to_utm_write_temp_file(
			temp_filename,
			starting_waypoint,
			visit_waypoint_list ) )
	{
		return 0;
	}

	return waypoint_convert_to_utm_read_temp_file(
			starting_waypoint,
			visit_waypoint_list,
			temp_filename );

} /* waypoint_convert_to_utm() */

char *waypoint_get_temp_filename( void )
{
	static char temp_filename[ 128 ];

	sprintf( temp_filename, "/tmp/waypoint_%d", getpid() );
	return temp_filename;

} /* waypoint_get_temp_filename() */

boolean waypoint_convert_to_utm_read_temp_file(
			WAYPOINT *starting_waypoint,
			LIST *waypoint_list,
			char *temp_filename )
{
	WAYPOINT *waypoint;
	FILE *input_file;
	char input_buffer[ 128 ];
	char utm_easting[ 64 ];
	char utm_northing[ 64 ];
	boolean first_time = 1;

	if ( ! ( input_file = fopen( temp_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 temp_filename );
		return 0;
	}

	list_rewind( waypoint_list );

	while( get_line( input_buffer, input_file ) )
	{
		if ( !*input_buffer )
		{
			list_next( waypoint_list );
			continue;
		}

		column( utm_easting, 0, input_buffer );
		column( utm_northing, 1, input_buffer );

		if ( first_time && starting_waypoint )
		{
			starting_waypoint->utm_easting = atoi( utm_easting );
			starting_waypoint->utm_northing = atoi( utm_northing );
			first_time = 0;
		}
		else
		{
			waypoint = list_get_pointer( waypoint_list );
			waypoint->utm_easting = atoi( utm_easting );
			waypoint->utm_northing = atoi( utm_northing );
			list_next( waypoint_list );
		}
	}
	fclose( input_file );
	return 1;

} /* waypoint_convert_to_utm_read_temp_file() */

void waypoint_free_distance_list(
			LIST *distance_list )
{
	list_free( distance_list );
}

int waypoint_get_total_distance(
			LIST *distance_list )
{
	int total_distance = 0;
	int *distance;

	if ( !list_rewind( distance_list ) ) return 0;

	do {
		distance = list_get_pointer( distance_list );

		total_distance += *distance;
	} while( list_next( distance_list ) );

	return total_distance;
} /* waypoint_get_total_distance() */

LIST *waypoint_get_utm_distance_list(
			WAYPOINT *starting_waypoint,
			WAYPOINT *visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
			int array_length )
{
	LIST *distance_list = list_new();
	int i;
	int *distance;
	WAYPOINT *prior_waypoint = {0};
	WAYPOINT *waypoint;

	if ( !array_length ) return distance_list;

	for( i = 0; i < array_length; i++ )
	{
		if ( !prior_waypoint )
			prior_waypoint = starting_waypoint;
		else
			prior_waypoint = waypoint;

		if ( ! ( distance = (int *)calloc( 1, sizeof( int ) ) ) )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		waypoint = visit_waypoint_array[ i ];

		*distance =
			distance_get_pythagorean_distance_integer(
				waypoint->utm_easting  /* point_a_x */,
				waypoint->utm_northing /* point_a_y */,
				prior_waypoint->utm_easting
						       /* point_b_x */,
				prior_waypoint->utm_northing
							/* point_b_y */ );

		list_append_pointer( distance_list, distance );
	}

	return distance_list;

} /* waypoint_get_utm_distance_list() */

int waypoint_set_visit_array(
		WAYPOINT *visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		LIST *visit_waypoint_list )
{
	int array_length = 0;
	WAYPOINT *waypoint;

	if (	list_length( visit_waypoint_list ) > 
		WAYPOINT_MAX_ARRAY )
	{
		return 0;
	}

	if ( !list_rewind( visit_waypoint_list ) ) return 0;

	do {
		waypoint = list_get_pointer( visit_waypoint_list );

		visit_waypoint_array[ array_length++ ] = waypoint;

	} while( list_next( visit_waypoint_list ) );

	return array_length;

} /* waypoint_set_visit_array() */

boolean waypoint_get_arrangement_array(
		FILE **arrangement_input_pipe,
		int arrangement_array[ WAYPOINT_MAX_ARRAY ],
		int array_length )
{
	char input_buffer[ 128 ];
	char number_string[ 16 ];
	int i;

	if ( !*arrangement_input_pipe )
	{
		char sys_string[ 1024 ];

		sprintf( sys_string,
			 "complete_arrangement.e %d",
			 array_length );
		*arrangement_input_pipe = popen( sys_string, "r" );
	}

	if ( !get_line( input_buffer, *arrangement_input_pipe ) )
	{
		pclose( *arrangement_input_pipe );
		return 0;
	}

	for( i = 0; i < array_length; i++ )
	{
		column( number_string, i, input_buffer );
		arrangement_array[ i ] = atoi( number_string );
	}

	return 1;
} /* waypoint_get_arrangement_array() */

/* Note: the contents of arrangement_array[] is one-based. */
/* ------------------------------------------------------- */
void waypoint_set_arranged_visit_waypoint_array(
		WAYPOINT *arranged_visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		WAYPOINT *visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		int arrangement_array[ WAYPOINT_MAX_ARRAY ],
		int array_length )
{
	int i;

	for( i = 0; i < array_length; i++ )
	{
		arranged_visit_waypoint_array[ i ] =
			visit_waypoint_array
				[ arrangement_array[ i ] - 1 ];
	}

} /* waypoint_set_arranged_visit_waypoint_array() */

boolean waypoint_set_optimal_visit_waypoint_array(
		int *optimal_distance,
		FILE **arrangement_input_pipe,
		WAYPOINT *optimal_visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		WAYPOINT *visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		WAYPOINT *starting_waypoint,
		int array_length )
{
	WAYPOINT *arranged_visit_waypoint_array[ WAYPOINT_MAX_ARRAY ];
	int arrangement_array[ WAYPOINT_MAX_ARRAY ];

	while( waypoint_get_arrangement_array(
			arrangement_input_pipe,
			arrangement_array,
			array_length ) )
	{
		waypoint_set_arranged_visit_waypoint_array(
			arranged_visit_waypoint_array,
			visit_waypoint_array,
			arrangement_array,
			array_length );

		waypoint_set_optimal_visit_waypoint_array_maybe(
			optimal_distance,
			optimal_visit_waypoint_array,
			arranged_visit_waypoint_array,
			starting_waypoint,
			array_length );
	}


	return 1;

} /* waypoint_set_optimal_visit_waypoint_array() */

void waypoint_set_optimal_visit_waypoint_array_maybe(
		int *optimal_distance,
		WAYPOINT *optimal_visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		WAYPOINT *arranged_visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		WAYPOINT *starting_waypoint,
		int array_length )
{
	LIST *distance_list;
	int total_distance;

	distance_list =
		waypoint_get_utm_distance_list(
			starting_waypoint,
			arranged_visit_waypoint_array,
			array_length );

	total_distance = waypoint_get_total_distance( distance_list );

	if ( total_distance < *optimal_distance )
	{
		*optimal_distance = total_distance;

		waypoint_array_copy(
			optimal_visit_waypoint_array,
			arranged_visit_waypoint_array,
			array_length );
	}

} /* waypoint_set_optimal_visit_waypoint_array_maybe() */

char *waypoint_array_display(
		WAYPOINT *waypoint_array[ WAYPOINT_MAX_ARRAY ],
		int array_length )
{
	char return_string[ 65536 ];
	char *ptr = return_string;
	WAYPOINT *waypoint;

	int i;

	*ptr = '\0';

	for( i = 0; i < array_length; i++ )
	{
		waypoint = waypoint_array[ i ];
		ptr += sprintf(	ptr, "i = %d: %s\n",
				i,
				waypoint_display( waypoint ) );
	}

	return strdup( return_string );

} /* waypoint_array_display() */

char *waypoint_display( WAYPOINT *waypoint )
{
	char return_string[ 1024 ];

	if ( !waypoint ) return "NULL";

	sprintf( return_string,
"latitude_degrees_float = %s\nlongitude_degrees_float = %s\nlatitude = %.5lf\nlongitude = %.5lf\nlatitude_string = %s\nlongitude_string = %s\nutm_easting = %d\nutm_northing = %d\n",
		 waypoint->latitude_degrees_float,
		 waypoint->longitude_degrees_float,
		 waypoint->latitude,
		 waypoint->longitude,
		 (waypoint->latitude_string) ? waypoint->latitude_string : "",
		 (waypoint->longitude_string) ? waypoint->longitude_string : "",
		 waypoint->utm_easting,
		 waypoint->utm_northing );

	return strdup( return_string );

} /* waypoint_display() */

void waypoint_array_copy(
		WAYPOINT **destination_waypoint_array,
		WAYPOINT **source_waypoint_array,
		int array_length )
{
	WAYPOINT *waypoint;
	int i;

	for( i = 0; i < array_length; i++ )
	{
		waypoint = source_waypoint_array[ i ];
		destination_waypoint_array[ i ] = waypoint;
	}

} /* waypoint_array_copy() */

boolean waypoint_list_convert_to_utm(
			LIST *waypoint_list )
{
	boolean return_value;
	char *temp_filename = waypoint_get_temp_filename();

	if ( ! waypoint_list_convert_to_utm_write_temp_file(
			temp_filename,
			waypoint_list ) )
	{
		return 0;
	}

	return_value = waypoint_list_convert_to_utm_read_temp_file(
				waypoint_list,
				temp_filename );

	waypoint_list_remove_temp_file( temp_filename );

	return return_value;

} /* waypoint_list_convert_to_utm() */

void waypoint_list_remove_temp_file( char *temp_filename )
{
	char sys_string[ 128 ];

	sprintf( sys_string, "rm -f %s", temp_filename );
	system( sys_string );

} /* waypoint_list_remove_temp_file() */


boolean waypoint_list_convert_to_utm_write_temp_file(
		char *temp_filename,
		LIST *waypoint_list )
{
	return waypoint_convert_to_utm_write_temp_file(
			temp_filename,
			(WAYPOINT *)0 /* starting_waypoint */,
			waypoint_list );

} /* waypoint_list_convert_to_utm_write_temp_file() */

boolean waypoint_list_convert_to_utm_read_temp_file(
		LIST *waypoint_list,
		char *temp_filename )
{
	return waypoint_convert_to_utm_read_temp_file(
			(WAYPOINT *)0 /* starting_waypoint */,
			waypoint_list,
			temp_filename );

} /* waypoint_list_convert_to_utm_read_temp_file() */

LIST *waypoint_list_get_utm_distance_list(
			LIST *waypoint1_list,
			LIST *waypoint2_list )
{
	LIST *distance_list = list_new();
	int *distance;
	WAYPOINT *waypoint1;
	WAYPOINT *waypoint2;

	if ( !list_length( waypoint1_list ) ) return distance_list;

	if ( list_length( waypoint1_list ) != list_length( waypoint2_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: list length of %d does not equal length of %d\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 list_length( waypoint1_list ),
			 list_length( waypoint2_list ) );
		exit( 1 );
	}

	list_rewind( waypoint1_list );
	list_rewind( waypoint2_list );

	do {
		waypoint1 = list_get_pointer( waypoint1_list );
		waypoint2 = list_get_pointer( waypoint2_list );

		if ( ! ( distance = (int *)calloc( 1, sizeof( int ) ) ) )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		if ( !waypoint1 || !waypoint2 )
		{
			*distance = 0;
		}
		else
		{
/*
fprintf( stderr, "got 1) utm_easting = %d\n", waypoint1->utm_easting );
fprintf( stderr, "got 1) utm_northing = %d\n", waypoint1->utm_northing );
fprintf( stderr, "got 2) utm_easting = %d\n", waypoint2->utm_easting );
fprintf( stderr, "got 2) utm_northing = %d\n", waypoint2->utm_northing );
*/

			*distance =
			distance_get_pythagorean_distance_integer(
				waypoint1->utm_easting  /* point_a_x */,
				waypoint1->utm_northing /* point_a_y */,
				waypoint2->utm_easting  /* point_b_x */,
				waypoint2->utm_northing /* point_b_y */ );

/*
fprintf( stderr, "%s(): got distance = %d\n",
__FUNCTION__, *distance );
*/

		}

		list_append_pointer( distance_list, distance );

		list_next( waypoint2_list );
	} while( list_next( waypoint1_list ) );

	return distance_list;

} /* waypoint_list_get_utm_distance_list() */

void waypoint_list_convert_degrees_float(
				LIST *waypoint_list )
{
	WAYPOINT *waypoint;

	if ( list_rewind( waypoint_list ) )
	{
		do {
			waypoint = list_get_pointer( waypoint_list );

			if ( waypoint
			&&   waypoint->latitude_degrees_float )
			{
				waypoint_convert_degrees_float(
					&waypoint->latitude,
					waypoint->latitude_degrees_float );

				waypoint_convert_degrees_float(
					&waypoint->longitude,
					waypoint->longitude_degrees_float );
			}

		} while( list_next( waypoint_list ) );
	}

} /* waypoint_list_convert_degrees_float() */

