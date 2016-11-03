/* ---------------------------------------------------	*/
/* src_benthic/sampling_point.h				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef SAMPLING_POINT_H
#define SAMPLING_POINT_H

#include "list.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *datatype;
	double value;
} ABIOTIC_MEASUREMENT;

typedef struct
{
	char *anchor_date;
	char *anchor_time;
	char *location;
	int site_number;
	char *project;
	char *collection_name;
	int collection_number;
	int location_number;
	char *actual_latitude;
	char *actual_longitude;
	int season_number;
	char *region_code;
	int pink_assessment_area;
	LIST *abiotic_measurement_list;
	char *boat_lead_researcher;
	char *second_boat_researcher;
	char *collection_diver1;
	char *collection_diver2;
	char *begin_collection_dive_time;
	char *end_collection_dive_time;
	char *collection_air_pressure_in_PSI;
	char *collection_air_pressure_out_PSI;
	char *begin_habitat_dive_time;
	char *end_habitat_dive_time;
	char *habitat_diver;
	char *habitat_air_pressure_in_PSI;
	char *habitat_air_pressure_out_PSI;
	char *notes;
	char *bucket_number;
	char *tide;
} SAMPLING_POINT;

/* Prototypes */
/* ---------- */
SAMPLING_POINT *sampling_point_new(
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number );

LIST *sampling_point_get_list(
			char *application_name,
			LIST *collection_list,
			LIST *project_list );

char *sampling_point_get_select_string(
			char *application_name,
			LIST *collection_list,
			LIST *project_list );

char *sampling_point_or_sequence_get_where_clause(
			char *application_name,
			LIST *collection_list,
			LIST *project_list );

char *sampling_point_display_abiotic_heading_label_list(
			LIST *abiotic_measurement_list );

LIST *sampling_point_get_master_datatype_name_list(
			char *application_name );

void sampling_point_abiotic_output(
			FILE *output_file,
			SAMPLING_POINT *sampling_point );

boolean sampling_point_load(
			char **collection_name,
			char **actual_latitude,
			char **actual_longitude,
			char **boat_lead_researcher,
			char **second_boat_researcher,
			char **collection_diver1,
			char **collection_diver2,
			char **begin_collection_dive_time,
			char **end_collection_dive_time,
			char **collection_air_pressure_in_PSI,
			char **collection_air_pressure_out_PSI,
			char **begin_habitat_dive_time,
			char **end_habitat_dive_time,
			char **habitat_diver,
			char **habitat_air_pressure_in_PSI,
			char **habitat_air_pressure_out_PSI,
			char **notes,
			char **bucket_number,
			char **tide,
			char **project,
			SAMPLING_POINT *sampling_point,
			char *application_name );

char *sampling_point_get_primary_key_where_clause(
				char *anchor_date,
				char *anchor_time,
				char *location,
				int site_number );

#endif
