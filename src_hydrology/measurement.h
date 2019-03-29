/* $APPASERVER_HOME/src_hydrology/measurement.h */
/* -------------------------------------------- */

#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"
#include "boolean.h"

#define MEASUREMENT_SELECT_LIST	 	"station,datatype,measurement_date,measurement_time,measurement_value"

typedef struct
{
	char *station_name;
	char *datatype;
	char *measurement_date;
	char *measurement_time;
	char *value_string;
	double measurement_value;
	boolean null_value;
	double delta_value;
} MEASUREMENT;

typedef struct
{
	char *application_name;
	char *load_process;
	MEASUREMENT *measurement;
	FILE *insert_pipe;
	FILE *input_pipe;
	FILE *delete_pipe;
	FILE *html_table_pipe;
	char *argv_0;
} MEASUREMENT_STRUCTURE;

typedef struct
{
	char *station_name;
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
					char *station_name,
					char *datatype );

MEASUREMENT_STRUCTURE *measurement_structure_new(
					char *application_name );

FILE *measurement_open_input_pipe(	char *application_name,
					char *where_clause,
					char delimiter );

void measurement_free(			MEASUREMENT *m );

/* This function does strdup() for the memory. */
/* ------------------------------------------- */
MEASUREMENT *measurement_strdup_new(
					char *station_name,
					char *datatype,
					char *date,
					char *time,
					char *value_string );

void measurement_set_comma_delimited_record(
					MEASUREMENT_STRUCTURE *m, 
					char *comma_delimited_record,
					char *argv_0 );

void measurement_insert( 		MEASUREMENT_STRUCTURE *m,
					int really_yn,
					FILE *html_table_pipe );

/*
double measurement_get_value_from_db(	int *record_exists,
					int *db_null_value,
					char *application_name,
					char *station_name,
					char *datatype,
					char *date,
					char *time );
*/

void measurement_output_insert_pipe(	FILE *insert_pipe,
					char *station_name,
					char *datatype,
					char *date,
					char *time,
					double value,
					boolean null_value );

FILE *measurement_open_insert_pipe(	char *application_name,
					int delete_measurements_day,
					int really_yn );

FILE *measurement_open_html_table_pipe(	void );

double measurement_get_value(		boolean *null_value,
					char *value_string );

void measurement_set_load_process( 	MEASUREMENT *m,
					char *load_process,
					int really_yn );

void measurement_set_argv_0(		MEASUREMENT_STRUCTURE *m,
					char *argv_0 );

int measurement_structure_fetch(	MEASUREMENT_STRUCTURE *m,
					FILE *input_pipe );

FILE *measurement_open_delete_pipe(	char *application_name );

char *measurement_display(		MEASUREMENT *m );

void measurement_delete(		FILE *delete_pipe,
					MEASUREMENT *m );

void measurement_open_input_process( 	MEASUREMENT_STRUCTURE *m,
					char *load_process,
					int really_yn );

DICTIONARY *measurement_get_date_time_frequency_dictionary(
					char *application_name,
					char *station_name,
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
					char *station_name,
					char *datatype );

void measurement_update(		char *application_name,
					char *station_name,
					char *datatype,
					char *date,
					char *time,
					double value );

MEASUREMENT *measurement_calloc(	void );

LIST *measurement_fetch_list(		FILE *input_pipe,
					char delimiter );

MEASUREMENT *measurement_parse(		char *buffer,
					char delimiter );

#endif
