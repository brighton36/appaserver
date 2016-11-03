/* ---------------------------------------------------	*/
/* src_benthic/site.h					*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef SITE_H
#define SITE_H

#include "sweep.h"
#include "list.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define SWEEP_COUNT			5

/* Structures */
/* ---------- */
typedef struct
{
	char *location;
	char *project;
	int site_number;
	SWEEP *sweep_array[ SWEEP_COUNT ];
	int total_time;
} SITE;

/* Prototypes */
/* ---------- */
SITE *site_new(		int site_number );

boolean site_get_sweep_array(
			SWEEP *sweep_array[ SWEEP_COUNT ],
			int *total_time,
			char *application_name,
			char *project,
			char *location_name,
			char *collection_name,
			int site_number );

LIST *site_get_sweep_record_list(
			char *application_name,
			char *project,
			char *location_name,
			char *collection_name );

#endif
