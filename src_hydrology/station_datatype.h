/* station_datatype.h */
/* ------------------ */

#ifndef STATION_DATATYPE_H
#define STATION_DATATYPE_H

#include "list.h"

typedef struct
{
	char *station;
	char *datatype;
	char *manipulate_agency;
	char *units;
	char aggregation_sum_yn;
	char bar_graph_yn;
	char scale_graph_zero_yn;
} STATION_DATATYPE;

/* Operations */
/* ---------- */
LIST *station_datatype_fetch_list(
				char *application_name );

STATION_DATATYPE *station_datatype_list_seek(
				LIST *station_datatype_list,
				char *station,
				char *datatype );

LIST *station_datatype_get_station_datatype_list(
				char *application_name,
				LIST *station_list,
				LIST *datatype_list );

STATION_DATATYPE *station_datatype_get_station_datatype(
				char *application_name,
				char *station,
				char *datatype );

STATION_DATATYPE *station_datatype_new(
				void );

char *station_datatype_get_manipulate_agency(
				char *application_name,
				char *station,
				char *datatype );

#endif
