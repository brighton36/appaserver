/* $APPASERVER_HOME/src_hydrology/measurement.h */
/* -------------------------------------------- */

#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"

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

typedef struct
{
	char *station;
	char *datatype;
	DICTIONARY *date_time_frequency_dictionary;
} MEASUREMENT_FREQUENCY_STATION_DATATYPE;

typedef struct
{
	LIST *frequency_station_datatype_list;

} MEASUREMENT_FREQUENCY;

/* Constants */
/* --------- */
#define STATION_PIECE		0
#define DATATYPE_PIECE		1
#define DATE_PIECE		2
#define TIME_PIECE		3
#define VALUE_PIECE		4

/* Prototypes */
/* ---------- */
MEASUREMENT_FREQUENCY *measurement_frequency_new(
					void );

MEASUREMENT_FREQUENCY_STATION_DATATYPE *
				measurement_frequency_station_datatype_new(
					char *application_name,
					char *station,
					char *datatype,
					char *begin_measurement_date,
					char *end_measurement_date );

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

void measurement_set_comma_delimited_record( 
					MEASUREMENT *m, 
					char *comma_delimited_record,
					char *argv_0 );

void measurement_insert( 		MEASUREMENT *m,
					int really_yn,
					FILE *html_table_pipe );

/*
void measurement_update_mysql(		char *application_name,
					char *station,
					char *datatype,
					char *date,
					char *time,
					double value );
*/

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
					FILE *input_pipe );

FILE *measurement_open_delete_pipe(	char *application_name );

char *measurement_display(		MEASUREMENT_RECORD *m );

void measurement_delete(		FILE *delete_pipe,
					MEASUREMENT_RECORD *m );

void measurement_open_input_process( 	MEASUREMENT *m,
					char *load_process,
					int really_yn );

void measurement_close_html_table_pipe( FILE *html_table_pipe );

DICTIONARY *measurement_get_date_time_frequency_dictionary(
					char *application_name,
					char *station,
					char *datatype,
					char *begin_measurement_date_string,
					char *end_measurement_date_string );

boolean measurement_date_time_frequency_exists(
				DICTIONARY *date_time_frequency_dictionary,
				char *measurement_date_string,
				char *measurement_time_string );

MEASUREMENT_FREQUENCY_STATION_DATATYPE *
		measurement_frequency_get_or_set_station_datatype(
					LIST *frequency_station_datatype_list,
					char *application_name,
					char *station,
					char *datatype,
					char *begin_measurement_date,
					char *end_measurement_date );

#endif
