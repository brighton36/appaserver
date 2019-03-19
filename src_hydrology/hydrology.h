/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/hydrology.h		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

#ifndef HYDROLOGY_H
#define HYDROLOGY_H

/* Includes */
/* -------- */
#include "station_datatype.h"
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
	LIST *station_list;
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
HYDROLOGY *hydrology_new(	void );

HYDROLOGY *hydrology_fetch_new(	char *application_name,
				char *station_name );

LIST *hydrology_get_header_column_datatype_list(
				LIST *station_datatype_list,
				char *station_name,
				char *input_filename,
				int first_column_piece );

char *hydrology_translate_datatype_name(
				LIST *station_datatype_list,
				char *datatype_name );

STATION *hydrology_set_or_get_station(
				LIST *input_station_list,
				char *application_name,
				char *station_name );

STATION *hydrology_get_or_set_station(
				LIST *input_station_list,
				char *application_name,
				char *station_name );

char *hydrology_translate_units_name(
				LIST *station_datatype_list,
				char *units_phrase );

LIST *hydrology_get_header_column_datatype_list(
				LIST *station_datatype_list,
				char *station_name,
				char *input_filename,
				int first_column_piece );

void hydrology_parse_datatype_units_phrase(
				char *datatype_phrase	/* in/out */,
				char *units_phrase	/* in/out */,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase );

char *hydrology_datatype_name_seek_phrase(
				LIST *station_datatype_list,
				char *station_name,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase );

char *hydrology_units_name_seek_phrase(
				LIST *station_datatype_list,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase );

DATATYPE *hydrology_datatype_seek_phrase(
				LIST *station_datatype_list,
				char *station_name,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase );

MEASUREMENT *hydrology_extract_measurement(
				char *input_string,
				STATION_DATATYPE *station_datatype );

#endif
