/* ---------------------------------------------------	*/
/* src_benthic/benthic_library.c			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "benthic_library.h"
#include "timlib.h"
#include "piece.h"
#include "hash_table.h"

char *benthic_library_get_collection_project_combined_string(
				LIST *collection_list,
				LIST *project_list )
{
	static char return_string[ 65536 ];
	char *collection_name;
	char *project;
	char *project_string;
	char *ptr = return_string;
	boolean first_time = 1;

	if ( !list_rewind( collection_list ) )
	{
		fprintf( stderr,
		"ERROR In %s/%s()/%d: empty collection_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( list_length( collection_list ) != list_length( project_list ) )
	{
		fprintf( stderr,
"ERROR In %s/%s()/%d: the list of collections doesn't match the list of projects.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	list_rewind( project_list );

	do {
		collection_name = list_get_pointer( collection_list );
		project = list_get_pointer( project_list );

		project_string =
			benthic_library_get_project_string(
						collection_name,
						project );

		if ( first_time )
			first_time = 0;
		else
			ptr += sprintf( ptr, "," );

		ptr += sprintf( ptr, "%s", project_string );
		list_next( project_list );

	} while( list_next( collection_list ) );
	return return_string;
} /* benthic_library_get_collection_project_combined_string() */

char *benthic_library_get_project_string(
				char *collection_name,
				char *project )
{
	static char project_string[ 128 ];

	strcpy( project_string, collection_name );
	*project_string = toupper( *project_string );
	strcat( project_string, "/" );
	strcat( project_string, project );
	return project_string;
} /* benthic_library_get_project_string() */

int benthic_library_get_season(
			char *collection_name )
{
	int season;

	if ( timlib_strncmp(	collection_name,
				"spring" ) == 0 )
	{
		season = BENTHIC_LIBRARY_SPRING_SEASON;
	}
	else
	if ( timlib_strncmp(	collection_name,
				"fall" ) == 0 )
	{
		season = BENTHIC_LIBRARY_FALL_SEASON;
	}
	else
	{
		season = 0;
	}

	return season;

} /* benthic_library_get_season() */

char *benthic_library_get_season_string(
				char *species_count_input_buffer )
{
	char collection_name[ 128 ];
	static char season_string[ 8 ];

	if ( !piece( collection_name, ',', species_count_input_buffer, 2  ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot piece (comma,2) in (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 species_count_input_buffer );
		exit( 1 );
	}

	sprintf(season_string,
		"%d",
		benthic_library_get_season(
			collection_name ) );

	return season_string;

} /* benthic_library_get_season_string() */

SAMPLING_POINT_CANDIDATE *benthic_sampling_point_candidate_new(
				char *location,
				char *collection_name,
				int site_number,
				char *latitude,
				char *longitude,
				int position_number )
{
	SAMPLING_POINT_CANDIDATE *sampling_point_candidate;

	if ( ! ( sampling_point_candidate =
		(SAMPLING_POINT_CANDIDATE *)
			calloc( 1, sizeof( SAMPLING_POINT_CANDIDATE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sampling_point_candidate->location = location;
	sampling_point_candidate->collection_name = collection_name;
	sampling_point_candidate->site_number = site_number;
	sampling_point_candidate->latitude = latitude;
	sampling_point_candidate->longitude = longitude;
	sampling_point_candidate->position_number = position_number;

	sampling_point_candidate->hash_table_key =
		benthic_get_sampling_point_candidate_key(
				sampling_point_candidate->location,
				sampling_point_candidate->collection_name,
				sampling_point_candidate->site_number,
				sampling_point_candidate->position_number );
		
	return sampling_point_candidate;

} /* benthic_sampling_point_candidate_new() */

char *benthic_get_sampling_point_candidate_key(
				char *location,
				char *collection_name,
				int site_number,
				int position_number )
{
	char key[ 1024 ];

	sprintf( key,
		 "%s|%s|%d|%d",
		 location,
		 collection_name,
		 site_number,
		 position_number );

	return strdup( key );

} /* benthic_get_sampling_point_candidate_key() */

HASH_TABLE *benthic_get_sampling_point_candidate_hash_table(
				char *application_name,
				LIST *collection_list )
{
	HASH_TABLE *hash_table;
	char sys_string[ 4096 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	char *select;
	char where[ 4096 ];
	char *sampling_point_candidate_table_name;
	char *waypoint_position_table_name;
	char location[ 128 ];
	char collection_name[ 128 ];
	char site_number[ 128 ];
	char latitude[ 128 ];
	char longitude[ 128 ];
	char position_number[ 128 ];
	SAMPLING_POINT_CANDIDATE *sampling_point_candidate;

	sampling_point_candidate_table_name =
		get_table_name( application_name, "sampling_point_candidate" );

	waypoint_position_table_name =
		get_table_name( application_name, "waypoint_position" );

	select =
"location,collection_name,site_number,latitude,longitude,position_number";

	sprintf( where,
		 "%s.waypoint_position = %s.waypoint_position and	"
		 "collection_name in (%s)				",
		 sampling_point_candidate_table_name,
		 waypoint_position_table_name,
		 timlib_with_list_get_in_clause( collection_list ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s,%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 "sampling_point_candidate",
		 "waypoint_position",
		 where );

	hash_table = hash_table_new( hash_table_medium );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	location,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	collection_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	site_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	latitude,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		piece(	longitude,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );

		piece(	position_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			5 );

		sampling_point_candidate =
			benthic_sampling_point_candidate_new(
				strdup( location ),
				strdup( collection_name ),
				atoi( site_number ),
				strdup( latitude ),
				strdup( longitude ),
				atoi( position_number ) );

		hash_table_set_pointer(
			hash_table,
			sampling_point_candidate->hash_table_key,
			sampling_point_candidate );
	}

	pclose( input_pipe );
	return hash_table;

} /* benthic_get_sampling_point_candidate_hash_table() */

