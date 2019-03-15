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
#include "station.h"

/* Constants */
/* --------- */
#define TURKEY_POINT_DATE_TIME_PIECE		1
#define TURKEY_POINT_FIRST_COLUMN_PIECE		3

/* Structures */
/* ---------- */
typedef struct
{
	STATION *station;
} HYDROLOGY_INPUT;

typedef struct
{
	LIST *header_column_datatype_list;
} HYDROLOGY_OUTPUT;

typedef struct
{
	HYDROLOGY_INPUT input;
	HYDROLOGY_OUTPUT output;
} HYDROLOGY;

/* Operations */
/* ---------- */
HYDROLOGY *hydrology_new(	char *application_name,
				char *station_name );

void hydrology_datatype_list_display(
				LIST *header_column_datatype_list );

LIST *hydrology_get_header_column_datatype_list(
				char *application_name,
				char *station,
				char *input_filename,
				int first_column_piece,
				LIST *input_datatype_list,
				LIST *input_units_list );

char *hydrology_translate_datatype_name(
				LIST *station_datatype_list,
				char *datatype_name );

#endif
