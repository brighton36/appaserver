/* ---------------------------------------------------	*/
/* src_benthic/benthic_library.h			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef BENTHIC_LIBRARY_H
#define BENTHIC_LIBRARY_H

/* Includes */
/* -------- */
#include "list.h"
#include "hash_table.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define BENTHIC_LIBRARY_SPRING_SEASON	1
#define BENTHIC_LIBRARY_FALL_SEASON	2

/* Structures */
/* ---------- */
typedef struct
{
	char *hash_table_key;
	char *location;
	int site_number;
	char *latitude;
	char *longitude;
	char *collection_name;
	int position_number;
} SAMPLING_POINT_CANDIDATE;

/* Prototypes */
/* ---------- */
SAMPLING_POINT_CANDIDATE *benthic_sampling_point_candidate_new(
				char *location,
				char *collection_name,
				int site_number,
				char *latitude,
				char *longitude,
				int position_number );

char *benthic_get_sampling_point_candidate_key(
				char *location,
				char *collection_name,
				int site_number,
				int position_number );

HASH_TABLE *benthic_get_sampling_point_candidate_hash_table(
				char *application_name,
				LIST *collection_list );

char *benthic_library_get_collection_project_combined_string(
				LIST *collection_list,
				LIST *project_list );

char *benthic_library_get_project_string(
				char *collection_name,
				char *project );

int benthic_library_get_season(
				char *collection_name );

char *benthic_library_get_season_string(
				char *species_count_input_buffer );

#endif
