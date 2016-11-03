/* or_sequence_dvr.c		          */
/* -------------------------------------- */
/* Test drive program for OR_SEQUENCE ADT */
/* -------------------------------------- */
/* Tim Riley				  */
/* -------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "or_sequence.h"


int main( void )
{
	OR_SEQUENCE *or_sequence;
	LIST *attribute_name_list;

	attribute_name_list = list_new();

	list_append_pointer( attribute_name_list, "location_area" );
	list_append_pointer( attribute_name_list, "location_zone" );

	or_sequence = or_sequence_new( attribute_name_list );

	/* Set location area */
	/* ----------------- */
	or_sequence_set_data_list_string(
			or_sequence->data_list_list,
			"1,1,2" );

	/* Set location zone */
	/* ----------------- */
	or_sequence_set_data_list_string(
			or_sequence->data_list_list,
			"E,Entire,E" );

	printf( "where 1 = 1 %s\n",
		or_sequence_get_where_clause(
				or_sequence->attribute_name_list,
				or_sequence->data_list_list,
				1 /* with_and_prefix */ ) );

	return 0;
} /* main() */

