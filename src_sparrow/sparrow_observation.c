/* ---------------------------------------------------	*/
/* src_sparrow/sparrow_observation.c			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "folder.h"
#include "site_visit.h"
#include "sparrow_observation.h"
#include "piece.h"
#include "list.h"

SPARROW_OBSERVATION *sparrow_observation_new(
			char *quad_sheet,
			int site_number,
			char *visit_date,
			char *distance_from_observer_m,
			char *azimuth_degrees,
			int bird_count )
{
	SPARROW_OBSERVATION *sparrow_observation;

	if ( ! ( sparrow_observation =
			calloc( 1, sizeof( SPARROW_OBSERVATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sparrow_observation->quad_sheet = quad_sheet;
	sparrow_observation->site_number = site_number;
	sparrow_observation->visit_date = visit_date;

	sparrow_observation->distance_from_observer_m =
		distance_from_observer_m;

	sparrow_observation->azimuth_degrees = azimuth_degrees;
	sparrow_observation->bird_count = bird_count;

	return sparrow_observation;
} /* sparrow_observation_new() */

LIST *sparrow_observation_get_list(
			char *application_name,
			char *quad_sheet,
			int site_number,
			char *visit_date )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char *where_clause;
	char *select;
	char input_buffer[ 1024 ];
	char distance_from_observer_m[ 128 ];
	char azimuth_degrees[ 128 ];
	char bird_count[ 128 ];
	LIST *sparrow_observation_list;
	SPARROW_OBSERVATION *sparrow_observation;

	select = "distance_from_observer_m,azimuth_degrees,bird_count";

	where_clause =
		site_visit_get_primary_key_where_clause(
			quad_sheet,
			site_number,
			visit_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=sparrow_observation	"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where_clause );

	input_pipe = popen( sys_string, "r" );
	sparrow_observation_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	distance_from_observer_m,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	azimuth_degrees,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	bird_count,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		sparrow_observation =
			sparrow_observation_new(
				quad_sheet,
				site_number,
				visit_date,
				strdup( distance_from_observer_m ),
				strdup( azimuth_degrees ),
				atoi( bird_count ) );

		list_append_pointer(
			sparrow_observation_list,
			sparrow_observation );
	}

	pclose( input_pipe );

	return sparrow_observation_list;

} /* sparrow_observation_get_list() */

