/* src_everglades/shef_alias.h */
/* --------------------------- */

#ifndef SHEF_ALIAS_H
#define SHEF_ALIAS_H

#include "dictionary.h"

typedef struct
{
	DICTIONARY *shef_alias_dictionary;
} SHEF_ALIAS;

/* Prototypes */
/* ---------- */
SHEF_ALIAS *new_shef_alias( 
			char *entity );

char *shef_alias_get_shef(
			DICTIONARY *shef_alias_dictionary,
			char *shef_alias_string );
DICTIONARY *shef_alias_get_dictionary(
			char *entity );
#endif
