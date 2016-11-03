/* ---------------------------------------------------	*/
/* src_sparrow/vegetation_observation.h			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef VEGETATION_OBSERVATION_H
#define VEGETATION_OBSERVATION_H

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
	char *vegetation_name;
	int predominance_rank;
} VEGETATION_OBSERVATION;

/* Prototypes */
/* ---------- */
VEGETATION_OBSERVATION *vegetation_observation_new(
			char *quad_sheet,
			int site_number,
			char *visit_date,
			char *vegetation_name,
			int predominance_rank );

LIST *vegetation_observation_get_list(
			char *application_name,
			char *quad_sheet,
			int site_number,
			char *visit_date );

#endif
