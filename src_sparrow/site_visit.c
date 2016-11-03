/* ---------------------------------------------------	*/
/* src_sparrow/site_visit.c				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "folder.h"
#include "site_visit.h"
#include "piece.h"

SITE_VISIT *site_visit_new(
			char *quad_sheet,
			int site_number,
			char *visit_date )
{
	SITE_VISIT *site_visit;

	if ( ! ( site_visit = calloc( 1, sizeof( SITE_VISIT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	site_visit->quad_sheet = quad_sheet;
	site_visit->site_number = site_number;
	site_visit->visit_date = visit_date;

	return site_visit;
} /* site_visit_new() */

boolean site_visit_load(
			char **visit_time,
			int *survey_year,
			int *survey_number,
			char **observer,
			int *total_vegetation_cover_percent,
			char **reason_no_observations,
			char *validated_yn,
			char **person_validating,
			char **date_validated,
			char **comments,
			char **field_sheet,
			SITE_VISIT *site_visit,
			char *application_name )
{
	char sys_string[ 1024 ];
	char *where_clause;
	char *select;
	char *results;
	char piece_buffer[ 1024 ];

	select =
"visit_time,survey_year,survey_number,observer,total_vegetation_cover_percent,reason_no_observations,validated_yn,person_validating,date_validated,comments,field_sheet";

	where_clause =
		site_visit_get_primary_key_where_clause(
			site_visit->quad_sheet,
			site_visit->site_number,
			site_visit->visit_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=site_visit	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where_clause );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
	*visit_time = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
	*survey_year = atoi( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );
	*survey_number = atoi( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 3 );
	*observer = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 4 );
	*total_vegetation_cover_percent = atoi( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 5 );
	*reason_no_observations = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 6 );
	*validated_yn = *piece_buffer;

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 7 );
	*person_validating = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 8 );
	*date_validated = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 9 );
	*comments = strdup( piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 10 );
	*field_sheet = strdup( piece_buffer );

	return 1;
} /* site_visit_load() */

char *site_visit_get_primary_key_where_clause(
			char *quad_sheet,
			int site_number,
			char *visit_date )
{
	static char where_clause[ 1024 ] = {0};

	if ( *where_clause ) return where_clause;

	sprintf( where_clause,
		 "quad_sheet = '%s' and			"
		 "site_number = %d and			"
		 "visit_date = '%s'			",
		 quad_sheet,
		 site_number,
		 visit_date );

	return where_clause;

} /* site_visit_get_primary_key_where_clause() */

