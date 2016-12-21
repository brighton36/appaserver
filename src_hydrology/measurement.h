/* measurement.h */
/* ------------- */

#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <stdio.h>
#include <stdlib.h>

#define MEASUREMENT_SELECT_LIST	 	"station,datatype,measurement_date,measurement_time,measurement_value"

typedef struct
{
	char *station;
	char *datatype;
	char *measurement_date;
	char *measurement_time;
	char *value_string;
	double measurement_value;
	int null_value;
	int db_null_value;
} MEASUREMENT_RECORD;

typedef struct
{
	char *application_name;
	char *load_process;
	MEASUREMENT_RECORD *measurement_record;
	int record_exists;
	FILE *insert_pipe;
	FILE *input_pipe;
	FILE *delete_pipe;
	FILE *html_table_pipe;
	char *argv_0;
} MEASUREMENT;

/* Constants */
/* --------- */
#define STATION_PIECE		0
#define DATATYPE_PIECE		1
#define DATE_PIECE		2
#define TIME_PIECE		3
#define VALUE_PIECE		4

/* Prototypes */
/* ---------- */
MEASUREMENT *measurement_new_measurement(
					char *application_name );

FILE *measurement_open_input_pipe(	char *application_name,
					char *where_clause,
					char delimiter );

void measurement_record_free(		MEASUREMENT_RECORD *m );
MEASUREMENT_RECORD *measurement_record_new(
					char *station,
					char *datatype,
					char *date,
					char *time,
					char *value_string );

int measurement_exists(			MEASUREMENT *measurement );

void measurement_set_comma_delimited_record( 
					MEASUREMENT *m, 
					char *comma_delimited_record,
					char *argv_0,
					char *database_management_system );

void measurement_insert( 		MEASUREMENT *m,
					int really_yn,
					FILE *html_table_pipe );

void measurement_update_mysql(		char *application_name,
					char *station,
					char *datatype,
					char *date,
					char *time,
					double value );

double measurement_get_value_from_db(	int *record_exists,
					int *db_null_value,
					char *application_name,
					char *station,
					char *datatype,
					char *date,
					char *time );

void measurement_output_insert_pipe(	FILE *insert_pipe,
					char *station,
					char *datatype,
					char *date,
					char *time,
					double value,
					int null_value );

FILE *measurement_open_insert_pipe(	char *application_name,
					int delete_measurements_day,
					int really_yn );

FILE *measurement_open_html_table_pipe(	void );

void measurement_close_insert_pipe( 	FILE *insert_pipe );

double measurement_get_value(		int *null_value,
					char *value_string );

void measurement_set_load_process( 	MEASUREMENT *m,
					char *load_process,
					int really_yn );

void measurement_set_argv_0(		MEASUREMENT *m,
					char *argv_0 );

int measurement_fetch(			MEASUREMENT *m,
					FILE *input_pipe,
					char *database_management_system );

FILE *measurement_open_delete_pipe(	char *application_name );

char *measurement_display(		MEASUREMENT_RECORD *m );

void measurement_delete(		FILE *delete_pipe,
					MEASUREMENT_RECORD *m );

void measurement_open_input_process( 	MEASUREMENT *m,
					char *load_process,
					int really_yn );

void measurement_close_html_table_pipe( FILE *html_table_pipe );

#endif
