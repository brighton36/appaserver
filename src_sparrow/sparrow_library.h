/* src_sparrow/sparrow_library.h			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef SPARROW_LIBRARY_H
#define SPARROW_LIBRARY_H

#include <string.h>

/* Prototypes */
/* ---------- */
char *sparrow_library_get_observation_where(
					char *quad_sheet_list_string,
					char *begin_visit_date,
					char *end_visit_date,
					char *table_name );

char *sparrow_library_get_distinct_physical_parameter(
					char *physical_observation,
					char *observation_where );

char *sparrow_library_get_distinct_vegetation_name(
					char *vegetation_observation,
					char *observation_where );

char *sparrow_library_get_site_id(	char *application_name,
					char *quad_sheet,
					int site_number );

char *sparrow_library_get_title_string(
					char *quad_sheet,
					int site_number,
					char *visit_date );

#endif
