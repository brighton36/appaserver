/* alt_station.c					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "alt_station.h"
#include "piece.h"

ALT_STATION *alt_station_new(	char *model,
				char *alternative,
				char *station_name )
{
	ALT_STATION *s = (ALT_STATION *)calloc( 1, sizeof( ALT_STATION ) );

	if ( !s )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	s->model = model;
	s->alternative = alternative;
	s->station_name = station_name;
	return s;
} /* alt_station_new() */

static LIST *global_alt_station_list = {0};

double alt_station_get_ground_surface_elevation(
				boolean *station_not_found,
				char **station_elevation_null,
				char *application_name,
				char *model,
				char *alternative,
				char *station_name )
{
	ALT_STATION *alt_station;

	if ( !global_alt_station_list )
	{
		global_alt_station_list =
			alt_station_get_global_alt_station_list(
				application_name );
	}

	if ( ! ( alt_station = alt_station_seek(
				model,
				alternative,
				station_name,
				global_alt_station_list ) ) )
	{
		*station_not_found = 1;
		return 0.0;
	}
	else
	{
		*station_not_found = 0;
	}

	if ( alt_station->station_elevation_null )
		*station_elevation_null = station_name;
	else
		*station_elevation_null = (char *)0;

	return alt_station->ground_surface_elevation;
} /* alt_station_get_ground_surface_elevation() */

LIST *alt_station_get_global_alt_station_list(
				char *application_name )
{
	char sys_string[ 1024 ];
	char *select_list_string =
		"model,alt,station,ground_surface_elevation_ft";
	LIST *record_list;
	LIST *alt_station_list;
	char *record;
	char model[ 128 ];
	char alternative[ 128 ];
	char station[ 128 ];
	char ground_surface_elevation_string[ 16 ];
	ALT_STATION *alt_station;

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=alt_station		",
		application_name,
		select_list_string );

	record_list = pipe2list( sys_string );

	if ( !list_rewind( record_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot get alt_station list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	alt_station_list = list_new_list();
	do {
		record = list_get_pointer( record_list );

		piece(	model,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		piece(	alternative,
			FOLDER_DATA_DELIMITER,
			record,
			1 );

		piece(	station,
			FOLDER_DATA_DELIMITER,
			record,
			2 );

		alt_station =
			alt_station_new(
				strdup( model ),
				strdup( alternative ),
				strdup( station ) );

		piece(	ground_surface_elevation_string,
			FOLDER_DATA_DELIMITER,
			record,
			3 );

		if ( !*ground_surface_elevation_string )
			alt_station->station_elevation_null = 1;
		else
			alt_station->ground_surface_elevation =
				atof( ground_surface_elevation_string );

		list_append_pointer( alt_station_list, alt_station );

	} while( list_next( record_list ) );

	list_free_string_list( record_list );
	return alt_station_list;
} /* alt_station_get_global_alt_station_list() */

ALT_STATION *alt_station_seek(	char *model,
				char *alternative,
				char *station_name,
				LIST *alt_station_list )
{
	ALT_STATION *alt_station;

	if ( list_rewind( alt_station_list ) )
	{
		do {
			alt_station = list_get_pointer( alt_station_list );

			if ( strcmp(	model,
					alt_station->model ) == 0
			&&   strcmp(	alternative,
					alt_station->alternative ) == 0
			&&   strcmp(	station_name,
					alt_station->station_name ) == 0 )
			{
				return alt_station;
			}
		} while( list_next( alt_station_list ) );
	}

	return (ALT_STATION *)0;
} /* alt_station_seek() */

