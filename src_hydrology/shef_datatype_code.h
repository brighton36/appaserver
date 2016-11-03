/* src_hydrology/shef_datatype_code.h */
/* ---------------------------------- */

#ifndef SHEF_DATATYPE_CODE_H
#define SHEF_DATATYPE_CODE_H

#include "list.h"
#include "hash_table.h"
#include "station_datatype.h"

#define SHEF_DEFAULT_UPLOAD_STATION	"DBC1"
#define SHEF_AGGREGATE_STUB		"SHEF_STUB"

typedef struct
{
	char *station;
	char *datatype;
	char *measurement_date;
	char *measurement_time;
	double measurement_value;
} SHEF_UPLOAD_AGGREGATE_MEASUREMENT;

typedef struct
{
	char *station;
	char *datatype;
	char *shef_upload_code;
} SHEF_UPLOAD_DATATYPE;

typedef struct
{
	char *station;
	char *datatype;
	char *shef_download_code;
} SHEF_DOWNLOAD_DATATYPE;

typedef struct
{
	LIST *shef_upload_datatype_list;
	LIST *shef_download_datatype_list;
	STATION_DATATYPE_LIST *station_datatype_list;
} SHEF_DATATYPE_CODE;

/* Prototypes */
/* ---------- */
SHEF_UPLOAD_AGGREGATE_MEASUREMENT *
			shef_upload_aggregate_measurement_new(
				char *station,
				char *datatype,
				char *measurement_date,
				char *measurement_time,
				double measurement_value );

SHEF_DATATYPE_CODE *new_shef_datatype_code(
				char *application_name );

char *shef_datatype_code_get_shef_download_code(
			char *station, 
			char *datatype, 
			LIST *shef_download_datatype_list );

char *shef_datatype_code_get_upload_datatype(
				SHEF_UPLOAD_AGGREGATE_MEASUREMENT **
					shef_upload_aggregate_measurement,
				char *station,
				char *shef_code,
				LIST *shef_upload_datatype_list,
				LIST *station_datatype_list,
				char *measurement_date,
				char *measurement_time,
				double measurement_value );

LIST *load_upload_datatype_list( char *application_name );

LIST *load_download_datatype_list( char *application_name );

SHEF_UPLOAD_DATATYPE *shef_get_upload_datatype(
				char *station,
				char *shef_code_upper_case,
				LIST *shef_upload_datatype_list );

boolean shef_is_min_max(	int *str_len,
				char *shef_code_upper_case );

char *shef_datatype_code_get_upload_min_max_datatype(
				SHEF_UPLOAD_AGGREGATE_MEASUREMENT **
					shef_upload_aggregate_measurement,
				HASH_TABLE *shef_upload_hash_table,
				char *station,
				char *datatype_lower_case,
				char *measurement_date,
				char *measurement_time,
				double measurement_value );

char *shef_upload_datatype_get_key(
				char *datatype_name,
				char *measurement_date,
				char *measurement_time );

#endif
