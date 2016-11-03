/* ---------------------------------------------------	*/
/* src_benthic/benthic_waypoint.h			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef BENTHIC_WAYPOINT_H
#define BENTHIC_WAYPOINT_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "waypoint.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define CANDIDATES_PER_SITE			100
#define CANDIDATES_PER_SITE_COLLECTION		5
#define SITES_PER_LOCATION			30

/* Structures */
/* ---------- */
typedef struct
{
	char *latitude;
	char *longitude;
	boolean inaccessible;
	boolean is_candidate;
} BENTHIC_WAYPOINT;

typedef struct
{
	char *location;
	char *project;
	int site_number;
	BENTHIC_WAYPOINT *waypoint_array[ CANDIDATES_PER_SITE ];
	BENTHIC_WAYPOINT *sampling_point_candidate_array
			[ CANDIDATES_PER_SITE_COLLECTION ];
} WAYPOINT_SITE;

/* Prototypes */
/* ---------- */
WAYPOINT_SITE *waypoint_site_new(
			char *application_name,
			char *location,
			char *project,
			int site_number );

BENTHIC_WAYPOINT *benthic_waypoint_new(
			char *latitude,
			char *longitude,
			boolean inaccessible );

boolean waypoint_get_waypoint_array(
			BENTHIC_WAYPOINT *waypoint_array[ CANDIDATES_PER_SITE ],
			char *application_name,
			char *project,
			char *location,
			int site_number );

BENTHIC_WAYPOINT *benthic_waypoint_seek(
			BENTHIC_WAYPOINT **waypoint_array,
			char *latitude,
			char *longitude,
			int array_length );

boolean waypoint_get_sampling_point_candidate_array(
			BENTHIC_WAYPOINT *sampling_point_candidate_array
				[ CANDIDATES_PER_SITE_COLLECTION ],
			BENTHIC_WAYPOINT *waypoint_array[ CANDIDATES_PER_SITE ],
			char *project,
			char *location,
			int site_number,
			char *collection_name );

void waypoint_populate_random_sampling_point_candidate_array(
		BENTHIC_WAYPOINT *sampling_point_candidate_array
			[ CANDIDATES_PER_SITE_COLLECTION ],
		BENTHIC_WAYPOINT *waypoint_array[ CANDIDATES_PER_SITE ] );

void waypoint_save_random_sampling_point_candidate_array(
			char *application_name,
			BENTHIC_WAYPOINT *sampling_point_candidate_array
				[ CANDIDATES_PER_SITE_COLLECTION ],
			char *location,
			char *project,
			int site_number,
			char *collection_name );

char *waypoint_get_position_string(
			char *application_name,
			int position_number );

BENTHIC_WAYPOINT **waypoint_optimize_random_sampling_point_candidate_array(
			BENTHIC_WAYPOINT *sampling_point_candidate_array
				[ CANDIDATES_PER_SITE_COLLECTION ] );

BENTHIC_WAYPOINT **waypoint_reorder_candidate_array(
		BENTHIC_WAYPOINT *sampling_point_candidate_array
			[ CANDIDATES_PER_SITE_COLLECTION ],
		WAYPOINT *optimal_visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		int trip_array_length );

char *benthic_waypoint_display(
			BENTHIC_WAYPOINT *benthic_waypoint );

char *benthic_waypoint_array_display(
			BENTHIC_WAYPOINT **benthic_waypoint_array,
			int array_length );

#endif
