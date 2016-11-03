/* ---------------------------------------------------	*/
/* src_sparrow/physical_observation.h			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef PHYSICAL_OBSERVATION_H
#define PHYSICAL_OBSERVATION_H

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
	char *physical_parameter;
	int measurement_number;
	double measurement_value;
} PHYSICAL_OBSERVATION;

/* Prototypes */
/* ---------- */
PHYSICAL_OBSERVATION *physical_observation_new(
			char *quad_sheet,
			int site_number,
			char *visit_date,
			char *physical_parameter,
			int measurement_number,
			double measurement_value );

LIST *physical_observation_get_list(
			char *application_name,
			char *quad_sheet,
			int site_number,
			char *visit_date );

#endif
