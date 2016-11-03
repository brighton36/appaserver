/* measurement_validation.h */
/* ------------------------ */

#ifndef MEASUREMENT_VALIDATION_H
#define MEASUREMENT_VALIDATION_H

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define MEASUREMENT_VALIDATION_VALIDATION_PROCESS	"visual_inspection"
#define MEASUREMENT_VALIDATION_UNVALIDATION_PROCESS	"unvalidated"

/* Prototypes */
/* ---------- */
void measurement_validation_insert_into_manual_validation_event(
						char *application_name,
						char *login_name,
						char *station,
						char *datatype,
						char *begin_measurement_date,
						char *end_measurement_date,
						char *validation_date,
						char *validation_time,
						char *validation_process );

int measurement_validation_update_measurement(
					char *application_name,
					char *login_name,
					char *station,
					char *datatype,
					char *validation_date,
					char *begin_measurement_date,
					char *end_measurement_date );

#endif
