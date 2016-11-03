/* ---------------------------------------------------	*/
/* src_sparrow/sparrow_observation.h			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef SPARROW_OBSERVATION_H
#define SPARROW_OBSERVATION_H

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
	char *distance_from_observer_m;
	char *azimuth_degrees;
	int bird_count;
} SPARROW_OBSERVATION;

/* Prototypes */
/* ---------- */
SPARROW_OBSERVATION *sparrow_observation_new(
			char *quad_sheet,
			int site_number,
			char *visit_date,
			char *distance_from_observer_m,
			char *azimuth_degrees,
			int bird_count );

LIST *sparrow_observation_get_list(
			char *application_name,
			char *quad_sheet,
			int site_number,
			char *visit_date );

#endif
