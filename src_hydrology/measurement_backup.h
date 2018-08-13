/* measurement_backup.h */
/* -------------------- */

#ifndef MEASUREMENT_BACKUP_H
#define MEASUREMENT_BACKUP_H

#include "boolean.h"

typedef struct
{
	char *application_name;
	char *station;
	char *datatype;
	char *measurement_date;
	char *measurement_time;
	char *measurement_update_date;
	char *measurement_update_time;
	char *measurement_update_method;
	char *login_name;
	char *original_value_string;
	FILE *insert_pipe;
	char really_yn;
} MEASUREMENT_BACKUP;

/* Constants */
/* --------- */
#define DELETE_MEASUREMENT_UPDATE_METHOD	"block_delete"

#define MEASUREMENT_BACKUP_STATION_PIECE				0
#define MEASUREMENT_BACKUP_DATATYPE_PIECE				1
#define MEASUREMENT_BACKUP_DATE_PIECE					2
#define MEASUREMENT_BACKUP_TIME_PIECE					3
#define MEASUREMENT_BACKUP_VALUE_PIECE					4

#define MEASUREMENT_BACKUP_INSERT_COLUMN_COMMA_LIST	 	"station,datatype,measurement_date,measurement_time,measurement_update_date,measurement_update_time,measurement_update_method,login_name,original_value"

/* Prototypes */
/* ---------- */
MEASUREMENT_BACKUP *measurement_backup_new(	
					char *application_name,
					char *measurement_update_method,
					char *login_name,
					char *measurement_update_date,
					char *measurement_update_time );

MEASUREMENT_BACKUP *new_measurement_backup(	
					char *application_name,
					char *measurement_update_method,
					char *login_name,
					char *measurement_update_date,
					char *measurement_update_time );

FILE *measurement_backup_open_insert_pipe(
					char *application_name );
void measurement_backup_insert(		FILE *insert_pipe,
					char *measurement_update_date,
					char *measurement_update_time,
					char *measurement_update_method,
					char *login_name,
					char *comma_delimited_record,
					char really_yn,
					char delimiter );

void measurement_backup_close(		FILE *insert_pipe );
#endif
