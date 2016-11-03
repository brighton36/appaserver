/* ---------------------------------------------------	*/
/* src_benthic/site.c					*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "site.h"
#include "sweep.h"
#include "appaserver_library.h"
#include "timlib.h"
#include "piece.h"

SITE *site_new( int site_number )
{
	SITE *site;

	if ( ! ( site = calloc( 1, sizeof( SITE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	site->site_number = site_number;
	return site;
} /* site_new() */

boolean site_get_sweep_array(
			SWEEP *sweep_array[ SWEEP_COUNT ],
			int *total_time,
			char *application_name,
			char *project,
			char *location_name,
			char *collection_name,
			int site_number )
{
	char piece_buffer[ 128 ];
	static LIST *record_list = {0};
	char search_string[ 128 ];
	char *record;
	SWEEP *sweep;
	int sweep_number;

	if ( !record_list )
	{
		record_list =
			site_get_sweep_record_list(
				application_name,
				project,
				location_name,
				collection_name );
	}

	if ( !list_rewind( record_list ) ) return 0;

	sprintf( search_string, "%d^", site_number );

	do {
		record = list_get_pointer( record_list );

		if ( timlib_strncmp( record, search_string ) == 0 )
		{
/*
"site_number,sweep_number,wash_time_minutes,pick_time_minutes,quality_control_time_minutes,sort_minutes,identify_penaeid_minutes,identify_fish_minutes,identify_caridean_minutes,identify_crabs_minutes";
*/
			piece( piece_buffer, '^', record, 1 );

			sweep_number = atoi( piece_buffer );

			if ( sweep_number < 1 || sweep_number > SWEEP_COUNT )
			{
				fprintf( stderr,
		"Error in %s/%s()/%d: invalid sweep number = %d in (%s)\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 sweep_number,
					 record );
				return 0;
			}

			sweep = sweep_new( sweep_number );

			piece( piece_buffer, '^', record, 2 );
			sweep->wash_time_minutes = atoi( piece_buffer );

			piece( piece_buffer, '^', record, 3 );
			sweep->pick_time_minutes = atoi( piece_buffer );

			piece( piece_buffer, '^', record, 4 );
			sweep->quality_control_time_minutes =
					atoi( piece_buffer );

			piece( piece_buffer, '^', record, 5 );
			sweep->sort_minutes = atoi( piece_buffer );

			piece( piece_buffer, '^', record, 6 );
			sweep->identify_penaeid_minutes = atoi( piece_buffer );

			piece( piece_buffer, '^', record, 7 );
			sweep->identify_fish_minutes = atoi( piece_buffer );

			piece( piece_buffer, '^', record, 8 );
			sweep->identify_caridean_minutes = atoi( piece_buffer );

			piece( piece_buffer, '^', record, 9 );
			sweep->identify_crabs_minutes = atoi( piece_buffer );

			*total_time +=
				(
				sweep->wash_time_minutes +
				sweep->pick_time_minutes +
				sweep->quality_control_time_minutes +
				sweep->sort_minutes +
				sweep->identify_penaeid_minutes +
				sweep->identify_fish_minutes +
				sweep->identify_caridean_minutes +
				sweep->identify_crabs_minutes
				);

			sweep_array[ sweep_number - 1 ] = sweep;
		}
	} while( list_next( record_list ) );
	return 1;
} /* site_get_sweep_array() */

LIST *site_get_sweep_record_list(
			char *application_name,
			char *project,
			char *location_name,
			char *collection_name )
{
	char *sweep_table_name;
	char *sampling_point_table_name;
	char select[ 1024 ];
	char where_join[ 1024 ];
	char sampling_point_where[ 1024 ];
	char sys_string[ 1024 ];

	sweep_table_name = get_table_name(
					application_name, 
					"sweep" );

	sampling_point_table_name = get_table_name(
					application_name, 
					"sampling_point" );

	sprintf( select,
"%s.site_number,sweep_number,wash_time_minutes,pick_time_minutes,quality_control_time_minutes,sort_minutes,identify_penaeid_minutes,identify_fish_minutes,identify_caridean_minutes,identify_crabs_minutes",
		 sampling_point_table_name );

	sprintf(	where_join,
			" and %s.anchor_date = %s.anchor_date		"
			" and %s.anchor_time = %s.anchor_time		",
			sweep_table_name,
			sampling_point_table_name,
			sweep_table_name,
			sampling_point_table_name );

	sprintf(	sampling_point_where,
			" and project = '%s'				"
			" and %s.location = '%s'			"
			" and collection_name = '%s'			",
			project,
			sampling_point_table_name,
			location_name,
			collection_name );

	sprintf(	sys_string,
			"echo \"select %s				 "
			"	from %s,%s				 "
			"	where 1 = 1 %s %s;\"			|"
			"sql.e						 ",
			select,
			sweep_table_name,
			sampling_point_table_name,
			where_join,
			sampling_point_where );

/*
fprintf( stderr, "%s\n", sys_string );
*/

	return pipe2list( sys_string );
} /* site_get_sweep_record_list() */

