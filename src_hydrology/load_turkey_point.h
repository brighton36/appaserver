/* -----------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/load_turkey_point.h		*/
/* -----------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------	*/

#ifndef LOAD_TURKEY_POINT_H
#define LOAD_TURKEY_POINT_H

/* Includes */
/* -------- */
#include "measurement.h"

/* Constants */
/* --------- */
#define STDERR_COUNT				1000

#define INSERT_MEASUREMENT 			"station,datatype,measurement_date,measurement_time,measurement_value"

/* Prototypes */
/* ---------- */
int load_turkey_point_file(	char *application_name,
				char *station,
				char *input_filename,
				boolean execute );

void remove_error_file(		char *error_filename );

boolean extract_static_attributes(
				char **error_message,
				char *measurement_date,
				char *measurement_time,
				char *application_name,
				char *input_string,
				LIST *header_column_datatype_list );

void delete_measurement(
				char *application_name,
				char *input_filename,
				LIST *header_column_datatype_list );

MEASUREMENT *extract_measurement(
				char *input_string,
				LIST *header_column_datatype_list );

#endif
