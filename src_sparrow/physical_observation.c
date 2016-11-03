/* ---------------------------------------------------	*/
/* src_sparrow/physical_observation.c			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "folder.h"
#include "site_visit.h"
#include "physical_observation.h"
#include "piece.h"
#include "list.h"

PHYSICAL_OBSERVATION *physical_observation_new(
			char *quad_sheet,
			int site_number,
			char *visit_date,
			char *physical_parameter,
			int measurement_number,
			double measurement_value )
{
	PHYSICAL_OBSERVATION *physical_observation;

	if ( ! ( physical_observation =
			calloc( 1, sizeof( PHYSICAL_OBSERVATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	physical_observation->quad_sheet = quad_sheet;
	physical_observation->site_number = site_number;
	physical_observation->visit_date = visit_date;
	physical_observation->physical_parameter = physical_parameter;
	physical_observation->measurement_number = measurement_number;
	physical_observation->measurement_value = measurement_value;

	return physical_observation;
} /* physical_observation_new() */

LIST *physical_observation_get_list(
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
	char physical_parameter[ 128 ];
	char measurement_number[ 128 ];
	char measurement_value[ 128 ];
	LIST *physical_observation_list;
	PHYSICAL_OBSERVATION *physical_observation;

	select = "physical_parameter,measurement_number,measurement_value";

	where_clause =
		site_visit_get_primary_key_where_clause(
			quad_sheet,
			site_number,
			visit_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=physical_observation	"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where_clause );

	input_pipe = popen( sys_string, "r" );
	physical_observation_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	physical_parameter,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	measurement_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	measurement_value,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		physical_observation =
			physical_observation_new(
				quad_sheet,
				site_number,
				visit_date,
				strdup( physical_parameter ),
				atoi( measurement_number ),
				atof( measurement_value ) );

		list_append_pointer(
			physical_observation_list,
			physical_observation );
	}

	pclose( input_pipe );

	return physical_observation_list;

} /* physical_observation_get_list() */

