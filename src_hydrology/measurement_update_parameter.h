/* measurement_update_parameter.h					*/
/* -------------------------------------------------------------------- */
/* This is the hydrology measurement_update_parameter ADT.		*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef MEASUREMENT_UPDATE_PARAMETER_H
#define MEASUREMENT_UPDATE_PARAMETER_H

#include "list.h"
#include "dictionary.h"

/* Constants */
/* --------- */

/* Data Structures */
/* --------------- */
typedef struct
{
	char *application_name;
	char *station;
	char *datatype;
	char *measurement_update_method;
	char *login_name;
	DICTIONARY *parameter_dictionary;
	char *now_date;
	char *now_time;
	char *notes;
} MEASUREMENT_UPDATE_PARAMETER;

/* Operations */
/* ---------- */
MEASUREMENT_UPDATE_PARAMETER *measurement_update_parameter_new(
				char *application_name,
				char *station,
				char *datatype,
				char *measurement_update_method,
				char *login_name,
				DICTIONARY *parameter_dictionary,
				char *notes );

void measurement_update_parameter_save(	MEASUREMENT_UPDATE_PARAMETER *
					measurement_update_parameter );

void measurement_update_parameter_save_event(
				char *application_name,
				char *station,
				char *datatype,
				char *measurement_update_method,
				char *login_name,
				char *now_date,
				char *now_time,
				char *notes );

void measurement_update_parameter_save_parameter(
				char *application_name,
				char *station,
				char *datatype,
				char *measurement_update_method,
				char *login_name,
				DICTIONARY *parameter_dictionary,
				char *now_date,
				char *now_time );

#endif
