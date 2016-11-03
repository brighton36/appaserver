/* ---------------------------------------------------	*/
/* src_sparrow/vegetation_observation.c			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "folder.h"
#include "site_visit.h"
#include "vegetation_observation.h"
#include "piece.h"
#include "list.h"

VEGETATION_OBSERVATION *vegetation_observation_new(
			char *quad_sheet,
			int site_number,
			char *visit_date,
			char *vegetation_name,
			int predominance_rank )
{
	VEGETATION_OBSERVATION *vegetation_observation;

	if ( ! ( vegetation_observation =
			calloc( 1, sizeof( VEGETATION_OBSERVATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	vegetation_observation->quad_sheet = quad_sheet;
	vegetation_observation->site_number = site_number;
	vegetation_observation->visit_date = visit_date;
	vegetation_observation->vegetation_name = vegetation_name;
	vegetation_observation->predominance_rank = predominance_rank;

	return vegetation_observation;
} /* vegetation_observation_new() */

LIST *vegetation_observation_get_list(
			char *application_name,
			char *quad_sheet,
			int site_number,
			char *visit_date )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char *where_clause;
	char *select;
	char input_buffer[ 512 ];
	char vegetation_name[ 128 ];
	char predominance_rank[ 128 ];
	LIST *vegetation_observation_list;
	VEGETATION_OBSERVATION *vegetation_observation;

	select = "vegetation_name,predominance_rank";

	where_clause =
		site_visit_get_primary_key_where_clause(
			quad_sheet,
			site_number,
			visit_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=vegetation_observation	"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where_clause );

	input_pipe = popen( sys_string, "r" );
	vegetation_observation_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	vegetation_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	predominance_rank,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		vegetation_observation =
			vegetation_observation_new(
				quad_sheet,
				site_number,
				visit_date,
				strdup( vegetation_name ),
				atoi( predominance_rank ) );

		list_append_pointer(
			vegetation_observation_list,
			vegetation_observation );
	}

	pclose( input_pipe );

	return vegetation_observation_list;

} /* vegetation_observation_get_list() */

