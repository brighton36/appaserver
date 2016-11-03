/* ---------------------------------------------------	*/
/* src_benthic_spss/benthic_spss.c			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sampling_point.h"
#include "list.h"
#include "timlib.h"
#include "piece.h"
#include "benthic_spss.h"
#include "benthic_species.h"
#include "benthic_library.h"

BENTHIC_SPSS *benthic_spss_new(
				char *application_name,
				LIST *faunal_group_name_list,
				char *select_output_filename )
{
	BENTHIC_SPSS *benthic_spss;
	char *faunal_group_name;
	SPSS_FAUNAL_GROUP *faunal_group;

	if ( ! ( benthic_spss = (BENTHIC_SPSS *)
			calloc( 1, sizeof( BENTHIC_SPSS ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( benthic_spss->input_file =
			fopen( select_output_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 select_output_filename );
		exit( 1 );
	}

	benthic_spss->faunal_group_list = list_new();

	if ( list_rewind( faunal_group_name_list ) )
	{
		do {
			faunal_group_name =
				list_get_pointer(
					faunal_group_name_list );
	
			faunal_group =
				benthic_spss_faunal_group_new(
					faunal_group_name );
	
			faunal_group->benthic_species_list =
			     benthic_species_with_faunal_group_get_species_list(
					application_name,
					faunal_group->faunal_group_name );

			list_append_pointer(
					benthic_spss->faunal_group_list,
					faunal_group );

		} while( list_next( faunal_group_name_list ) );
	}

	timlib_reset_line_queue();
	benthic_reset_old_sampling_point_key();

	return benthic_spss;
} /* benthic_spss_new() */

SPSS_FAUNAL_GROUP *benthic_spss_faunal_group_new(
				char *faunal_group_name )
{
	SPSS_FAUNAL_GROUP *benthic_spss_faunal_group;

	if ( ! ( benthic_spss_faunal_group =
		(SPSS_FAUNAL_GROUP *)
			calloc( 1, sizeof( SPSS_FAUNAL_GROUP ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	benthic_spss_faunal_group->faunal_group_name = faunal_group_name;
	return benthic_spss_faunal_group;

} /* benthic_spss_faunal_group_new() */

boolean benthic_spss_parse_input_buffer(
			char *collection_number,
			char *location_number,
			char *collection_name,
			char *location,
			char *region_code,
			char *pink_code,
			char *site_number,
			char *sweep_number,
			char *scientific_name,
			char *count,
			char *input_buffer,
			char sum_sweeps_yn )
{
	boolean no_animals_found = 0;

	*scientific_name = '\0';

	piece(	collection_number,
		',',
		input_buffer,
		BENTHIC_SPSS_COLLECTION_NUMBER_PIECE );

	piece(	location_number,
		',',
		input_buffer,
		BENTHIC_SPSS_LOCATION_NUMBER_PIECE );

	piece(	collection_name,
		',',
		input_buffer,
		BENTHIC_SPSS_COLLECTION_NAME_PIECE );

	piece(	location,
		',',
		input_buffer,
		BENTHIC_SPSS_LOCATION_PIECE );

	piece(	region_code,
		',',
		input_buffer,
		BENTHIC_SPSS_REGION_CODE_PIECE );

	piece(	pink_code,
		',',
		input_buffer,
		BENTHIC_SPSS_PINK_CODE_PIECE );

	piece(	site_number,
		',',
		input_buffer,
		BENTHIC_SPSS_SITE_NUMBER_PIECE );

	if ( sum_sweeps_yn != 'y' )
	{
		piece(	sweep_number,
			',',
			input_buffer,
			BENTHIC_SPSS_SWEEP_NUMBER_PIECE );

		if ( !piece(	scientific_name,
				',',
				input_buffer,
				BENTHIC_SPSS_NO_SUM_SCIENTIFIC_NAME_PIECE ) )
		{
			no_animals_found = 1;
		}
		else
		{
			piece(	count,
				',',
				input_buffer,
				BENTHIC_SPSS_NO_SUM_COUNT_PIECE );
		}
	}
	else
	{
		if ( !piece(	scientific_name,
				',',
				input_buffer,
				BENTHIC_SPSS_NO_SUM_SCIENTIFIC_NAME_PIECE - 1 ))
		{
			no_animals_found = 1;
		}
		else
		{
			piece(	count,
				',',
				input_buffer,
				BENTHIC_SPSS_NO_SUM_COUNT_PIECE - 1 );
		}
	}

	if ( !*scientific_name ) no_animals_found = 1;

	return no_animals_found;

} /* benthic_spss_parse_input_buffer() */

char *benthic_spss_get_sampling_point_key(
				char *collection_name,
				char *location,
				int site_number,
				int sweep_number,
				char sum_sweeps_yn )
{
	static char key[ SPSS_KEY_SIZE ];

	if ( sum_sweeps_yn != 'y' )
	{
		sprintf(	key,
				"%s|%s|%d|%d",
				collection_name,
				location,
				site_number,
				sweep_number );
	}
	else
	{
		sprintf(	key,
				"%s|%s|%d",
				collection_name,
				location,
				site_number );
	}

	return key;
} /* benthic_spss_get_sampling_point_key() */

BENTHIC_SPECIES *benthic_spss_benthic_species_seek(
			LIST *faunal_group_list,
			char *scientific_name )
{
	SPSS_FAUNAL_GROUP *faunal_group;
	BENTHIC_SPECIES *benthic_species;

	if ( !list_rewind( faunal_group_list ) ) return (BENTHIC_SPECIES *)0;

	do {
		faunal_group = list_get_pointer( faunal_group_list );

		if ( !list_rewind( faunal_group->benthic_species_list ) )
			continue;
		do {
			benthic_species =
				list_get_pointer(
					faunal_group->benthic_species_list );

			if ( timlib_strcmp(
					benthic_species->scientific_name,
					scientific_name ) == 0 )
			{
				return benthic_species;
			}
		} while( list_next( faunal_group->benthic_species_list ) );
	} while( list_next( faunal_group_list ) );

	return (BENTHIC_SPECIES *)0;

} /* benthic_spss_benthic_species_seek() */

SPSS_SPECIES_COUNT *benthic_spss_species_count_new(
			BENTHIC_SPECIES *root_benthic_species,
			int count_per_square_meter )
{
	SPSS_SPECIES_COUNT *spss_species_count;

	if ( ! ( spss_species_count = (SPSS_SPECIES_COUNT *)
			calloc( 1, sizeof( SPSS_SPECIES_COUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	spss_species_count->root_benthic_species = root_benthic_species;
	spss_species_count->count_per_square_meter = count_per_square_meter;
	return spss_species_count;

} /* benthic_spss_species_count_new() */

SPSS_SAMPLING_POINT *benthic_spss_sampling_point_new(
				char *sampling_point_key,
				char *collection_number,
				char *location_number,
				char *collection_name,
				char *location,
				char *region_code,
				char *pink_assessment_area,
				char *site_number,
				char *sweep_number,
				LIST *faunal_group_list,
				char *scientific_name,
				char *count,
				boolean no_animals_found )
{
	SPSS_SAMPLING_POINT *spss_sampling_point;

	if ( ! ( spss_sampling_point = (SPSS_SAMPLING_POINT *)
			calloc( 1, sizeof( SPSS_SAMPLING_POINT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	spss_sampling_point->sampling_point_key = sampling_point_key;
	spss_sampling_point->species_count_list = list_new();

	spss_sampling_point->root_sampling_point =
		sampling_point_new(
			(char *)0 /* anchor_date */,
			(char *)0 /* anchor_time */,
			(char *)0 /* location */,
			0 /* site_number */ );

	spss_sampling_point->
		root_sampling_point->collection_number =
			atoi( collection_number );

	spss_sampling_point->
		root_sampling_point->location_number = atoi( location_number );

	spss_sampling_point->
		root_sampling_point->collection_name = collection_name;

	spss_sampling_point->
		root_sampling_point->location = location;

	spss_sampling_point->
		root_sampling_point->region_code = region_code;

	spss_sampling_point->
		root_sampling_point->pink_assessment_area =
			atoi( pink_assessment_area );

	spss_sampling_point->
		root_sampling_point->site_number = atoi( site_number );

	spss_sampling_point->sweep_number = atoi( sweep_number );

	spss_sampling_point->
		root_sampling_point->season_number =
			benthic_library_get_season(
				collection_name );

	benthic_spss_append_species_count(
			spss_sampling_point->species_count_list,
			faunal_group_list,
			scientific_name,
			count,
			no_animals_found );

	return spss_sampling_point;

} /* benthic_spss_sampling_point_new() */

void benthic_spss_append_species_count(
			LIST *species_count_list,
			LIST *faunal_group_list,
			char *scientific_name,
			char *count,
			boolean no_animals_found )
{
	BENTHIC_SPECIES *root_benthic_species;
	SPSS_SPECIES_COUNT *spss_species_count;

	if ( !no_animals_found
	&&   ( root_benthic_species =
		benthic_spss_benthic_species_seek(
			faunal_group_list,
			scientific_name ) ) )
	{
		spss_species_count =
			benthic_spss_species_count_new(
				root_benthic_species,
				atoi( count ) );

		list_append_pointer(
			species_count_list,
			spss_species_count );
	}

} /* benthic_spss_append_species_count() */

char *benthic_spss_get_heading_string(
				LIST *faunal_group_list,
				char sum_sweeps_yn )
{
	char heading_string[ 65536 ];

	if ( sum_sweeps_yn != 'y' )
	{
		sprintf( heading_string,
",,,,,,,,%s\n"
"collect_num,site_num,collection,location,region,pink_area,season,cell,sweep%s\n",
			 benthic_spss_get_common_name_label_string(
				faunal_group_list ),
		 	 benthic_spss_get_scientific_name_label_string(
				faunal_group_list ) );
	}
	else
	{
		sprintf( heading_string,
",,,,,,,%s\n"
"collect_num,site_num,collection,location,region,pink_area,season,cell%s\n",
			 benthic_spss_get_common_name_label_string(
				faunal_group_list ),
		 	 benthic_spss_get_scientific_name_label_string(
				faunal_group_list ) );
	}

	return strdup( heading_string );

} /* benthic_spss_get_heading_string() */

char *benthic_spss_get_common_name_label_string(
				LIST *faunal_group_list )
{
	static char heading_label_string[ 65536 ];
	char *ptr = heading_label_string;
	SPSS_FAUNAL_GROUP *faunal_group;
	LIST *benthic_species_list;
	BENTHIC_SPECIES *benthic_species;

	if ( !list_rewind( faunal_group_list ) ) return "";

	do {
		faunal_group = list_get_pointer( faunal_group_list );

		benthic_species_list =
			faunal_group->benthic_species_list;

		if ( !list_rewind( benthic_species_list ) ) continue;

		do {
			benthic_species =
				list_get_pointer(
					benthic_species_list );

			ptr += sprintf(	ptr,
				",%s",
				benthic_species->common_name );

		} while( list_next( benthic_species_list ) );

		/* An extra one for the total column */
		/* --------------------------------- */
		ptr += sprintf(	ptr, "," );

	} while( list_next( faunal_group_list ) );

	*ptr = '\0';
	return heading_label_string;
} /* benthic_spss_get_common_name_label_string() */

char *benthic_spss_get_scientific_name_label_string(
				LIST *faunal_group_list )
{
	static char heading_label_string[ 65536 ];
	char *ptr = heading_label_string;
	SPSS_FAUNAL_GROUP *faunal_group;
	LIST *benthic_species_list;
	BENTHIC_SPECIES *benthic_species;

	if ( !list_rewind( faunal_group_list ) ) return "";

	do {
		faunal_group = list_get_pointer( faunal_group_list );

		benthic_species_list =
			faunal_group->benthic_species_list;

		if ( !list_rewind( benthic_species_list ) ) continue;

		do {
			benthic_species =
				list_get_pointer(
					benthic_species_list );

			ptr += sprintf(	ptr,
				",%s",
				benthic_species->scientific_name );

		} while( list_next( benthic_species_list ) );

		ptr += sprintf(	ptr,
				",%s Total",
				faunal_group->faunal_group_name );

	} while( list_next( faunal_group_list ) );

	*ptr = '\0';
	return heading_label_string;

} /* benthic_spss_get_scientific_name_label_string() */

char *benthic_spss_get_sampling_point_output_line(
				SPSS_SAMPLING_POINT *spss_sampling_point,
				LIST *faunal_group_list,
				char sum_sweeps_yn )
{
	static char output_line[ 65536 ];

	if ( sum_sweeps_yn != 'y' )
	{
		sprintf( output_line,
		 "%d,%d,%s,%s,%s,%d,%d,%d,%d%s",
		 spss_sampling_point->root_sampling_point->collection_number,
		 spss_sampling_point->root_sampling_point->location_number,
		 spss_sampling_point->root_sampling_point->collection_name,
		 spss_sampling_point->root_sampling_point->location,
		 spss_sampling_point->root_sampling_point->region_code,
		 spss_sampling_point->root_sampling_point->pink_assessment_area,
		 spss_sampling_point->root_sampling_point->season_number,
		 spss_sampling_point->root_sampling_point->site_number,
		 spss_sampling_point->sweep_number,
		 benthic_spss_sampling_point_display_count_list(
			spss_sampling_point->species_count_list,
			faunal_group_list ) );
	}
	else
	{
		sprintf( output_line,
		 "%d,%d,%s,%s,%s,%d,%d,%d%s",
		 spss_sampling_point->root_sampling_point->collection_number,
		 spss_sampling_point->root_sampling_point->location_number,
		 spss_sampling_point->root_sampling_point->collection_name,
		 spss_sampling_point->root_sampling_point->location,
		 spss_sampling_point->root_sampling_point->region_code,
		 spss_sampling_point->root_sampling_point->pink_assessment_area,
		 spss_sampling_point->root_sampling_point->season_number,
		 spss_sampling_point->root_sampling_point->site_number,
		 benthic_spss_sampling_point_display_count_list(
			spss_sampling_point->species_count_list,
			faunal_group_list ) );
	}

	return output_line;

} /* benthic_spss_get_sampling_point_output_line() */

char *benthic_spss_sampling_point_display_count_list(
			LIST *species_count_list,
			LIST *faunal_group_list )
{
	static char output_line[ 65536 ];
	char *ptr = output_line;
	SPSS_FAUNAL_GROUP *spss_faunal_group;
	SPSS_SPECIES_COUNT *spss_species_count;
	BENTHIC_SPECIES *benthic_species;
	LIST *benthic_species_list;

	benthic_spss_calculate_faunal_group_row_total(
			faunal_group_list,
			species_count_list );

	if ( !list_rewind( faunal_group_list ) ) return "";

	do {
		spss_faunal_group = list_get_pointer( faunal_group_list );
		benthic_species_list = spss_faunal_group->benthic_species_list;

		if ( !list_rewind( benthic_species_list ) )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: benthic_species_list is empty for (%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 spss_faunal_group->faunal_group_name );
			exit( 1 );
		}

		do {
			benthic_species =
				list_get_pointer(
					benthic_species_list );

			if ( ( spss_species_count =
				benthic_spss_species_count_seek(
					species_count_list,
					benthic_species->scientific_name ) ) )
			{
				ptr += sprintf(	ptr,
						",%d",
						spss_species_count->
						count_per_square_meter );
			}
			else
			{
				ptr += sprintf(	ptr, ",0" );
			}

		} while( list_next( benthic_species_list ) );

		ptr += sprintf(	ptr,
				",%d",
				spss_faunal_group->
					faunal_group_row_total );

	} while( list_next( faunal_group_list ) );

	return output_line;

} /* benthic_spss_sampling_point_display_count_list() */

void benthic_spss_calculate_faunal_group_row_total(
			LIST *faunal_group_list,
			LIST *species_count_list )
{
	SPSS_FAUNAL_GROUP *spss_faunal_group;
	SPSS_SPECIES_COUNT *spss_species_count;

	if ( !list_rewind( faunal_group_list ) ) return;

	do {
		spss_faunal_group = list_get_pointer( faunal_group_list );
		spss_faunal_group->faunal_group_row_total = 0;

		if ( !list_rewind( species_count_list ) ) continue;

		do {
			spss_species_count =
				list_get_pointer(
					species_count_list );

			if ( spss_species_count->root_benthic_species
			&&   timlib_strcmp(	spss_species_count->
							root_benthic_species->
							faunal_group,
						spss_faunal_group->
							faunal_group_name )
							== 0 )
			{
				spss_faunal_group->faunal_group_row_total +=
					spss_species_count->
						count_per_square_meter;
			}

		} while( list_next( species_count_list ) );

	} while( list_next( faunal_group_list ) );

} /* benthic_spss_calculate_faunal_group_row_total() */

SPSS_SPECIES_COUNT *benthic_spss_species_count_seek(
				LIST *species_count_list,
				char *scientific_name )
{
	SPSS_SPECIES_COUNT *spss_species_count;

	if ( !list_rewind( species_count_list ) )
		return (SPSS_SPECIES_COUNT *)0;

	do {
		spss_species_count = list_get_pointer( species_count_list );

		if ( timlib_strcmp(	spss_species_count->
					root_benthic_species->
						scientific_name,
					scientific_name ) == 0 )
		{
			return spss_species_count;
		}
	} while( list_next( species_count_list ) );
	return (SPSS_SPECIES_COUNT *)0;

} /* benthic_spss_species_count_seek() */

char *benthic_spss_sampling_point_display(
				SPSS_SAMPLING_POINT *spss_sampling_point )
{
	char buffer[ 65536 ];
	char *ptr = buffer;
	SPSS_SPECIES_COUNT *spss_species_count;
	LIST *species_count_list;

	species_count_list = spss_sampling_point->species_count_list;

	ptr += sprintf( ptr,
			"key = %s: sweep=%d:",
			spss_sampling_point->sampling_point_key,
			spss_sampling_point->sweep_number );

	if ( list_rewind( species_count_list ) )
	{
		do {
			spss_species_count =
				list_get_pointer(
					species_count_list );

			if ( !list_at_head( species_count_list ) )
				ptr += sprintf( ptr, "," );

			ptr += sprintf(	ptr,
					"%s=%d",
					spss_species_count->
						root_benthic_species->
						scientific_name,
					spss_species_count->
						count_per_square_meter );

		} while( list_next( species_count_list ) );
	}

	return strdup( buffer );

} /* benthic_spss_sampling_point_display() */

static char old_sampling_point_key[ SPSS_KEY_SIZE ];

SPSS_SAMPLING_POINT *benthic_spss_get_spss_sampling_point(
				int *all_done,
				FILE *input_file,
				LIST *faunal_group_list,
				char sum_sweeps_yn )
{
	char input_buffer[ 1024 ];
	char collection_number[ 128 ];
	char location_number[ 128 ];
	char collection_name[ 128 ];
	char location[ 128 ];
	char region_code[ 128 ];
	char pink_code[ 128 ];
	char site_number[ 128 ];
	char sweep_number[ 128 ] = {0};
	char scientific_name[ 128 ];
	char count[ 128 ];
	char *sampling_point_key;
	SPSS_SAMPLING_POINT *spss_sampling_point = {0};
	int no_animals_found;

	if ( *all_done ) return (SPSS_SAMPLING_POINT *)0;

	while ( get_line_queue( input_buffer, input_file ) )
	{
		no_animals_found =
			benthic_spss_parse_input_buffer(
				collection_number,
				location_number,
				collection_name,
				location,
				region_code,
				pink_code,
				site_number,
				sweep_number,
				scientific_name,
				count,
				input_buffer,
				sum_sweeps_yn );

		sampling_point_key =
			benthic_spss_get_sampling_point_key(
				collection_name,
				location,
				atoi( site_number ),
				atoi( sweep_number ),
				sum_sweeps_yn );

		/* If first time */
		/* ------------- */
		if ( !*old_sampling_point_key )
		{
			strcpy( old_sampling_point_key, sampling_point_key );

			spss_sampling_point =
				benthic_spss_sampling_point_new(
					strdup( sampling_point_key ),
					collection_number,
					location_number,
					strdup( collection_name ),
					strdup( location ),
					strdup( region_code ),
					pink_code,
					site_number,
					sweep_number,
					faunal_group_list,
					strdup( scientific_name ),
					count,
					no_animals_found );
			continue;
		}

		/* If same block */
		/* ------------- */
		if ( timlib_strcmp(	sampling_point_key,
					old_sampling_point_key ) == 0 )
		{
			benthic_spss_append_species_count(
				spss_sampling_point->species_count_list,
				faunal_group_list,
				strdup( scientific_name ),
				count,
				no_animals_found );
		}
		else
		/* ------------ */
		/* If new block */
		/* ------------ */
		{
			timlib_unget_line( input_buffer );
			*old_sampling_point_key = '\0';
			return spss_sampling_point;
		}

	}

	fclose( input_file );
	*all_done = 1;
	return spss_sampling_point;

} /* benthic_spss_get_spss_sampling_point() */

void benthic_spss_sampling_point_free(
			SPSS_SAMPLING_POINT *spss_sampling_point )
{
spss_sampling_point = ( SPSS_SAMPLING_POINT *)0;
/*
	free( spss_sampling_point->sampling_point_key );
	free( spss_sampling_point->root_sampling_point->collection_name );
	free( spss_sampling_point->root_sampling_point->location );
	free( spss_sampling_point->root_sampling_point->region_code );
	free( spss_sampling_point->root_sampling_point );
	list_free_container( spss_sampling_point->species_count_list );
	free( spss_sampling_point );
*/

} /* benthic_spss_sampling_point_free() */

void benthic_reset_old_sampling_point_key( void )
{
	*old_sampling_point_key = '\0';
}

