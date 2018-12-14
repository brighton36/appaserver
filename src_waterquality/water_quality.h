/* ---------------------------------------------------	*/
/* src_waterquality/water_quality.h			*/
/* ---------------------------------------------------	*/
/* This is the library for the water quality project.	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef WATER_QUALITY_H
#define WATER_QUALITY_H

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define COLLECTION_DATE_HEADING_KEY		"collection_date_heading"
#define COLLECTION_TIME_HEADING_KEY		"collection_time_heading"
#define COLLECTION_DEPTH_METERS_HEADING_KEY	\
	"collection_depth_meters_heading"
#define STATION_HEADING_KEY			"station_heading"
#define STATION_LATITUDE_HEADING_KEY		"station_latitude_heading"
#define STATION_LONGITUDE_HEADING_KEY		"station_longitude_heading"

/* Structures */
/* ---------- */
typedef struct
{
	char *parameter_name;
	char *units;
} PARAMETER_UNIT;

typedef struct
{
	PARAMETER_UNIT *parameter_unit;
	char *concentration;
} RESULTS;

typedef struct
{
	char *collection_date;
	char *collection_time;
	char *collection_depth_meters;
} COLLECTION;

typedef struct
{
	PARAMETER_UNIT *parameter_unit;
} STATION_PARAMETER;

typedef struct
{
	char *station_name;
	char *longitude;
	char *latitude;
	LIST *station_parameter_list;
	LIST *collection_list;
} STATION;

typedef struct
{
	LIST *station_list;
	LIST *parameter_unit_list;
} WATER_PROJECT;

typedef struct
{
	char *parameter_unit_alias;
	PARAMETER_UNIT *parameter_unit;
} PARAMETER_UNIT_ALIAS;

typedef struct
{
	char *unit_alias;
	char *units;
} UNIT_ALIAS;

typedef struct
{
	char *parameter_alias;
	char *parameter_name;
} PARAMETER_ALIAS;

typedef struct
{
	char *station_collection_attribute;
	PARAMETER_UNIT *parameter_unit;
	int column_piece;
} LOAD_COLUMN;

typedef struct
{
	WATER_PROJECT *water_project;
	LIST *parameter_alias_list;
	LIST *unit_alias_list;
	LIST *parameter_name_list;
	LIST *unit_name_list;
} WATER_QUALITY_INPUT;

typedef struct
{
	WATER_QUALITY_INPUT water_quality_input;
	LIST *parameter_unit_alias_list;
	LIST *load_column_list;
} WATER_QUALITY;

/* Prototypes */
/* ---------- */
LIST *water_fetch_parameter_name_list(
				char *application_name );

LIST *water_fetch_unit_name_list(
				char *application_name );

LIST *water_fetch_parameter_alias_list(
				char *application_name );

LIST *water_fetch_unit_alias_list(
				char *application_name );

char *water_quality_get_project_name(
				char *application_name,
				char *project_code );

char *water_quality_get_parameter_name(
				char **matrix,
				char *application_name,
				char *parameter_code );

PARAMETER_UNIT_ALIAS *water_parameter_unit_alias_new(
				char *parameter_unit_alias,
				char *parameter_name,
				char *units );

UNIT_ALIAS *water_unit_alias_new(
				void );

WATER_QUALITY *water_quality_new(
				char *application_name,
				char *project_name );

WATER_PROJECT *water_project_new(
				char *application_name,
				char *project_name );

STATION_PARAMETER *water_new_station_parameter(
				void );

LOAD_COLUMN *water_new_load_column(
				void );

PARAMETER_ALIAS *water_new_parameter_alias(
				void );

STATION *water_new_station(
				void );

COLLECTION *water_collection_new(
				void );

RESULTS *water_results_new(
				void );

PARAMETER_UNIT *water_new_parameter_unit(
				char *parameter_name,
				char *units );

LIST *water_fetch_parameter_unit_list(
				char *application_name,
				char *project_name );

LIST *water_fetch_station_parameter_list(
				char *application_name,
				char *station_name );

LIST *water_fetch_station_list(	char *application_name,
				char *project_name );

LIST *water_fetch_load_column_list(
				char *error_message,
				char *load_table_filename,
				LIST *parameter_unit_list,
				LIST *parameter_alias_list,
				DICTIONARY *application_constants_dictionary );

PARAMETER_ALIAS *water_seek_parameter_alias(
				char *parameter_alias_string,
				LIST *parameter_alias_list );

PARAMETER_UNIT *water_seek_parameter_unit(
				char *parameter_name,
				LIST *parameter_unit_list );

PARAMETER_UNIT *water_seek_filename_parameter_unit(
				char *parameter_alias_string,
				LIST *parameter_unit_list,
				LIST *parameter_alias_list );

int water_get_station_collection_attribute_piece(
				char *station_collection_attribute,
				LIST *load_column_list );

LOAD_COLUMN *water_get_next_load_column_parameter_unit(
				LIST *load_column_list );

void water_seek_application_constants_dictionary(
				LIST **collection_date_heading_list,
				LIST **collection_time_heading_list,
				LIST **collection_depth_meters_heading_list,
				LIST **station_heading_list,
				LIST **station_latitude_heading_list,
				LIST **station_longitude_heading_list,
				DICTIONARY *application_constants_dictionary );

LIST *water_get_parameter_unit_alias_list(
				LIST *parameter_name_list,
				LIST *unit_name_list,
				LIST *parameter_alias_list,
				LIST *unit_alias_list );

#endif
