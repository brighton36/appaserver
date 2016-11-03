/* or_sequence.h */
/* ------------- */

#ifndef OR_SEQUENCE_H
#define OR_SEQUENCE_H

/*
This library supports the generation of a sequence of "or" statements
used in sql statement where clauses.
*/

#include <stdio.h>
#include "list.h"
#include "boolean.h"

typedef struct 
{
	LIST *attribute_name_list;
	LIST *data_list_list;
} OR_SEQUENCE;


/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
OR_SEQUENCE *or_sequence_new(		LIST *attribute_name_list );
int or_sequence_set_data_list_string(	LIST *data_list_list,
					char *data_list_string );
int or_sequence_set_data_list(		LIST *data_list_list,
					LIST *data_list );
char *or_sequence_get_where_clause(	LIST *attribute_name_list,
					LIST *data_list_list,
					boolean with_and_prefix );
#endif
