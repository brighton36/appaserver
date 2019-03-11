/* station_datatype.h */
/* ------------------ */

#ifndef STATION_DATATYPE_H
#define STATION_DATATYPE_H

#include "list.h"

typedef struct
{
	char *datatype_alias;
	char *datatype_name;
} DATATYPE_ALIAS;

typedef struct
{
	char *units_converted;
	double multiply_by;
} UNITS_CONVERTED;

typedef struct
{
	char *units_alias;
	char *units;
} UNITS_ALIAS;

typedef struct
{
	char *units;
	LIST *units_alias_list;
	LIST *units_converted_list;
} UNITS;

typedef struct
{
	char *datatype_name;
	UNITS *units;
	LIST *datatype_alias_list;
} DATATYPE;

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
DATATYPE_ALIAS *station_datatype_alias_new(
				void );

UNITS_CONVERTED *station_datatype_units_converted_new(
				void );

UNITS_ALIAS *station_datatype_units_alias_new(
				void );

UNITS *station_datatype_units_new(
				void );

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
