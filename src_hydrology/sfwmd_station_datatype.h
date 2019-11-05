/* sfwmd_station_datatype.h */
/* ------------------------ */

#ifndef SFWMD_STATION_DATATYPE_H
#define SFWMD_STATION_DATATYPE_H

#include "list.h"
#include "hash_table.h"

typedef struct
{
	char *station;
	char *datatype;
	char *db_key;
	double measurement_value;
	boolean is_null;
	char measurement_update_method[ 64 ];
	char last_validation_date_string[ 16 ];
	char measurement_date_string[ 16 ];
	char measurement_time_string[ 16 ];
	double conversion_factor;
} SFWMD_STATION_DATATYPE;

#define SFWMD_STATION_DATATYPE_FOLDER	"dbhydro_dfe"

HASH_TABLE *load_sfwmd_station_datatype_hash_table(
					char *application_name );

SFWMD_STATION_DATATYPE *new_sfwmd_station_datatype(
					void );

#endif
