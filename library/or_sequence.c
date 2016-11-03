/* or_sequence.c					*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/
/*
This library supports the generation of a sequence of "or" statements
used in sql statement where clauses.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "or_sequence.h"

OR_SEQUENCE *or_sequence_new( LIST *attribute_name_list )
{
	OR_SEQUENCE *or_sequence;

	or_sequence =
		(OR_SEQUENCE *)
			calloc( 1, sizeof( OR_SEQUENCE ) );
	or_sequence->attribute_name_list = attribute_name_list;
	or_sequence->data_list_list = list_new();
	return or_sequence;

} /* or_sequence_new() */

int or_sequence_set_data_list_string(	LIST *data_list_list,
					char *data_list_string )
{
	LIST *temp_list;
	LIST *data_list;
	int length;
	char *data;
	int i;

	temp_list = list_string2list( data_list_string, ',' );
	length = list_length( temp_list );

	if ( !length ) return 0;

	if ( !list_length( data_list_list ) )
	{
		for ( i = 0; i < length; i++ )
		{
			data_list = list_new();
			list_append_pointer( data_list_list, data_list );
		}
	}

	list_rewind( temp_list );
	list_rewind( data_list_list );

	do {
		data = list_get_pointer( temp_list );
		data_list = list_get_pointer( data_list_list );
		list_append_pointer( data_list, data );
		list_next( data_list_list );
	} while( list_next( temp_list ) );
	list_free_container( temp_list );
	return length;
} /* or_sequence_set_data_list_string() */

int or_sequence_set_data_list(	LIST *data_list_list,
				LIST *data_list )
{
	char *data_list_string;

	data_list_string = list_display_delimited( data_list, ',' );

	return or_sequence_set_data_list_string(
			data_list_list,
			data_list_string );

} /* or_sequence_set_data_list() */

char *or_sequence_get_where_clause(	LIST *attribute_name_list,
					LIST *data_list_list,
					boolean with_and_prefix )
{
	char *attribute_name;
	LIST *data_list;
	char *data;
	char where_clause[ 65536 ];
	char *where_ptr = where_clause;
	boolean first_time_and;
	boolean first_time_or;

	if ( !list_rewind( data_list_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty data_list_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	*where_ptr = '\0';

	if ( with_and_prefix )
	{
		where_ptr += sprintf( where_ptr, "and ( " );
	}

	first_time_or = 1;
	do {
		if ( first_time_or )
		{
			first_time_or = 0;
		}
		else
		{
			where_ptr += sprintf( where_ptr, " or " );
		}

		data_list = list_get_pointer( data_list_list );

		list_rewind( attribute_name_list );
		list_rewind( data_list );

		where_ptr += sprintf( where_ptr, "(" );
		first_time_and = 1;
		do {
			attribute_name =
				list_get_pointer( attribute_name_list );
			data = list_get_pointer( data_list );

			if ( first_time_and )
			{
				first_time_and = 0;
			}
			else
			{
				where_ptr +=
					sprintf( where_ptr,
						 " and " );
			}

			where_ptr +=
				sprintf( where_ptr,
					 "%s = '%s'",
					 attribute_name,
					 data );
			list_next( data_list );
		} while( list_next( attribute_name_list ) );
		where_ptr += sprintf( where_ptr, ")" );

	} while( list_next( data_list_list ) );
	if ( with_and_prefix )
	{
		where_ptr += sprintf( where_ptr, " )" );
	}

	return strdup( where_clause );
} /* or_sequence_get_where_clause() */

