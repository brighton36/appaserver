/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/hydrology.h		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

#ifndef HYDROLOGY_H
#define HYDROLOGY_H

/* Includes */
/* -------- */
#include "datatype.h"

/* Constants */
/* --------- */
#define TURKEY_POINT_DATE_TIME_PIECE		1
#define TURKEY_POINT_FIRST_COLUMN_PIECE		3

/* Structures */
/* ---------- */
typedef struct
{
	LIST *datatype_list;
} HYDROLOGY_INPUT;

typedef struct
{
	HYDROLOGY_INPUT input;
	LIST *header_column_datatype_list;
} HYDROLOGY;

/* Operations */
/* ---------- */
HYDROLOGY *hydrology_new(	char *application_name );

void hydrology_datatype_list_display(
				LIST *header_column_datatype_list );

LIST *hydrology_get_header_column_datatype_list(
				char *application_name,
				LIST *input_datatype_list,
				char *input_filename,
				int first_column_piece );

#endif
