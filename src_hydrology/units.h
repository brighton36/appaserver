/* $APPASERVER_HOME/src_hydrology/units.h */
/* -------------------------------------- */

#ifndef UNITS_H
#define UNITS_H

#include "list.h"

typedef struct
{
	char *units_converted;
	double multiply_by;
} UNITS_CONVERTED;

typedef struct
{
	char *units_alias;
	char *units_name;
} UNITS_ALIAS;

typedef struct
{
	char *units_name;
	LIST *units_alias_list;
	LIST *units_converted_list;
} UNITS;

/* Operations */
/* ---------- */
UNITS_CONVERTED *units_converted_new(
				void );

UNITS_ALIAS *units_alias_new(
				void );

UNITS *units_new(		char *units_name );

LIST *units_get_units_alias_list(
				char *application_name,
				char *units_name );

LIST *units_fetch_units_alias_list(
				char *application_name );

LIST *units_fetch_units_list(
				char *application_name );

#endif
