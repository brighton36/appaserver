/* src_everglades/station_alias.h */
/* ------------------------------ */

#ifndef STATION_ALIAS_H
#define STATION_ALIAS_H

#include "dictionary.h"

typedef struct
{
	DICTIONARY *station_alias_dictionary;
} STATION_ALIAS;

/* Prototypes */
/* ---------- */
STATION_ALIAS *new_station_alias( char *application );

char *station_alias_get_station(
			DICTIONARY *station_alias_dictionary,
			char *station_alias_string );
DICTIONARY *station_alias_get_dictionary( char *application );
#endif
