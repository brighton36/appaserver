/* ---------------------------------------------------	*/
/* src_benthic/location.c				*/
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
#include "location.h"

LOCATION *location_new(	char *project,
			char *location_name )
{
	LOCATION *location;

	if ( ! ( location = calloc( 1, sizeof( LOCATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	location->project = project;
	location->location_name = location_name;
	return location;
} /* location_new() */

LIST *location_get_site_list(
				int *grand_total_time,
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name )
{
	char sys_string[ 1024 ];
	char where_clause[ 256 ];
	LIST *site_number_string_list;
	char *site_number_string;
	LIST *site_list;
	SITE *site;

	sprintf( where_clause,
		 "location = '%s' and project = '%s'",
		 location_name,
		 project );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=site_number		"
		 "			folder=site			"
		 "			where=\"%s\"			"
		 "			order=site_number		",
		 application_name,
		 where_clause );

	site_number_string_list = pipe2list( sys_string );

	if ( !list_rewind( site_number_string_list ) ) return (LIST *)0;

	site_list = list_new();

	do {
		site_number_string =
			list_get_pointer( site_number_string_list );

		site = site_new( atoi( site_number_string ) );

		if ( ! site_get_sweep_array(
					site->sweep_array,
					&site->total_time,
					application_name,
					project,
					location_name,
					collection_name,
					site->site_number ) )
		{
			return (LIST *)0;
/*
			fprintf( stderr,
			"Error in %s/%s()/%d: cannot get sweep array.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
*/
		}

		*grand_total_time += site->total_time;
		list_append_pointer( site_list, site );

	} while( list_next( site_number_string_list ) );
	return site_list;
} /* location_get_site_list() */

