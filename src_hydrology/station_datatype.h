/* $APPASERVER_HOME/src_hydrology/station_datatype.h */
/* ------------------------------------------------- */

#ifndef STATION_DATATYPE_H
#define STATION_DATATYPE_H

#include "list.h"
#include "datatype.h"
#include "units.h"

typedef struct
{
	char *station_name;
	DATATYPE *datatype;
	char *shef_upload_code;
	char *manipulate_agency;
	char *units;
	char aggregation_sum_yn;
	char bar_graph_yn;
	char scale_graph_zero_yn;
	LIST *measurement_list;
} STATION_DATATYPE;

/* Operations */
/* ---------- */
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
				char *station_name,
				char *datatype_name );

STATION_DATATYPE *station_datatype_fetch_new(
				char *application_name,
				char *station_name,
				char *datatype_name );

STATION_DATATYPE *station_datatype_new(
				void );

char *station_datatype_get_manipulate_agency(
				char *application_name,
				char *station,
				char *datatype );

void station_datatype_free(
				STATION_DATATYPE *station_datatype );

char *station_datatype_translate_datatype_name(
				DATATYPE *datatype,
				char *shef_upload_code,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase );

UNITS *station_datatype_list_seek_units(
				LIST *station_datatype_list,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase );

STATION_DATATYPE *station_datatype_parse(
			char *application_name,
			/* -------------------------------------------- */
			/* Only shef_upload_datatpe_list for a station. */
			/* -------------------------------------------- */
		   	LIST *shef_upload_datatype_list,
			char *input_buffer );

LIST *station_datatype_fetch_list(
			char *application_name,
			char *station_name,
			/* -------------------------------------------- */
			/* Only shef_upload_datatpe_list for a station. */
			/* -------------------------------------------- */
			LIST *shef_upload_datatype_list );

LIST *station_datatype_fetch_measurement_list(
			char *application_name,
			char *station_name,
			char *datatype_name,
			char *begin_measurement_date,
			char *end_measurement_date );

#endif
