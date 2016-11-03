/* ---------------------------------------------------	*/
/* src_benthic/location.h				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef LOCATION_H
#define LOCATION_H

#include "list.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *project;
	char *location_name;
	LIST *site_list;
	int grand_total_time;
} LOCATION;

/* Prototypes */
/* ---------- */
LOCATION *location_new(		char *project,
				char *location_name );

LIST *location_get_site_list(
				int *grand_total_time,
				char *application_name,
				char *project,
				char *collection_name,
				char *location_name );

#endif
