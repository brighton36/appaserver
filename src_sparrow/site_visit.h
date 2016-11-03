/* ---------------------------------------------------	*/
/* src_sparrow/site_visit.h				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef SITE_VISIT_H
#define SITE_VISIT_H

#include "list.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *quad_sheet;
	int site_number;
	char *visit_date;
	char *visit_time;
	int survey_year;
	int survey_number;
	char *observer;
	int total_vegetation_cover_percent;
	char *reason_no_observations;
	char validated_yn;
	char *person_validating;
	char *date_validated;
	char *comments;
	char *field_sheet;
	LIST *sparrow_observation_list;
	LIST *physical_observation_list;
	LIST *vegetation_observation_list;
} SITE_VISIT;

/* Prototypes */
/* ---------- */
SITE_VISIT *site_visit_new(
			char *quad_sheet,
			int site_number,
			char *visit_date );

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
			char *application_name );

char *site_visit_get_primary_key_where_clause(
			char *quad_sheet,
			int site_number,
			char *visit_date );

#endif
