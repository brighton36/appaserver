/* ---------------------------------------------------	*/
/* src_benthic/waypoint.c				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "waypoint.h"
#include "benthic_waypoint.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"

WAYPOINT_SITE *waypoint_site_new(
			char *application_name,
			char *location,
			char *project,
			int site_number )
{
	WAYPOINT_SITE *waypoint_site;

	if ( ! ( waypoint_site = calloc( 1, sizeof( WAYPOINT_SITE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	waypoint_site->location = location;
	waypoint_site->project = project;
	waypoint_site->site_number = site_number;

	if ( !waypoint_get_waypoint_array(
			waypoint_site->waypoint_array,
			application_name,
			waypoint_site->project,
			waypoint_site->location,
			waypoint_site->site_number ) )
	{
		return (WAYPOINT_SITE *)0;
	}

	return waypoint_site;
} /* waypoint_site_new() */

BENTHIC_WAYPOINT *benthic_waypoint_new(
			char *latitude,
			char *longitude,
			boolean inaccessible )
{
	BENTHIC_WAYPOINT *waypoint;

	if ( ! ( waypoint = calloc( 1, sizeof( BENTHIC_WAYPOINT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	waypoint->latitude = latitude;
	waypoint->longitude = longitude;
	waypoint->inaccessible = inaccessible;

	return waypoint;

} /* benthic_waypoint_new() */

boolean waypoint_get_waypoint_array(
			BENTHIC_WAYPOINT *waypoint_array[ CANDIDATES_PER_SITE ],
			char *application_name,
			char *project,
			char *location,
			int site_number )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char latitude[ 128 ];
	char longitude[ 128 ];
	char inaccessible_yn[ 128 ];
	char where[ 512 ];
	FILE *input_pipe;
	char *select;
	BENTHIC_WAYPOINT *waypoint;
	int offset = 0;
	register int return_value = 0;

	select = "latitude,longitude,inaccessible_yn";

	sprintf( where,
		 "location = '%s' and		"
		 "project = '%s' and		"
		 "site_number = %d		",
		 location,
		 project,
		 site_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=waypoint		"
		 "			where=\"%s\"		"
		 "			order=none		",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		return_value = 1;

		if ( offset == CANDIDATES_PER_SITE )
		{
			pclose( input_pipe );
			return 1;
		}

		piece(	latitude,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	longitude,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	inaccessible_yn,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		waypoint = benthic_waypoint_new(
				strdup( latitude ),
				strdup( longitude ),
				( tolower( *inaccessible_yn ) == 'y' ) );

		waypoint_array[ offset++ ] = waypoint;
	}
	pclose( input_pipe );

	return return_value;

} /* waypoint_get_waypoint_array() */

boolean waypoint_get_sampling_point_candidate_array(
			BENTHIC_WAYPOINT *sampling_point_candidate_array
				[ CANDIDATES_PER_SITE_COLLECTION ],
			BENTHIC_WAYPOINT *waypoint_array[ CANDIDATES_PER_SITE ],
			char *project,
			char *location,
			int site_number,
			char *collection_name )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	char latitude[ 128 ];
	char longitude[ 128 ];
	char position_number_string[ 128 ];
	int position_number;
	char *from;
	char where[ 512 ];
	char where_join[ 512 ];
	FILE *input_pipe;
	char *select;
	BENTHIC_WAYPOINT *waypoint;

	select = "latitude,longitude,position_number";
	from = "sampling_point_candidate,waypoint_position";

	sprintf( where_join,
		 "sampling_point_candidate.waypoint_position =	"
		 "waypoint_position.waypoint_position" );

	sprintf( where,
		 "location = '%s' and		"
		 "project = '%s' and		"
		 "site_number = %d and		"
		 "collection_name = '%s' and	"
		 "%s				",
		 location,
		 project,
		 site_number,
		 collection_name,
		 where_join );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql.e",
		 select,
		 from,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	latitude,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	longitude,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	position_number_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );


		if ( ! ( waypoint = benthic_waypoint_seek(
				waypoint_array,
				latitude,
				longitude,
				CANDIDATES_PER_SITE ) ) )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: cannot seek waypoint (%s,%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 latitude,
				 longitude );
			pclose( input_pipe );
			return 0;
		}

		position_number = atoi( position_number_string );

		if (	position_number < 1
		||	position_number > CANDIDATES_PER_SITE_COLLECTION )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: invalid position number of %d for (%s,%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 position_number,
				 latitude,
				 longitude );
			pclose( input_pipe );
			return 0;
		}

		sampling_point_candidate_array
			[ position_number - 1 ] =
				waypoint;

		waypoint->is_candidate = 1;
	}
	pclose( input_pipe );
	return 1;
} /* waypoint_get_sampling_point_candidate_array() */

BENTHIC_WAYPOINT *benthic_waypoint_seek(
			BENTHIC_WAYPOINT **waypoint_array,
			char *latitude,
			char *longitude,
			int array_length )
{
	int i;
	BENTHIC_WAYPOINT *waypoint;

	for( i = 0; i < array_length; i++ )
	{
		waypoint = waypoint_array[ i ];

		if ( waypoint )
		{
			if ( strcmp( latitude, waypoint->latitude ) == 0
			&&   strcmp( longitude, waypoint->longitude ) == 0 )
			{
				return waypoint;
			}
		}
	}
	return (BENTHIC_WAYPOINT *)0;
} /* benthic_waypoint_seek() */

void waypoint_populate_random_sampling_point_candidate_array(
		BENTHIC_WAYPOINT *sampling_point_candidate_array
			[ CANDIDATES_PER_SITE_COLLECTION ],
		BENTHIC_WAYPOINT *waypoint_array[ CANDIDATES_PER_SITE ] )
{
	int position_offset;
	BENTHIC_WAYPOINT *waypoint;
	int random_number;

	for(	position_offset = 0;
		position_offset < CANDIDATES_PER_SITE_COLLECTION;
		position_offset++ )
	{
		while ( 1 )
		{
			random_number =
				(int)timlib_random( CANDIDATES_PER_SITE ) - 1;

			waypoint = waypoint_array[ random_number ];

			if ( waypoint
			&&   !waypoint->inaccessible
			&&   !waypoint->is_candidate )
			{
				sampling_point_candidate_array
					[ position_offset ] = waypoint;
				waypoint->is_candidate = 1;
				break;
			}
		}

	}
	return;
} /* waypoint_populate_random_sampling_point_candidate_array() */

void waypoint_save_random_sampling_point_candidate_array(
				char *application_name,
				BENTHIC_WAYPOINT *sampling_point_candidate_array
					[ CANDIDATES_PER_SITE_COLLECTION ],
				char *location,
				char *project,
				int site_number,
				char *collection_name )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *table_name;
	char *field_list_string;
	char *waypoint_position_string;
	int position_offset;
	BENTHIC_WAYPOINT *benthic_waypoint;

	if ( !sampling_point_candidate_array ) return;

	table_name = get_table_name(
				application_name,
				"sampling_point_candidate" );

	field_list_string =
"location,project,site_number,latitude,longitude,collection_name,waypoint_position";

	sprintf( sys_string,
	"insert_statement.e table=%s field=\"%s\" delimiter='^' | sql.e",
		 table_name,
		 field_list_string );

	output_pipe = popen( sys_string, "w" );

	for(	position_offset = 0;
		position_offset < CANDIDATES_PER_SITE_COLLECTION;
		position_offset++ )
	{
		if ( ! ( benthic_waypoint =
				sampling_point_candidate_array
					[ position_offset ] ) )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: empty waypoint for position_offset = %d\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 position_offset );
			pclose( output_pipe );
			exit( 1 );
		}

		if ( ! ( waypoint_position_string =
			waypoint_get_position_string(
				application_name,
				position_offset + 1 ) ) )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get waypoint_position_string for position_offset = %d\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 position_offset );
			pclose( output_pipe );
			exit( 1 );
		}

/*
"location,project,site_number,latitude,longitude,collection_name,waypoint_position";
*/
		fprintf( output_pipe,
			 "%s^%s^%d^%s^%s^%s^%s\n",
			 location,
			 project,
			 site_number,
			 benthic_waypoint->latitude,
			 benthic_waypoint->longitude,
			 collection_name,
			 waypoint_position_string );
	}

	pclose( output_pipe );

} /* waypoint_save_random_sampling_point_candidate_array() */

char *waypoint_get_position_string(
				char *application_name,
				int position_number )
{
	static LIST *waypoint_position_record_list = {0};
	char *record;
	static char position_string[ 128 ];
	char search_string[ 128 ];

	if ( !waypoint_position_record_list )
	{
		char sys_string[ 1024 ];
		char *select;

		select = "position_number,waypoint_position";

		sprintf( sys_string,
			 "get_folder_data	application=%s		 "
			 "			select=%s		 "
			 "			folder=waypoint_position ",
			 application_name,
			 select );

		waypoint_position_record_list = pipe2list( sys_string );
	}

	sprintf(	search_string,
			"%d%c",
			position_number,
			FOLDER_DATA_DELIMITER );

	if ( !list_rewind( waypoint_position_record_list ) ) return (char *)0;

	do {
		record = list_get_pointer( waypoint_position_record_list );

		if ( timlib_strncmp( record, search_string ) == 0 )
		{
			piece(	position_string,
				FOLDER_DATA_DELIMITER,
				record,
				1 );
			return position_string;
		}

	} while( list_next( waypoint_position_record_list ) );

	return (char *)0;

} /* waypoint_get_position_string() */

BENTHIC_WAYPOINT **waypoint_optimize_random_sampling_point_candidate_array(
			BENTHIC_WAYPOINT *sampling_point_candidate_array
				[ CANDIDATES_PER_SITE_COLLECTION ] )
{
	int position_offset;
	BENTHIC_WAYPOINT *benthic_waypoint;
	WAYPOINT *starting_waypoint;
	WAYPOINT *waypoint;
	WAYPOINT_TRIP *waypoint_trip;
	BENTHIC_WAYPOINT **reordered_candidate_array;

	/* Set starting_waypoint */
	/* --------------------- */
	if ( ! ( benthic_waypoint = sampling_point_candidate_array [ 0 ] ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty waypoint for position_offset = %d\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 0 );
		exit( 1 );
	}

	starting_waypoint = waypoint_new_degrees_float(
				benthic_waypoint->latitude,
				benthic_waypoint->longitude );

	/* Instantiate waypoint_trip */
	/* ------------------------- */
	waypoint_trip = waypoint_trip_new( starting_waypoint );

	for(	position_offset = 1;
		position_offset < CANDIDATES_PER_SITE_COLLECTION;
		position_offset++ )
	{
		if ( ! ( benthic_waypoint =
				sampling_point_candidate_array
					[ position_offset ] ) )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: empty waypoint for position_offset = %d\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 position_offset );
			exit( 1 );
		}

		waypoint = waypoint_new_degrees_float(
					benthic_waypoint->latitude,
					benthic_waypoint->longitude );

		if ( !waypoint_set_visit(
			waypoint_trip->visit_waypoint_list,
			waypoint ) )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: cannot set visit for position_offset = %d\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 position_offset );
			exit( 1 );
		}

	}

	if ( !waypoint_convert_to_utm(
			waypoint_trip->starting_waypoint,
			waypoint_trip->visit_waypoint_list ) )
	{
			fprintf( stderr,
"ERROR in %s/%s()/%d: cannot convert to UTM.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
	}

	waypoint_trip->array_length =
		waypoint_set_visit_array(
			waypoint_trip->visit_waypoint_array,
			waypoint_trip->visit_waypoint_list );

	if ( ! waypoint_set_optimal_visit_waypoint_array(
		&waypoint_trip->optimal_distance,
		&waypoint_trip->arrangement_input_pipe,
		waypoint_trip->optimal_visit_waypoint_array,
		waypoint_trip->visit_waypoint_array,
		waypoint_trip->starting_waypoint,
		waypoint_trip->array_length ) )
	{
			fprintf( stderr,
"ERROR in %s/%s()/%d: cannot set optimal visit waypoint array.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
	}

	if ( ! ( reordered_candidate_array =
			waypoint_reorder_candidate_array(
				sampling_point_candidate_array,
				waypoint_trip->optimal_visit_waypoint_array,
				waypoint_trip->array_length ) ) )
	{
			fprintf( stderr,
"ERROR in %s/%s()/%d: cannot reorder sampling point candidate array.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
	}

	return reordered_candidate_array;

} /* waypoint_optimize_random_sampling_point_candidate_array() */

BENTHIC_WAYPOINT **waypoint_reorder_candidate_array(
		BENTHIC_WAYPOINT *sampling_point_candidate_array
			[ CANDIDATES_PER_SITE_COLLECTION ],
		WAYPOINT *optimal_visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		int trip_array_length )
{
	int i;
	BENTHIC_WAYPOINT *benthic_waypoint = {0};
	static BENTHIC_WAYPOINT *reordered_sampling_point_candidate_array
			[ CANDIDATES_PER_SITE_COLLECTION ];
	WAYPOINT *waypoint;

	reordered_sampling_point_candidate_array[ 0 ] =
		sampling_point_candidate_array[ 0 ];

	for( i = 0; i < trip_array_length; i++ )
	{
		waypoint = optimal_visit_waypoint_array[ i ];

		if ( !waypoint )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: empty waypoint with i = %d\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 i );
			exit( 1 );
		}

		if ( ! ( benthic_waypoint =
				benthic_waypoint_seek(
					sampling_point_candidate_array,
					waypoint->latitude_degrees_float,
					waypoint->longitude_degrees_float,
					CANDIDATES_PER_SITE_COLLECTION ) ) )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: cannot seek (%s/%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 waypoint->latitude_degrees_float,
				 waypoint->longitude_degrees_float );
			return 0;
		}

		reordered_sampling_point_candidate_array[ i + 1 ] =
			benthic_waypoint;
	}

	return reordered_sampling_point_candidate_array;

} /* waypoint_reorder_candidate_array() */

char *benthic_waypoint_array_display(
			BENTHIC_WAYPOINT **benthic_waypoint_array,
			int array_length )
{
	char return_string[ 65536 ];
	char *ptr = return_string;
	int i;
	BENTHIC_WAYPOINT *benthic_waypoint;

	ptr += sprintf( ptr, "\n" );

	for( i = 0; i < array_length; i++ )
	{
		benthic_waypoint = benthic_waypoint_array[ i ];
		ptr += sprintf( ptr,
				"i = %d: %s\n",
				i,
				benthic_waypoint_display( benthic_waypoint ) );
	}

	return strdup( return_string );

} /* benthic_waypoint_array_display() */

char *benthic_waypoint_display(
			BENTHIC_WAYPOINT *benthic_waypoint )
{
	char return_string[ 1024 ];

	if ( !benthic_waypoint ) return "NULL";

	sprintf( return_string,
"latitude = %s; longitude = %s; inaccessible = %d; is_candidate = %d",
		benthic_waypoint->latitude,
		benthic_waypoint->longitude,
		benthic_waypoint->inaccessible,
		benthic_waypoint->is_candidate );

	return strdup( return_string );
} /* benthic_waypoint_display() */

