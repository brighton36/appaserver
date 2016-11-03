/* ---------------------------------------------------	*/
/* src_benthic/environment.h				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *anchor_date;
	char *anchor_time;
	char *actual_latitude;
	char *actual_longitude;
	char *datatype;
	double value;
} ENVIRONMENT;

/* Prototypes */
/* ---------- */
ENVIRONMENT *environment_new(
				char *anchor_date,
				char *anchor_time,
				char *actual_latitude,
				char *actual_longitude );


#endif
