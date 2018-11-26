/* alt_station.h					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef ALT_STATION_H
#define ALT_STATION_H

#include "list.h"

typedef struct 
{
	char *model;
	char *alternative;
	char *station_name;
	double ground_surface_elevation;
	boolean station_elevation_null;
} ALT_STATION;

/* Operations */
/* ---------- */
ALT_STATION *alt_station_new(	char *model,
				char *alternative,
				char *station_name );
double alt_station_get_ground_surface_elevation(
				boolean *station_not_found,
				char **station_elevation_null,
				char *application_name,
				char *model,
				char *alternative,
				char *station_name );
LIST *alt_station_get_global_alt_station_list(
				char *application_name );
ALT_STATION *alt_station_seek(	char *model,
				char *alternative,
				char *station_name,
				LIST *alt_station_list );
#endif
