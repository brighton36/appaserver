/* $APPASERVER_HOME/library/dictionary.c		*/
/* ==================================================== */
/* This contains the code for the dictionary ADT.	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "list.h"
#include "list_usage.h"
#include "piece.h"
#include "dictionary.h"
#include "attribute.h"
#include "timlib.h"
#include "date_convert.h"
#include "query.h"
#include "date.h"

/* ------------------------------------------------------------ */
/* Sample:							*/
/* From:  "station^datatype_1=BA^stage"				*/
/* To:    "station_1=BA and datatype_1=stage"			*/
/* ------------------------------------------------------------ */
void dictionary_parse_multi_attribute_keys(
				DICTIONARY *dictionary, 
				char key_delimiter,
				char data_delimiter,
				char *prefix,
				boolean dont_include_relational_operators )
{
	LIST *multi_attribute_key_list;
	LIST *attribute_key_list;
	LIST *attribute_data_list;
	char *full_key, *full_data;
	char *key, *data;
	char key_without_index[ 1024 ];
	char key_with_index[ 1024 ];
	char key_delimiter_string[ 2 ];
	char data_delimiter_string[ 2 ];
	int index;
	int str_len = 0;

	if ( prefix && *prefix ) str_len = strlen( prefix );

	/* Set the delimiter strings */
	/* ------------------------- */
	*key_delimiter_string = key_delimiter;
	*(key_delimiter_string + 1) = '\0';

	*data_delimiter_string = data_delimiter;
	*(data_delimiter_string + 1) = '\0';

	multi_attribute_key_list = dictionary_get_key_list( dictionary );

	if ( !list_rewind( multi_attribute_key_list ) ) return;

	do {
		full_key = list_get_pointer( multi_attribute_key_list );

		if ( prefix
		&&   *prefix
		&&   strncmp( full_key, prefix, str_len ) != 0 )
		{
			continue;
		}

		if ( dont_include_relational_operators
		&&   timlib_strncmp(
				full_key + str_len,
				QUERY_RELATION_OPERATOR_STARTING_LABEL ) == 0 )
		{
			continue;
		}

		if ( instr( key_delimiter_string, full_key, 1 ) != -1 )
		{
			index = get_index_from_string( full_key );
			trim_index( key_without_index, full_key );

			attribute_key_list = 
				list_string2list(
					key_without_index,
					key_delimiter);

			full_data =
				dictionary_get_data(
					dictionary,
					full_key );

			attribute_data_list = 
				list_string2list(
					full_data,
					data_delimiter );

			list_go_tail( attribute_key_list );
			list_go_tail( attribute_data_list );

			do {
				key = list_get_pointer( attribute_key_list );

				if ( prefix
				&&   *prefix
				&&   strncmp( key, prefix, str_len ) != 0 )
				{
					sprintf( key_with_index, 
					 	 "%s%s_%d",
					 	 prefix, key, index );
				}
				else
				{
					sprintf( key_with_index, 
					 	 "%s_%d",
					 	 key, index );
				}

				if ( dictionary_exists_key(
						dictionary,
						key_with_index ) )
				{
					list_previous( attribute_data_list );
					continue;
				}

				data = list_get_pointer( attribute_data_list );

				dictionary_set_pointer( 
						dictionary, 
						strdup( key_with_index ),
						data );

				list_previous( attribute_data_list );

			} while( list_previous( attribute_key_list ) );
		}

	} while( list_next( multi_attribute_key_list ) );

} /* dictionary_parse_multi_attribute_keys() */

void dictionary_parse_multi_attribute_relation_operator_keys(
				DICTIONARY *dictionary, 
				char multi_attribute_drop_down_delimiter )
{
	LIST *multi_attribute_key_list;
	LIST *attribute_key_list;
	char *full_key;
	char *key, *data;
	char full_parse_key[ 1024 ];
	char delimiter_string[ 2 ];

	/* Set the delimiter string */
	/* ------------------------ */
	*delimiter_string = multi_attribute_drop_down_delimiter;
	*(delimiter_string + 1) = '\0';

	multi_attribute_key_list = dictionary_get_key_list( dictionary );

	if ( !list_rewind( multi_attribute_key_list ) ) return;

	do {
		full_key = list_get_pointer( multi_attribute_key_list );

		/* Only parse relational operators */
		/* ------------------------------- */
		if ( timlib_strncmp(
			full_key,
			QUERY_RELATION_OPERATOR_STARTING_LABEL ) != 0 )
		{
			continue;
		}

		if ( instr( delimiter_string, full_key, 1 ) == -1 )
			continue;

		attribute_key_list = 
			list_string2list(
				full_key,
				multi_attribute_drop_down_delimiter );

		data = dictionary_get_data( dictionary, full_key );

		list_rewind( attribute_key_list );

		do {
			key = list_get_pointer( attribute_key_list );

			if ( timlib_strncmp(
				key,
				QUERY_RELATION_OPERATOR_STARTING_LABEL ) == 0 )
			{
				strcpy( full_parse_key, key );
			}
			else
			{
				sprintf(full_parse_key,
					"%s%s",
					QUERY_RELATION_OPERATOR_STARTING_LABEL,
					key );
			}

			if ( dictionary_exists_key(
					dictionary,
					full_parse_key ) )
			{
				continue;
			}

			dictionary_set_pointer( 
					dictionary, 
					strdup( full_parse_key ),
					data );

		} while( list_next( attribute_key_list ) );

	} while( list_next( multi_attribute_key_list ) );

} /* dictionary_parse_multi_attribute_relation_operator_keys() */

void dictionary_delete_keys_with_prefix(
					DICTIONARY *dictionary, 
					char *prefix )
{
	LIST *attribute_key_list;
	char *key;
	int str_len;

	attribute_key_list = dictionary_get_key_list( dictionary );
	if ( !( attribute_key_list
	&&      list_reset( attribute_key_list ) ) )
	{
		return;
	}
	
	str_len = strlen( prefix );

	do {
		key = list_get_pointer( attribute_key_list );

		if ( strncmp( key, prefix, str_len ) == 0 )
		{
			dictionary_delete_key( dictionary, key );
			continue;
		}

	} while( list_next( attribute_key_list ) );
} /* dictionary_delete_keys_with_prefix() */

int dictionary_key_exists( 	DICTIONARY *dictionary,
				char *search_key )
{
	return dictionary_exists_key( dictionary, search_key );
}

boolean dictionary_exists_key( 	DICTIONARY *dictionary,
				char *search_key )
{
	char *data;
	data = dictionary_get( dictionary, search_key );
	return (int)data;
}

boolean dictionary_populated_key_exists_index_one(
					DICTIONARY *dictionary,
					char *key )
{
	char *data;
	char dictionary_key[ 1024 ];

	sprintf( dictionary_key,
	 	"%s_1",
	 	key );
	data = dictionary_get_pointer( dictionary, dictionary_key );

	if ( !data || !*data )
		return 0;
	else
		return 1;

} /* dictionary_populated_key_exists_index_one() */

boolean dictionary_populated_key_exists_index_zero(
					DICTIONARY *dictionary,
					char *key )
{
	char *data;
	char dictionary_key[ 1024 ];

	sprintf( dictionary_key,
	 	"%s_0",
	 	key );
	data = dictionary_get_pointer( dictionary, dictionary_key );

	if ( !data || !*data )
		return 0;
	else
		return 1;

} /* dictionary_populated_key_exists_index_zero() */

boolean dictionary_non_populated_key_exists_index_zero(
					DICTIONARY *dictionary,
					char *key )
{
	char *data;
	char dictionary_key[ 1024 ];

	sprintf( dictionary_key,
	 	"%s_0",
	 	key );

	data = dictionary_get_pointer( dictionary, dictionary_key );

	if ( !data || !*data )
		return 1;
	else
		return 0;

} /* dictionary_non_populated_key_exists_index_zero() */

boolean dictionary_exists_key_index( 	DICTIONARY *dictionary,
					char *search_key,
					int row )
{
	char *data;
	char dictionary_key[ 1024 ];

	sprintf( dictionary_key, 
	 	"%s_%d",
	 	search_key, row );
	data = dictionary_get( dictionary, dictionary_key );

	if ( !data && row == 0 )
	{
		sprintf( dictionary_key, 
		 	"%s",
		 	search_key );
		data = dictionary_get( dictionary, dictionary_key );
	}

	return (boolean)data;

} /* dictionary_exists_key_index() */

/* --------------------------------------------------------------- */
/* Returns: 	1 if the data comes from the index 	           */
/*		0 if the data comes from index = 0 	           */
/*	       -1 if the data is not found or didn't change        */
/* --------------------------------------------------------------- */
int dictionary_get_index_data_if_changed(
					char **destination,
					DICTIONARY *dictionary_1,
					DICTIONARY *dictionary_2,
					char *key, 
					int index )
{
	char *data_1, *data_2;
	int results;

	results = dictionary_get_index_data(	&data_1,
						dictionary_1,
						key,
						index );

	if ( results == -1 )
	{
		*destination = "";
		return -1;
	}

	/* Return here if no second dictionary to compare with */
	/* --------------------------------------------------- */
	if ( !dictionary_2 )
	{
		*destination = data_1;
		return results;
	}

	/* ------------------------------------------------------- */
	/* If not found in the second dictionary, then return true */
	/* ------------------------------------------------------- */
	if ( dictionary_get_index_data(	&data_2,
					dictionary_2,
					key,
					index ) == -1 )
	{
		data_2 = "";
	}

	*destination = data_1;

	if ( strcmp( data_1, data_2 ) == 0 ) results = -1;

	return results;
} /* dictionary_get_index_data_if_changed() */


int dictionary_get_index_data_if_populated(
					char **destination,
					DICTIONARY *dictionary,
					char *key, 
					int index )
{
	char *data;
	int results;

	results = dictionary_get_index_data(	&data,
						dictionary,
						key,
						index );

	if ( results == -1 )
	{
		*destination = "";
		return 0;
	}

	*destination = data;
	return ( *data != '\0' );
} /* dictionary_get_index_data_if_populated() */

LIST *dictionary_get_index_data_list( 	DICTIONARY *dictionary,
					char *key  )
{
	LIST *return_list = list_new_list();
	char *data;
	int index;
	int results;

	for( index = 1 ;; index++ )
	{
		results =
			dictionary_get_index_data(
					&data,
					dictionary,
					key,
					index );

		if ( results <= 0 ) return return_list;

		list_append_pointer( return_list, data );
	}
	return return_list;
} /* dictionary_get_index_data_list() */

int dictionary_get_key_highest_index( DICTIONARY *d )
{
	return get_dictionary_key_highest_index( d );
}

int dictionary_attribute_name_list_get_highest_index(
				DICTIONARY *d,
				LIST *attribute_name_list,
				char *prepend_folder_name )
{
	LIST *key_list;
	char *key;
	char *attribute_name;
	char folder_attribute_name[ 128 ];
	int highest_index = -1;
	int index;

	if ( !list_length( attribute_name_list ) ) return -1;

	key_list = dictionary_get_key_list( d );

	if ( !list_rewind( key_list ) ) return -1;

	do {
		key = list_get_pointer( key_list );

		list_rewind( attribute_name_list );

		do {
			attribute_name = list_get( attribute_name_list );

			if ( timlib_strncmp( key, attribute_name ) == 0 )
			{
				index = get_index( key );

				if ( highest_index < index )
					highest_index = index;
			}

			if ( prepend_folder_name && *prepend_folder_name )
			{
				sprintf(folder_attribute_name,
					"%s.%s",
					prepend_folder_name,
					attribute_name );

				if ( timlib_strncmp(
						key,
						folder_attribute_name ) == 0 )
				{
					index = get_index( key );

					if ( highest_index < index )
						highest_index = index;
				}

			}

		} while( list_next( attribute_name_list ) );

	} while( list_next( key_list ) );

	list_free_container( key_list );

	return highest_index;

} /* dictionary_attribute_name_list_get_highest_index() */

int dictionary_get_highest_index( DICTIONARY *d )
{
	return get_dictionary_key_highest_index( d );
}

int get_dictionary_key_highest_index( DICTIONARY *d )
{
	char *key;
	int highest_index = 0;
	int index;
	LIST *key_list;

	key_list = get_dictionary_key_list( d );

	if ( list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );
			index = get_index( key );
			if ( highest_index < index ) highest_index = index;
		} while( list_next( key_list ) );
	}
	return highest_index;
} /* get_dictionary_key_highest_index() */

LIST *dictionary_get_key_list( DICTIONARY *d )
{
	return get_dictionary_key_list( d );
}

char *dictionary_display_delimited( DICTIONARY *d, char delimiter )
{
	return dictionary_display_delimiter( d, delimiter );
}

char *dictionary_index_zero_display_delimited( DICTIONARY *d, char delimiter )
{
	char buffer[ 65536 ];

	if ( !d ) return "NULL";

	return strdup( hash_table_index_zero_display_delimiter(
							buffer, 
							d->hash_table,
							delimiter ) );
}

char *dictionary_display_delimiter( DICTIONARY *d, char delimiter )
{
	/* char buffer[ 65536 ]; */
	char buffer[ 131072 ];

	if ( !d ) return "NULL";

	return strdup( hash_table_display_delimiter( 	buffer, 
							d->hash_table,
							delimiter ) );
}

char *dictionary_list_display( LIST *dictionary_list )
{
	DICTIONARY *dictionary;
	char buffer[ 131072 ];
	char *dictionary_buffer;
	char *buf_ptr = buffer;

	*buf_ptr = '\0';

	if ( !list_rewind( dictionary_list ) ) return "";

	do {
		dictionary = list_get( dictionary_list );
		dictionary_buffer = dictionary_display( dictionary );
		buf_ptr += sprintf( buf_ptr, "%s\n", dictionary_buffer );
		free( dictionary_buffer );
	} while( list_next ( dictionary_list ) );

	return strdup( buffer ); 
}
	

char *dictionary_display( DICTIONARY *d )
{
	return dictionary_display_delimiter( d, ',' );
}

/* Sample: "datatype=salinity&station=BA&grade_1=a" */
/* ------------------------------------------------ */
DICTIONARY *dictionary_string2dictionary( char *dictionary_string )
{
	DICTIONARY *d;
        char attribute[ 1024 ];
	char pair_string[ 65536 ];
        char data[ 65536 ];
	int i;
	char delimiter;
	char pair_delimiter;

	if ( character_exists( dictionary_string, '=' ) )
	{
		delimiter = '=';
	}
	else
	if ( character_exists(	dictionary_string,
				DICTIONARY_ALTERNATIVE_DELIMITER ) )
	{
		delimiter = DICTIONARY_ALTERNATIVE_DELIMITER;
	}
	else
	{
		return (DICTIONARY *)0;
	}

	if ( character_exists( dictionary_string, '&' ) )
		pair_delimiter = '&';
	else
	if ( character_exists( dictionary_string, '@' ) )
		pair_delimiter = '@';
	else
		pair_delimiter = ',';

	d = dictionary_large_new();

 	for(	i = 0;
		piece( pair_string, pair_delimiter, dictionary_string, i );
		i++ )
        {
                piece( attribute, delimiter, pair_string, 0 );

                if ( piece( data, delimiter, pair_string, 1 ) )
                {
                        /* Don't save if empty or "select" or <CR> */
                        /* --------------------------------------- */
			if ( !*data
			||   strcmp( data, "select" ) == 0
			||   strcmp( data, "%0D%0A" ) == 0 )
			{
				continue;
			}

			dictionary_set_pointer(
					d, 
					strdup( attribute ),
					strdup( data ) );
		}
	}

	return d;

} /* dictionary_string2dictionary() */

DICTIONARY *dictionary_index_string2dictionary( char *dictionary_string )
{
	return dictionary_string2dictionary( dictionary_string );

} /* dictionary_index_string2dictionary() */

void dictionary_add( DICTIONARY *d, char *key, char *data, int size )
{
	dictionary_set( d, key, data, size );
}

int dictionary_all_null_data( DICTIONARY *d )
{
	return hash_table_all_null_data( d->hash_table );
}

DICTIONARY *dictionary_small_new()
{
	return dictionary_small_dictionary_new();
}

DICTIONARY *dictionary_small_dictionary_new()
{
	DICTIONARY *dictionary = 
		(DICTIONARY *)calloc( 1, sizeof( DICTIONARY ) );

	if ( !dictionary )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	dictionary->hash_table = hash_table_init( hash_table_small );

	if ( !dictionary->hash_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary;
} /* dictionary_small_dictionary_new() */

DICTIONARY *dictionary_calloc( void )
{
	DICTIONARY *dictionary = 
		(DICTIONARY *)calloc( 1, sizeof( DICTIONARY ) );

	if ( !dictionary )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary;
} /* dictionary_calloc() */

DICTIONARY *dictionary_new_huge_dictionary()
{
	DICTIONARY *dictionary = dictionary_calloc();

	dictionary->hash_table = hash_table_init( hash_table_huge );

	if ( !dictionary->hash_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary;
} /* dictionary_new_huge_dictionary() */

DICTIONARY *dictionary_new_super_dictionary()
{
	DICTIONARY *dictionary = dictionary_calloc();

	dictionary->hash_table = hash_table_init( hash_table_super );

	if ( !dictionary->hash_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary;
} /* dictionary_new_super_dictionary() */

DICTIONARY *dictionary_new_duper_dictionary()
{
	DICTIONARY *dictionary = dictionary_calloc();

	dictionary->hash_table = hash_table_init( hash_table_duper );

	if ( !dictionary->hash_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary;
} /* dictionary_new_duper_dictionary() */

DICTIONARY *dictionary_new_large_dictionary()
{
	return dictionary_large_dictionary_new();
}

DICTIONARY *dictionary_large_new()
{
	return dictionary_large_dictionary_new();
}

DICTIONARY *dictionary_large_dictionary_new()
{
	DICTIONARY *dictionary = 
		(DICTIONARY *)calloc( 1, sizeof( DICTIONARY ) );

	if ( !dictionary )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	dictionary->hash_table = hash_table_init( hash_table_large );

	if ( !dictionary->hash_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary;
} /* dictionary_large_dictionary_new() */

DICTIONARY *dictionary_huge_new()
{
	DICTIONARY *dictionary = 
		(DICTIONARY *)calloc( 1, sizeof( DICTIONARY ) );

	if ( !dictionary )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	dictionary->hash_table = hash_table_init( hash_table_huge );

	if ( !dictionary->hash_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary;
}

DICTIONARY *dictionary_medium_new()
{
	return dictionary_medium_dictionary_new();
}

DICTIONARY *dictionary_new_medium_dictionary()
{
	return dictionary_medium_dictionary_new();
}

DICTIONARY *dictionary_medium_dictionary_new()
{
	DICTIONARY *dictionary = 
		(DICTIONARY *)calloc( 1, sizeof( DICTIONARY ) );

	if ( !dictionary )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	dictionary->hash_table = hash_table_init( hash_table_medium );

	if ( !dictionary->hash_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return dictionary;
} /* dictionary_medium_dictionary_new() */

DICTIONARY *dictionary_new()
{
	return dictionary_medium_new();
}


int dictionary_len( DICTIONARY *d )
{
	return hash_table_len( d->hash_table );
}

int dictionary_length( DICTIONARY *d )
{
	if ( !d )
		return 0;
	else
		return hash_table_len( d->hash_table );
}

void dictionary_add_string(	DICTIONARY *d,
				char *key,
				char *data_string )
{
	dictionary_set_string( d, key, data_string );
}

void dictionary_set_index_zero(		DICTIONARY *d,
					char *key, 
					char *data_string )
{
	return dictionary_set_string_index_key(
			d, key, data_string, 0 );
}

void dictionary_set_string_index_key(	DICTIONARY *d,
					char *key, 
					char *data_string,
					int index )
{
	char full_key[ 1024 ];
	sprintf( full_key, "%s_%d", key, index );
	dictionary_set_pointer( d, strdup( full_key ), data_string );
}

void dictionary_set_string(	DICTIONARY *d, 
				char *key, 
				char *data_string )
{
	dictionary_set( d, key, data_string, strlen( data_string ) + 1 );
}

void dictionary_set_pointer(	DICTIONARY *d,
				char *key,
				char *data )
{
	if ( !d ) return;
	if ( !d->hash_table ) return;
	if ( !key ) return;
	if ( !data ) return;

	hash_table_set( d->hash_table, key, data );
}

void dictionary_set(	DICTIONARY *d, 
			char *key, 
			void *other_data, 
			int sizeof_other_data )
{
	char *memory = (char *)calloc( 1, sizeof_other_data );

	if ( !memory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	memcpy( memory, other_data, sizeof_other_data );
	hash_table_set( d->hash_table, strdup( key ), memory );

} /* dictionary_set() */

void dictionary_add_pointer(	DICTIONARY *d, 
				char *key, 
				void *data )
{
	if ( d && key && data ) hash_table_set( d->hash_table, key, data );

} /* dictionary_add_pointer() */

char *dictionary_get_pointer( DICTIONARY *d, char *key )
{
	return dictionary_get( d, key );
}

char *dictionary_get_data( DICTIONARY *d, char *key )
{
	return dictionary_get( d, key );
}

char *dictionary_get_string( DICTIONARY *d, char *key )
{
	return dictionary_get( d, key );
}

void *dictionary_seek( DICTIONARY *d, char *key )
{
	return dictionary_get( d, key );
}

void *dictionary_get( DICTIONARY *d, char *key )
{
	int duplicate_indicator;
	char *memory;

	if ( !d ) return (void *)0;

	memory = (char *)hash_table_retrieve_other_data( 
					d->hash_table, 
					key,
					&duplicate_indicator );
	if ( !memory )
		return (void *)0;
	else
		return (void *)memory;
} /* dictionary_get() */

void dictionary_free_data( DICTIONARY *d, LIST *key_list )
{
	char *key;
	char *data;

	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );
			if ( ( data = dictionary_get_pointer(
					d, key ) ) )
			{
				free( data );
			}
		} while( list_next( key_list ) );
	}
} /* dictionary_free_data() */

void dictionary_free( DICTIONARY *d )
{
	char *key;
	LIST *key_list;

	key_list = get_dictionary_key_list( d );

	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );
			free( dictionary_get_pointer(
					d,
					key ) );
			/* core dumps :-( free( key ); */
		} while( list_next( key_list ) );
	}
	/* core dumps :-( list_free( key_list ); */
	/* core dumps :-( hash_table_free( d->hash_table ); */
	free( d->hash_table );
	free( d );
} /* dictionary_free() */

LIST *dictionary_get_ordered_key_list( DICTIONARY *d )
{
	return hash_table_get_ordered_key_list( d->hash_table );
}

LIST *get_dictionary_key_list( DICTIONARY *d )
{
	if ( !d ) return (LIST *)0;
	return get_hashtbl_key_list( d->hash_table );
}

/* ----------------------------------------------------------- */
/* Returns: 	1 if the data comes from the index	       */
/*		0 if the data comes from index = 0 or no index */
/*	       -1 if the data is not found	   	       */
/* ----------------------------------------------------------- */
int dictionary_get_index_data_multi(	char *destination,
					DICTIONARY *dictionary,
					char *key_multi,
					int index,
					char delimiter )
{
	char key_single[ 1024 ];
	char *data;
	int i, return_value = 0;
	int first_time = 1;

	*destination = '\0';

	for( i = 0; piece( key_single, delimiter, key_multi, i ); i++ )
	{
		return_value = 
			dictionary_get_index_data(	&data,
							dictionary,
							key_single,
							index );

		if ( return_value == -1 ) return -1;

		if ( first_time )
		{
			destination += sprintf( 	destination,
							"%s", 
							data );
			first_time = 0;
		}
		else
		{
			destination += sprintf( 	destination,
							"%c%s", 
							delimiter, 
							data );
		}
	}

	return return_value;

} /* dictionary_get_index_data_multi() */

char *dictionary_fetch_index_zero(	DICTIONARY *dictionary,
					char *key )
{
	return dictionary_get_index_zero( dictionary, key );
}

char *dictionary_get_index_zero_data(	DICTIONARY *dictionary,
					char *key )
{
	return dictionary_get_index_zero( dictionary, key );
}

char *dictionary_get_index_zero( DICTIONARY *dictionary, char *key )
{
	int results;
	char *data;

	if ( !dictionary ) return (char *)0;

	results = dictionary_get_index_data(	
				&data,
				dictionary,
				key,
				0 );

	if ( results == -1 )
		return (char *)0;
	else
		return data;

} /* dictionary_get_index_zero() */

/* ----------------------------------------------------------- */
/* Returns: 	1 if the data comes from the index	       */
/*	        0 if the data is not found	   	       */
/* ----------------------------------------------------------- */
boolean dictionary_get_index_data_strict_index(
				char **destination,
				DICTIONARY *dictionary,
				char *key,
				int index )
{
	char dictionary_key[ 1024 ];
	char *data;

	*destination = "";
	if ( !key ) return 0;

	sprintf( dictionary_key, 
	 	"%s_%d",
	 	key, index );

	data = dictionary_get( dictionary, dictionary_key );

	if ( !data ) return 0;
	*destination  = data;
	return 1;
} /* dictionary_get_index_data_strict_index() */

/* ----------------------------------------------------------- */
/* Returns: 	1 if the data comes from the index	       */
/*		0 if the data comes from index = 0 or no index */
/*	       -1 if the data is not found	   	       */
/* ----------------------------------------------------------- */
int dictionary_get_index_data(	char **destination,
				DICTIONARY *dictionary,
				char *key,
				int index )
{
	char dictionary_key[ 1024 ];
	int return_value;
	char *data;

	if ( !key ) return -1;
	if ( !dictionary ) return -1;

	/* Try to get the data with the index */
	/* ---------------------------------- */
	sprintf( dictionary_key, 
	 	"%s_%d",
	 	key, index );

	data = dictionary_get( dictionary, dictionary_key );

	if ( !data )
	{
		/* Try again with the index zero */
		/* ----------------------------- */
		sprintf( dictionary_key, 
		 	"%s_0",
		 	key );
		data = dictionary_get( dictionary, dictionary_key );

		if ( data )
		{
			return_value = 0;
		}
		else
		{
			/* Try with the no index */
			/* --------------------- */
			data = dictionary_get( dictionary, key );

			if ( data )
			{
				int key_index;

				key_index = get_index( key );

				if ( key_index )
				{
					return_value = 1;
				}
				else
				{
					return_value = 0;
				}
			}
			else
			{
				/* Nogo */
				/* ---- */
				return_value = -1;
				data = "";
			}
		}
	}
	else
	{
		if ( index == 0 )
		{
			return_value = 0;
		}
		else
		{
			return_value = 1;
		}
	}
	*destination  = data;
	return return_value;
} /* dictionary_get_index_data() */

void dictionary_search_replace_command_arguments( 	
					char *source_destination,
					DICTIONARY *dictionary, 
					int index )
{
	LIST *column_list;
	char *column;
	char search[ 128 ];
	char replace[ 4096 ];
	char buffer[ 4096 ];
	char *data;
	int results;

	column_list = list_usage_get_column_list( source_destination );

	if ( !list_rewind( column_list ) ) return;

	/* Skip the first because it's the executable */
	/* ------------------------------------------ */
	if ( list_length( column_list ) == 1 ) return;
	list_next( column_list );

	do {
		column = list_get_pointer( column_list );

		results = dictionary_get_index_data( 	
				&data,
				dictionary, 
				column, 
				index );

		if ( results > -1 )
		{
			sprintf( search, " %s", column );
			sprintf( replace, " \"%s\"", data );

			search_replace_word( 
					source_destination,
					search, 
					replace );
		}
		else
		/* ----------------------------------- */
		/* If no index, then check if an array */
		/* ----------------------------------- */
		if ( !index )
		{
			if ( dictionary_get_index_list_string(
					buffer,
					dictionary,
					column ) )
			{
				sprintf( search, " %s", column );
				sprintf( replace, " \"%s\"", buffer );

				search_replace_word( 
						source_destination,
						search, 
						replace );
			}
		}
	} while( list_next( column_list ) );

} /* dictionary_search_replace_command_arguments() */

LIST *dictionary_using_list_get_index_data_list(
					DICTIONARY *dictionary,
					LIST *key_list,
					int index )
{
	char *data, *key;
	int results;
	LIST *list = list_new_list();

	if ( !list_rewind( key_list ) ) return list;

	do {
		key = list_get_pointer( key_list );

		results = 
			dictionary_get_index_data(
						&data,
						dictionary,
						key,
						index );
		if ( results == -1 )
		{
			results = dictionary_get_index_data(
						&data,
						dictionary,
						key,
						0 );
		}

		if ( results != -1 )
			list_append_pointer( list, trim( data ) );
		else
			list_append_pointer( list, "" );

	} while( list_next( key_list ) );
	return list;
} /* dictionary_using_list_get_index_data_list() */

DICTIONARY *dictionary_copy_dictionary( DICTIONARY *dictionary )
{
	return copy_dictionary( dictionary );
}

DICTIONARY *dictionary_copy( DICTIONARY *dictionary )
{
	return copy_dictionary( dictionary );
}

void dictionary_set_delimited_string(
				DICTIONARY *dictionary,
				LIST *key_list,
				char *delimited_string,
				char delimiter )
{
	char *key;
	char piece_buffer[ 65536 ];
	int piece_offset = 0;

	if ( !list_reset( key_list ) ) return;

	do {
		key = list_get_string( key_list );

		if ( piece(	piece_buffer,
				delimiter,
				delimited_string,
				piece_offset ) )
		{
			dictionary_set_pointer(	dictionary,
						key,
						strdup( piece_buffer ) );
			piece_offset++;
		}
	} while( list_next( key_list ) );
} /* dictionary_set_delimited_string() */

DICTIONARY *copy_dictionary( DICTIONARY *dictionary )
{
	DICTIONARY *destination;
	LIST *key_list;
	char *key;

	if ( !dictionary ) return dictionary_small_new();

	destination = dictionary_large_dictionary_new();
	key_list = dictionary_get_key_list( dictionary );

	if ( list_reset( key_list ) )
	{
		do {
			key = list_get_string( key_list );
			dictionary_set_pointer(	destination,
						strdup( key ),
						strdup( dictionary_get(
							dictionary,
							key ) ) );
		} while( list_next( key_list ) );
	}

	list_free_container( key_list );

	return destination;

} /* copy_dictionary() */

DICTIONARY *dictionary_prepend_key(	DICTIONARY *dictionary,
					char *label )
{
	DICTIONARY *destination;
	LIST *key_list;
	char *key;
	char new_key[ 1024 ];
	int strlen_label = strlen( label );

	key_list = dictionary_get_key_list( dictionary );
	destination = dictionary_new();

	if ( list_reset( key_list ) )
	{
		do {
			key = list_get_string( key_list );

			if ( strncmp( key, label, strlen_label ) != 0 )
			{
				sprintf( new_key, "%s%s", label, key );
				dictionary_set_pointer(	destination,
							strdup( new_key ),
							dictionary_get(
								dictionary,
								key ) );
			}
		} while( list_next( key_list ) );
	}
	return destination;
} /* dictionary_prepend_key() */

DICTIONARY *dictionary_extract_prepended_key(
					DICTIONARY *dictionary,
					char *label,
					int with_strip )
{
	DICTIONARY *destination;
	LIST *key_list;
	char *key;
	char destination_key[ 1024 ];
	int str_len;

	str_len = strlen( label );
	key_list = dictionary_get_key_list( dictionary );
	destination = dictionary_new();

	if ( list_reset( key_list ) )
	{
		do {
			key = list_get_string( key_list );
			if ( strncmp( key, label, str_len ) == 0 )
			{
				if ( with_strip )
				{
					strcpy( destination_key, 
						key + str_len );
				}
				else
				{
					strcpy( destination_key, key );
				}

				dictionary_set_string(
					destination,
					strdup( destination_key ),
					dictionary_get( dictionary, key ) );
			}
		} while( list_next( key_list ) );
	}
	return destination;
} /* dictionary_extract_prepended_key() */

int dictionary_key_exists_index_zero(	DICTIONARY *indexed_dictionary,
					char *key )
{
	return 
	dictionary_exists_key_index( indexed_dictionary, key, 0 );
}

int dictionary_data_exists_index_zero(	DICTIONARY *indexed_dictionary,
					char *key )
{
	char *data;

	return ( dictionary_get_index_data(
					&data,
					indexed_dictionary,
					key,
					0 ) == 0 );
}

void dictionary_add_elements_by_removing_prefix(
				    	DICTIONARY *dictionary,
				    	char *starting_prefix )
{
	LIST *key_list;
	char *key;
	char *data;
	char *existing_data;
	char key_without_starting_prefix[ 1024 ];
	int str_len;

	str_len = strlen( starting_prefix );

	key_list = dictionary_get_key_list( dictionary );
	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			if ( strncmp( starting_prefix, key, str_len ) == 0 )
			{
				data = dictionary_get( dictionary, key );

				strcpy( key_without_starting_prefix,
					key + str_len );

				existing_data =
					dictionary_get(
						dictionary, 
						key_without_starting_prefix );

				if ( !existing_data )
				{
					dictionary_set_pointer( 
						dictionary,
						strdup(
						key_without_starting_prefix ),
						data );				
				}
			}
		} while( list_next( key_list ) );
	}
} /* dictionary_add_elements_by_removing_prefix() */

void dictionary_add_elements_by_removing_index_zero( DICTIONARY *dictionary )
{
	LIST *key_list;
	char *key;
	char *data;
	char key_without_index_zero[ 1024 ];
	int str_len;
	char *index_zero_suffix = "_0";

	str_len = strlen( index_zero_suffix );

	key_list = dictionary_get_key_list( dictionary );
	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			if ( strcmp( index_zero_suffix,
				     key + (strlen( key ) - str_len ) ) == 0 )
			{
				data = dictionary_get( dictionary, key );

				strcpy( key_without_index_zero, key );
				*(key_without_index_zero + 
				  strlen( key ) - str_len) = '\0';

				dictionary_set_pointer( 
					dictionary,
					strdup( key_without_index_zero ),
					data );				
			}
		} while( list_next( key_list ) );
	}
} /* dictionary_add_elements_by_removing_index_zero() */


DICTIONARY *dictionary_remove_prefix( 	DICTIONARY *source_dictionary,
				    	char *starting_prefix )
{
	DICTIONARY *dictionary;
	LIST *key_list;
	char *key;
	char *data;
	char key_without_starting_prefix[ 1024 ];
	int str_len;

	dictionary = dictionary_new();
	str_len = strlen( starting_prefix );

	key_list = dictionary_get_key_list( source_dictionary );
	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			data = dictionary_get( 	source_dictionary, 
						key );

			if ( strncmp( starting_prefix, key, str_len ) == 0 )
			{
				strcpy( key_without_starting_prefix,
					key + str_len );

				dictionary_set_string( 
					dictionary,
					key_without_starting_prefix,
					data );				
			}
			else
			{
				dictionary_set_string( 
					dictionary,
					key,
					data );
			}
		} while( list_next( key_list ) );
	}
	return dictionary;
} /* dictionary_remove_prefix() */

DICTIONARY *dictionary_zap_with_prefix(	DICTIONARY *dictionary,
				    	char *starting_prefix )
{
	DICTIONARY *return_dictionary;
	LIST *key_list;
	char *key;
	int str_len;

	return_dictionary = dictionary_copy( dictionary );
	str_len = strlen( starting_prefix );

	key_list = dictionary_get_key_list( return_dictionary );
	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			if ( strncmp( starting_prefix, key, str_len ) == 0 )
			{
				hash_table_remove(
					return_dictionary->hash_table,
					key );
			}
		} while( list_next( key_list ) );
	}
	return return_dictionary;
} /* dictionary_zap_with_prefix() */

DICTIONARY *dictionary_get_with_prefix(
					DICTIONARY *source_dictionary,
				    	char *starting_prefix )
{
	DICTIONARY *dictionary;
	LIST *key_list;
	char *key;
	char *data;
	int str_len;

	dictionary = dictionary_large_new();
	str_len = strlen( starting_prefix );

	key_list = dictionary_get_key_list( source_dictionary );
	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			if ( strncmp( starting_prefix, key, str_len ) == 0 )
			{
				data = dictionary_get( 	source_dictionary, 
							key );

				dictionary_set_pointer( 
					dictionary,
					strdup( key ),
					strdup( data ) );
			}
		} while( list_next( key_list ) );
	}
	list_free_container( key_list );
	return dictionary;
} /* dictionary_get_with_prefix() */

DICTIONARY *dictionary_get_without_prefix(
					DICTIONARY *source_dictionary,
				    	char *starting_prefix )
{
	DICTIONARY *dictionary;
	LIST *key_list;
	char *key;
	char *data;
	int str_len;
	char key_without_starting_prefix[ 1024 ];

	str_len = strlen( starting_prefix );

	key_list = dictionary_get_key_list( source_dictionary );

	if ( !list_rewind( key_list ) ) return (DICTIONARY *)0;

	dictionary = dictionary_small_new();

	do {
		key = list_get_pointer( key_list );

		if ( strncmp( key, starting_prefix, str_len ) == 0 )
		{
			data = dictionary_get( 	source_dictionary, 
						key );

			strcpy( key_without_starting_prefix,
				key + str_len );

			dictionary_set_pointer( 
				dictionary,
				strdup( key_without_starting_prefix ),
				strdup( data ) );
		}
	} while( list_next( key_list ) );

	list_free( key_list );

	return dictionary;

} /* dictionary_get_without_prefix() */

DICTIONARY *dictionary_remove_index( DICTIONARY *source_dictionary )
{
	DICTIONARY *dictionary;
	LIST *key_list;
	char *key;
	char *data;
	char key_without_index[ 1024 ];

	dictionary = dictionary_new();

	key_list = dictionary_get_key_list( source_dictionary );
	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			data = dictionary_get_pointer(
						source_dictionary, 
						key );

			trim_index( key_without_index, key );

			dictionary_set_pointer(
				dictionary,
				strdup( key_without_index ),
				data );				
		} while( list_next( key_list ) );
	}
	return dictionary;
} /* dictionary_remove_index() */

DICTIONARY *dictionary_remove_index_zero( DICTIONARY *source_dictionary )
{
	DICTIONARY *dictionary;
	LIST *key_list;
	char *key;
	char *data;
	char key_without_index[ 1024 ];

	dictionary = dictionary_new();

	key_list = dictionary_get_key_list( source_dictionary );
	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			data = dictionary_get_pointer(
						source_dictionary, 
						key );

			trim_index_zero( key_without_index, key );

			dictionary_set_pointer(
				dictionary,
				strdup( key_without_index ),
				data );				
		} while( list_next( key_list ) );
	}
	return dictionary;
} /* dictionary_remove_index_zero() */

DICTIONARY *dictionary_extract_key_prefix( 	DICTIONARY *source_dictionary,
						char *starting_prefix )
{
	DICTIONARY *dictionary;
	LIST *key_list;
	char *key;
	char *data;
	int str_len;

	dictionary = dictionary_new();
	str_len = strlen( starting_prefix );

	key_list = dictionary_get_key_list( source_dictionary );
	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			if ( strncmp( starting_prefix, key, str_len ) == 0 )
			{
				data = dictionary_get( 	source_dictionary, 
							key );

				dictionary_set_string( 
					dictionary,
					key,
					data );				
			}
		} while( list_next( key_list ) );
	}
	return dictionary;
} /* dictionary_extract_key_prefix() */

LIST *dictionary_extract_and_remove_prefixed_key_list(
					DICTIONARY *dictionary,
					char *starting_prefix )
{
	LIST *prefixed_key_list;

	prefixed_key_list =
		dictionary_extract_prefixed_key_list(
			dictionary,
			starting_prefix );

	return list_usage_remove_prefix( prefixed_key_list, starting_prefix );

} /* dictionary_extract_and_remove_prefixed_key_list() */

LIST *dictionary_extract_prefixed_key_list( 	DICTIONARY *dictionary,
						char *starting_prefix )
{
	LIST *key_list;
	LIST *return_key_list;
	char *key;
	int str_len;

	str_len = strlen( starting_prefix );

	return_key_list = list_new_list();
	key_list = dictionary_get_key_list( dictionary );

	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			if ( strncmp( starting_prefix, key, str_len ) == 0 )
			{
				list_append_pointer( return_key_list, key );
			}
		} while( list_next( key_list ) );
	}
	return return_key_list;
} /* dictionary_extract_prefixed_key_list() */

char *dictionary_get_with_prefix_at_index_zero(
					DICTIONARY *dictionary,
					char *prefix,
					char *key )
{
	char dictionary_key[ 1024 ];

	sprintf( dictionary_key, 
	 	"%s%s_0",
	 	prefix, key );
	return dictionary_get( dictionary, dictionary_key );
}

/* --------------------------------------------------------------------------- 
Two notes:
1) No memory gets allocated. Both the key and data still exist
   in the append_dictionary dictionary.
2) If a they share a key, then it doesn't clobber the source_destinatation.
--------------------------------------------------------------------------- */
void dictionary_append_row_zero(	DICTIONARY *source_destination,
					DICTIONARY *append_dictionary )
{
	LIST *key_list;
	char *key;
	int key_length;

	if ( !append_dictionary ) return;
	if ( !source_destination ) return;

	key_list = dictionary_get_key_list( append_dictionary );

	if ( list_reset( key_list ) )
	{
		do {
			key = list_get_string( key_list );

			key_length = strlen( key );

			if ( key_length < 2 ) continue;

			if ( strcmp( ( key + key_length ) - 2, "_0" ) != 0 )
				continue;

			if ( !dictionary_exists_key(
						source_destination,
						key ) )
			{
				dictionary_set_pointer(
						source_destination,
						key,
						dictionary_get(
							append_dictionary,
							key ) );
			}
		} while( list_next( key_list ) );
	}
} /* dictionary_append_row_zero() */

/* --------------------------------------------------------------------------- 
Two notes:
1) No memory gets allocated. Both the key and data still exist
   in the append_dictionary dictionary.
2) If a they share a key, then it doesn't clobber the source_destinatation.
--------------------------------------------------------------------------- */
void dictionary_append_dictionary(	DICTIONARY *source_destination,
					DICTIONARY *append_dictionary )
{
	LIST *key_list;
	LIST *source_destination_non_indexed_key_list;
	char *key;
	char non_indexed_key[ 1024 ];


	if ( !append_dictionary ) return;
	if ( !source_destination ) return;

	key_list = dictionary_get_key_list( append_dictionary );

	source_destination_non_indexed_key_list =
		dictionary_get_non_indexed_key_list(
			source_destination );

	if ( !list_rewind( key_list ) ) return;

	do {
		key = list_get_string( key_list );

		timlib_trim_index( non_indexed_key, key );

		if ( !list_exists_string(
			source_destination_non_indexed_key_list,
			non_indexed_key ) )
		{
			dictionary_set_pointer(
					source_destination,
					key,
					dictionary_get(
						append_dictionary,
						key ) );
		}
	} while( list_next( key_list ) );

} /* dictionary_append_dictionary() */

void dictionary_add_index_zero( DICTIONARY *dictionary )
{
	dictionary_add_suffix_zero( dictionary );
}
 
void dictionary_add_suffix_zero( DICTIONARY *dictionary )
{
	LIST *key_list;
	char *key;
	char *data;
	char key_with_index_zero[ 1024 ];
	char *index_zero_suffix = "_0";

	key_list = dictionary_get_key_list( dictionary );
	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );
			data = dictionary_get( dictionary, key );

			if ( !dictionary_exists_index_in_key( key ) )
			{
				sprintf( key_with_index_zero, 
				 	"%s%s", 
				 	key,
				 	index_zero_suffix );

				dictionary_set_string( 
						dictionary,
						key_with_index_zero,
						data );				
			}
		} while( list_next( key_list ) );
	}
} /* dictionary_add_suffix_zero() */

int dictionary_exists_key_in_list(
		LIST *key_list,
		DICTIONARY *dictionary )
{
	char *data;
	char *key;

	if ( !list_rewind( key_list ) ) return 0;

	do {
		key = list_get_string( key_list );
		data = dictionary_get( dictionary, key );
		if ( (int)data ) return 1;
	} while( list_next( key_list ) );
	return 0;
} /* dictionary_exists_key_in_list() */

LIST *dictionary_get_populated_index_zero_key_list(
					DICTIONARY *dictionary, 
					LIST *attribute_name_list )
{
	LIST *return_list = list_new_list();
	char *attribute_name;

	if ( list_reset( attribute_name_list ) )
		do {
			attribute_name =
				list_get_pointer( attribute_name_list );

			if ( dictionary_populated_key_exists_index_zero(
					dictionary,
					attribute_name ) )
			{
				list_append_pointer( 	return_list, 
							attribute_name );
			}
		} while( list_next( attribute_name_list ) );
	return return_list;
} /* dictionary_get_populated_index_zero_key_list() */

LIST *dictionary_get_non_populated_index_zero_key_list(
					DICTIONARY *dictionary, 
					LIST *required_attribute_name_list )
{
	LIST *return_list = list_new_list();
	char *attribute_name;

	if ( !list_reset( required_attribute_name_list ) )
		return return_list;

	do {
		attribute_name =
			list_get_pointer( required_attribute_name_list );

		if ( dictionary_non_populated_key_exists_index_zero(
				dictionary,
				attribute_name ) )
		{
			list_append_pointer( 	return_list, 
						attribute_name );
		}
	} while( list_next( required_attribute_name_list ) );
	return return_list;
} /* dictionary_get_non_populated_index_zero_key_list() */

LIST *dictionary_get_index_zero_key_list(	DICTIONARY *dictionary,
						LIST *attribute_name_list )
{
	LIST *return_list = create_list();
	char *attribute_name;

	if ( list_reset( attribute_name_list ) )
		do {
			attribute_name =
				list_get_pointer( attribute_name_list );

			if ( dictionary_key_exists_index_zero(
					dictionary,
					attribute_name ) )
			{
				list_append_pointer( 	return_list, 
							attribute_name );
			}
		} while( list_next( attribute_name_list ) );
	return return_list;
} /* dictionary_get_index_zero_key_list() */

char *dictionary_get_delimited_data(		DICTIONARY *dictionary,
						LIST *attribute_name_list,
						char delimiter )
{
	char delimited_data[ 1024 ];
	char *delimited_data_ptr = delimited_data;
	char *attribute_name;
	char *data;
	int first_time = 1;

	*delimited_data = '\0';

	if ( !list_reset( attribute_name_list ) ) return "";
	do {
		attribute_name =
			list_get_pointer( attribute_name_list );

		if ( ( data = dictionary_get_index_zero(
				dictionary,
				attribute_name ) ) )
		{
			if ( first_time )
			{
				delimited_data_ptr +=
					sprintf( delimited_data_ptr,
						 "%s",
						 data );
				first_time = 0;
			}
			else
			{
				delimited_data_ptr +=
					sprintf( delimited_data_ptr,
						 "%c%s",
						 delimiter,
						 data );
			}
		}
	} while( list_next( attribute_name_list ) );
	return strdup( delimited_data );
} /* dictionary_get_delimited_data() */

LIST *dictionary_get_index_zero_data_list(	DICTIONARY *dictionary,
						LIST *attribute_name_list )
{
	LIST *return_list = create_list();
	char *attribute_name;
	char *data;

	if ( list_reset( attribute_name_list ) )
		do {
			attribute_name =
				list_get_pointer( attribute_name_list );

			if ( ( data = dictionary_get_index_zero(
					dictionary,
					attribute_name ) ) )
			{
				list_append_pointer( 	return_list, 
							data );
			}
		} while( list_next( attribute_name_list ) );
	return return_list;
} /* dictionary_get_index_zero_data_list() */

char *dictionary_new_index_key( DICTIONARY *d, char *key )
{
	char key_without_index[ 1024 ];
	char new_key[ 1024 ];
	int index;

	trim_index( key_without_index, key );
	for ( index = 0 ;; index++ )
	{
		sprintf( new_key, "%s_%d", key_without_index, index );

		if( !dictionary_key_exists( d, new_key ) )
		{
			return strdup( new_key );
		}
	}
} /* dictionary_new_index_key() */

LIST *dictionary_get_index_list(	DICTIONARY *d,
					char *search_key_without_prefix )
{
	LIST *return_list = create_list();
	char *data;
	int index;
	int results;

	for( index = 1 ;; index++ )
	{
		results =
			dictionary_get_index_data(
					&data,
					d,
					search_key_without_prefix,
					index );

		if ( results == -1 ) return return_list;

		list_append_string( return_list, data );

		/* If got from index zero then finished */
		/* ------------------------------------ */
		if ( results == 0 ) return return_list;
	}
} /* dictionary_get_index_list() */

LIST *dictionary_get_positive_index_list(
					DICTIONARY *d,
					char *search_key_without_prefix )
{
	LIST *return_list = create_list();
	char *data;
	int index;
	int results;

	for( index = 1 ;; index++ )
	{
		results =
			dictionary_get_index_data(
					&data,
					d,
					search_key_without_prefix,
					index );
/*
	char search_key[ 1024 ];
		sprintf( search_key,
			 "%s_%d",
			 search_key_without_prefix,
			 index );

		data = dictionary_get_string( d, search_key );
		if ( !data ) return return_list;
*/

		if ( results <= 0 ) return return_list;

		list_append_string( return_list, data );
	}
} /* dictionary_get_positive_index_list() */

DICTIONARY *dictionary_new_dictionary( void )
{
	return dictionary_new();
}

DICTIONARY *new_dictionary( void )
{
	return dictionary_new();
}

DICTIONARY *dictionary_pipe2dictionary( char *sys_string, char delimiter )
{
	return pipe2dictionary( sys_string, delimiter );
}

DICTIONARY *pipe2dictionary( char *sys_string, char delimiter )
{
	char buffer[ 65536 ];
	char data[ 65536 ];
	char key[ 1024 ];
	FILE *p = popen( sys_string, "r" );
	DICTIONARY *d = dictionary_large_new();

	*data = '\0';
	while( get_line( buffer, p ) )
	{
		if ( delimiter )
		{
			piece( key, delimiter, buffer, 0 );
			if ( !piece( data, delimiter, buffer, 1 ) )
				*buffer = '\0';
		}
		else
		{
			strcpy( key, buffer );
		}
		dictionary_set_pointer( d, strdup( key ), strdup( data ) );
	}
	pclose( p );
	return d;
} /* pipe2dictionary() */

DICTIONARY *dictionary_index_data2dictionary(
					DICTIONARY *source_dictionary,
					char *key_wo_index )
{
	int highest_index, index, results;
	char *data;
	DICTIONARY *destination_dictionary;
	char index_string[ 128 ];

	destination_dictionary = new_dictionary();

	highest_index = get_dictionary_key_highest_index( source_dictionary );

	for ( index = 0; index <= highest_index; index++ )
	{
		results = dictionary_get_index_data(	&data,
							source_dictionary,
							key_wo_index,
							index );
		if ( results == 1 )
		{
			sprintf( index_string, "%d", index );
			dictionary_add_string(
					destination_dictionary,
					data,
					strdup( index_string ) );
		}
	}

	return destination_dictionary;
} /* dictionary_index_data2dictionary() */

DICTIONARY *dictionary_merge_dictionary(
					DICTIONARY *dictionary1,
					DICTIONARY *dictionary2 )
{
	LIST *key_list;
	char *key;

	key_list = dictionary_get_key_list( dictionary2 );
	if ( list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			if ( !dictionary_exists_key(
						dictionary1,
						key ) )
			{
				dictionary_set_pointer(
						dictionary1,
						key,
						dictionary_get_data(
							dictionary2,
							key ) );
			}
		} while( list_next( key_list ) );
	}
	return dictionary1;
} /* dictionary_merge_dictionary() */

DICTIONARY *dictionary_merge_lists2dictionary(
					LIST *key_list,
					LIST *data_list )
{
	char *key;
	DICTIONARY *dictionary;

	dictionary = dictionary_new();

	if ( list_rewind( key_list ) && list_rewind( data_list ) )
	{
		do {
			key = list_get_string( key_list );
			dictionary_set_string(	dictionary,
						key,
						list_get_string( data_list ) );
			if ( !list_next( data_list ) ) break;
		} while( list_next( key_list ) );
	}
	return dictionary;
} /* dictionary_merge_lists2dictionary() */

void dictionary_add_constant(	DICTIONARY *dictionary,
				LIST *data_list,
				char *constant )
{
	if ( list_rewind( data_list ) )
	{
		do {
			dictionary_set_string(
					dictionary,
					list_get_string( data_list ),
					constant );
		} while( list_next( data_list ) );
	}
} /* dictionary_add_constant() */

int dictionary_remove_key(	DICTIONARY *d,
				char *key )
{
	return hash_table_remove_data( d->hash_table, key );
}

int dictionary_remove_data(	DICTIONARY *d,
				char *key )
{
	return hash_table_remove_data( d->hash_table, key );
}

void dictionary_new_index_key_list_for_data_list(
					DICTIONARY *indexed_dictionary,
					LIST *old_key_list,
					LIST *new_key_list,
					int index )
{
	char *old_key;
	char *new_key;
	char *data;
	char full_key[ 512 ];

	if ( !list_rewind( old_key_list ) ) return;
	if ( !list_rewind( new_key_list ) ) return;

	do {
		old_key = list_get_pointer( old_key_list );
		new_key = list_get_pointer( new_key_list );

		if ( dictionary_get_index_data(
				&data,
				indexed_dictionary,
				old_key,
				index ) != -1 )
		{
			sprintf(full_key,
				"%s_%d",
				new_key,
				index );

			dictionary_set_string(	indexed_dictionary,
						strdup( full_key ),
						data );
		}
		if ( !list_next( new_key_list ) ) return;
	} while( list_next( old_key_list ) );
} /* dictionary_new_index_key_list_for_data_list() */

void dictionary_search_replace_special_characters( DICTIONARY *dictionary )
{
	LIST *key_list;
	char *key;
	char *data;

	key_list = dictionary_get_key_list( dictionary );
	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );
			data = dictionary_get( dictionary, key );

			search_replace_special_characters( data );

			dictionary_set_pointer( 
					dictionary,
					key,
					data );				
		} while( list_next( key_list ) );
	}
} /* dictionary_search_replace_special_characters() */

DICTIONARY *dictionary_subtract_dictionary(
					DICTIONARY *d1,
					DICTIONARY *d2 )
{
	LIST *d2_key_list;

	d2_key_list = dictionary_get_key_list( d2 );
	if ( list_rewind( d2_key_list ) )
	{
		do {
			dictionary_remove_data(
				d1,
				list_get_pointer( d2_key_list ) );
		} while( list_next( d2_key_list ) );
	}
	return d1;
} /* dictionary_subtract_dictionary() */

boolean dictionary_exists_suffix( char *key )
{
	return dictionary_exists_index_in_key( key );
}

boolean dictionary_exists_index_in_key( char *key )
{
	char *end = key + strlen( key ) - 1;

	while( end != key )
	{
		if ( *end == '_' ) return 1;
		if ( !isdigit( *end ) ) return 0;
		end--;
	}
	return 0;
} /* dictionary_exists_index_in_key() */

void dictionary_replace_double_quote_with_single(
					DICTIONARY *dictionary )
{
	char *key;
	char *data;
	LIST *key_list;

	key_list = get_dictionary_key_list( dictionary );

	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );
			data = dictionary_get_data(
					dictionary,
					key );
			search_replace_string( data, "\"", "'" );
		} while( list_next( key_list ) );
	}
} /* dictionary_replace_double_quote_with_single() */

void dictionary_escape_single_quotes( DICTIONARY *dictionary )
{
	char *key;
	char *data;
	LIST *key_list;
	char new_data[ 4096 ];

	key_list = get_dictionary_key_list( dictionary );

	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );
			data = dictionary_get_data(
					dictionary,
					key );

			if ( character_count( '\'', data ) > 0 )
			{
				strcpy( new_data, data );
				escape_single_quotes( new_data );
				free( data );
				dictionary_set_pointer(
					dictionary,
					key,
					strdup( new_data ) );
			}
		} while( list_next( key_list ) );
	}
} /* dictionary_escape_single_quotes() */


LIST *dictionary_get_indexed_data_list(
					DICTIONARY *dictionary,
					char *key_prefix )
{
	char full_key[ 512 ];
	char *data;
	LIST *populated_list = list_new_list();
	int highest_index;
	int index;

	highest_index = dictionary_get_key_highest_index( dictionary );

	for ( index = 1; index <= highest_index; index++ )
	{
		sprintf(full_key,
			"%s_%d",
			key_prefix,
			index );

		if ( ( data = dictionary_get_data(
					dictionary,
					full_key ) ) )
		{
			list_append_pointer( populated_list, data );
		}
	}
	return populated_list;
} /* dictionary_get_indexed_data_list() */

LIST *dictionary_get_populated_index_list(
					DICTIONARY *dictionary,
					char *key_prefix )
{
	char full_key[ 512 ];
	char *data;
	LIST *populated_index_list = list_new_list();
	int highest_index;
	int index;
	char index_string[ 64 ];

	highest_index = dictionary_get_key_highest_index( dictionary );

	for ( index = 1; index <= highest_index; index++ )
	{
		sprintf(full_key,
			"%s_%d",
			key_prefix,
			index );

		if ( ( data = dictionary_get_data(
					dictionary,
					full_key ) ) )
		{
			sprintf( index_string, "%d", index );
			list_append_pointer(	populated_index_list,
						strdup( index_string ) );
		}
	}
	return populated_index_list;
} /* dictionary_get_populated_index_list() */

LIST *dictionary_with_populated_index_list_get_index_data_list(
					DICTIONARY *dictionary,
					LIST *populated_index_list,
					char *key_prefix )
{
	char full_key[ 512 ];
	char *data;
	LIST *index_data_list = list_new_list();
	char *index;

	if ( list_rewind( populated_index_list ) )
	{
		do {
			index = list_get_pointer( populated_index_list );
			sprintf(full_key,
				"%s_%s",
				key_prefix,
				index );

			if ( ( data = dictionary_get_data(
						dictionary,
						full_key ) ) )
			{
				list_append_pointer(	index_data_list,
							data );
			}
			else
			{
				list_append_pointer(	index_data_list,
							"" );
			}
		} while( list_next( populated_index_list ) );
	}
	return index_data_list;
} /* dictionary_with_populated_index_list_get_index_data_list() */

DICTIONARY *dictionary_record_to_dictionary(
					LIST *key_list,
					char *record,
					char delimiter )
{
	return dictionary_merge_lists2dictionary(
				key_list,
				list_string_to_list( record, delimiter ) );
} /* dictionary_record_to_dictionary() */

void dictionary_trim_double_bracked_data( DICTIONARY *dictionary )
{
	char *key;
	char *data;
	LIST *key_list;

	key_list = get_dictionary_key_list( dictionary );

	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			data = dictionary_get_data(
					dictionary,
					key );

			dictionary_trim_double_bracked_string( data );

		} while( list_next( key_list ) );
	}
} /* dictionary_trim_double_bracked_data() */

char *dictionary_trim_double_bracked_string( char *string )
{
	int start, end;
	int str_len;

	str_len = strlen( string );

	/* Don't trim middle brackets. */
	/* --------------------------- */
	if ( str_len
	&&   *( string + str_len - 1 ) == ']' )
	{
		if ( ( start = instr( "[", string, 1 ) ) != -1
		&&   ( end = instr( "]", string, 1 ) ) != -1 )
		{
			strcpy( string + start, string + end + 1 );
			right_trim( string );
		}
	}

	return string;

} /* dictionary_trim_double_bracked_string() */

int dictionary_delete_key( DICTIONARY *dictionary, char *key )
{
	return hash_table_remove_data( dictionary->hash_table, key );
}

int dictionary_delete( DICTIONARY *dictionary, char *key )
{
	return hash_table_remove_data( dictionary->hash_table, key );
}

void dictionary_delete_prefixed_keys(
					DICTIONARY *dictionary,
					char *prefix_string )
{
	LIST *key_list;
	char *key;
	int str_len;

	str_len = strlen( prefix_string );

	key_list = dictionary_get_key_list( dictionary );
	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			if ( strncmp( key, prefix_string, str_len ) == 0 )
			{
				dictionary_delete( dictionary, key );
			}
		} while( list_next( key_list ) );
	}
} /* dictionary_delete_prefixed_keys() */

DICTIONARY *dictionary_load_record_dictionary(
			FILE *input_pipe,
			int delimiter )
{
	char buffer[ 4096 ];
	char key[ 1024 ];
	char data[ 3072 ];
	char first_key[ 1024 ];
	DICTIONARY *d = (DICTIONARY *)0;
	int first_time = 1;

	while( get_line_queue( buffer, input_pipe ) )
	{
		piece( key, delimiter, buffer, 0 );

		if ( first_time )
		{
			d = new_dictionary();
			strcpy( first_key, key );
			first_time = 0;
		}
		else
		{
			if ( strcmp( key, first_key ) == 0 )
			{
				unget_line_queue( buffer );
				break;
			}
		}
		
		if ( piece( data, delimiter, buffer, 1 ) )
		{
			dictionary_set_pointer(	d,
						strdup( key ),
						strdup( data ) );
		}
	}
	return d;
} /* dictionary_load_record_dictionary() */

char *dictionary_fetch( DICTIONARY *d, char *key )
{
	return dictionary_get_pointer( d, key );
}

char *dictionary_safe_fetch( DICTIONARY *d, char *key )
{
	char *results;

	if ( ( results = dictionary_get_pointer( d, key ) ) )
		return results;
	else
		return "";
}

void dictionary_increment_count(
				DICTIONARY *dictionary,
				char *key )
{
	char *data_pointer;
	char count_string[ 32 ];
	int current_count;

	if ( ( data_pointer = dictionary_fetch( dictionary, key ) ) )
	{
		current_count = atoi( data_pointer );
		sprintf( count_string, "%d", current_count + 1 );
		free( data_pointer );
		data_pointer = strdup( count_string );
	}
	else
	{
		dictionary_set_pointer(	dictionary,
					strdup( key ),
					strdup( "1" ) );
	}

} /* dictionary_increment_count() */

void dictionary_output_with_prefix(
				DICTIONARY *dictionary,
				char *prefix )
{
	char *key;
	LIST *key_list;

	key_list = get_dictionary_key_list( dictionary );

	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );
			printf( "%s%s=%s\n",
				prefix,
				key,
				dictionary_get_pointer(
					dictionary,
					key ) );
		} while( list_next( key_list ) );
	}
} /* dictionary_output_with_prefix() */

void dictionary_output_html_table(
				DICTIONARY *dictionary,
				char *heading1,
				char *heading2,
				boolean align_right )
{
	char *key;
	LIST *key_list;

	key_list = get_dictionary_key_list( dictionary );

	printf( "<table border=1>\n" );

	printf( "<th>%s", heading1 );

	if ( align_right )
		printf( "<th align=right>%s\n", heading2 );
	else
		printf( "<th>%s\n", heading2 );

	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );
			printf( "<tr><td>%s</td>\n",
				key );

			if ( align_right )
			{
				printf( "<td align=right>%s</td>\n",
					dictionary_get_pointer(
						dictionary,
						key ) );
			}
			else
			{
				printf( "<td>%s</td>\n",
					dictionary_get_pointer(
						dictionary,
						key ) );
			}
		} while( list_next( key_list ) );
	}
	printf( "</table>\n" );
} /* dictionary_output_html_table() */

void dictionary_convert_index_to_index_zero(	DICTIONARY *dictionary,
						int index )
{
	char *key;
	LIST *key_list;
	int key_index;
	char *data;
	char new_key[ 1024 ];
	char key_without_index[ 1024 ];

	key_list = get_dictionary_key_list( dictionary );

	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );
			key_index = get_index( key );
			if ( key_index == index )
			{
				data = dictionary_get( dictionary, key );
				trim_index( key_without_index, key );
				sprintf(new_key, 
	 				"%s_0",
	 				key_without_index );

				dictionary_set_string( 
						dictionary, 
						strdup( new_key ),
						data );

			}
		} while( list_next( key_list ) );
	}
} /* dictionary_convert_index_to_index_zero() */

DICTIONARY *dictionary_add_prefix(	DICTIONARY *dictionary,
					char *prefix )
{
	char *key;
	char *data;
	LIST *key_list;
	char new_key[ 1024 ];
	DICTIONARY *new_dictionary;

	if ( !dictionary ) return (DICTIONARY *)0;

	new_dictionary = dictionary_large_new();

	key_list = get_dictionary_key_list( dictionary );

	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			sprintf(new_key,
				"%s%s",
				prefix,
				key );

			if ( ( data = dictionary_get_pointer(
						dictionary,
						key ) ) )
			{
				dictionary_set_pointer(
					new_dictionary,
					strdup( new_key ),
					strdup( data ) );
			}

		} while( list_next( key_list ) );
	}
	list_free_container( key_list );
	return new_dictionary;
} /* void dictionary_add_prefix() */

LIST *dictionary_get_key_data_list(	DICTIONARY *dictionary,
					LIST *key_list,
					char delimiter )
{
	char *data, *key;
	char key_data[ 1024 ];
	LIST *list = list_new_list();

	if ( list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			if ( ( data =
				dictionary_get_data( dictionary, key ) ) )
			{
				sprintf( key_data,
					 "%s%c%s",
					 key,
					 delimiter,
					 data );
				list_append_pointer( list, strdup( key_data ) );
			}

		} while( list_next( key_list ) );
	}
	return list;
} /* dictionary_get_key_data_list() */

void dictionary_clear_key_list(		DICTIONARY *dictionary,
					LIST *key_list )
{
	char *data, *key;

	if ( list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			if ( ( data =
				dictionary_get_data( dictionary, key ) ) )
			{
				hash_table_remove(
					dictionary->hash_table,
					key );
			}

		} while( list_next( key_list ) );
	}
} /* dictionary_clear_key_list() */

void dictionary_clear_key_list_index_zero(
					DICTIONARY *dictionary,
					LIST *key_list )
{
	char *data, *key;
	char indexed_key[ 512 ];

	if ( list_rewind( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			sprintf( indexed_key, "%s_0", key );
			if ( ( data =
				dictionary_get_data(	dictionary,
							indexed_key ) ) )
			{
				hash_table_remove(
					dictionary->hash_table,
					indexed_key );
			}

		} while( list_next( key_list ) );
	}
} /* dictionary_clear_key_list_index_zero() */

void dictionary_trim_multi_drop_down_index(
			DICTIONARY *dictionary,
			char multi_select_move_left_right_index_delimiter )
{
	char *ptr;
	char *data;
	char *key;
	LIST *key_list;
	char place_holder = multi_select_move_left_right_index_delimiter;

	key_list = dictionary_get_key_list( dictionary );

	if ( !list_rewind( key_list ) ) return;

	do {
		key = list_get_pointer( key_list );

		data = dictionary_get_data( dictionary, key );

		if ( !strlen( data ) ) continue;

		ptr = data + strlen( data ) - 1;

		while( ptr > data )
		{
			if ( isdigit( *ptr ) )
			{
				ptr--;
			}
			else
			if ( *ptr == place_holder )
			{
				*ptr = '\0';
				break;
			}
			else
			{
				break;
			}
		}
	} while( list_next( key_list ) );
} /* dictionary_trim_multi_drop_down_index() */

boolean dictionary_get_index_list_string(
					char *destination,
					DICTIONARY *dictionary,
					char *key_without_index )
{
	int highest_index, index, results;
	char *data;
	char escaped_data[ 1024 ];
	boolean found_any = 0;
	int first_time = 1;

	*destination = '\0';
	highest_index = dictionary_get_key_highest_index( dictionary );
	if ( !highest_index ) return 0;
	
	for ( index = 1; index <= highest_index; index++ )
	{
		results = dictionary_get_index_data(	&data,
							dictionary,
							key_without_index,
							index );
		if ( results == 1 )
		{
			escape_character( escaped_data, data, ',' );

			if ( first_time )
			{
				destination +=
					sprintf(destination,
						"%s",
						escaped_data );
				first_time = 0;
			}
			else
			{
				destination +=
					sprintf(destination,
						",%s",
						escaped_data );
			}
			found_any = 1;
		}
	}

	return found_any;

} /* dictionary_get_index_list_string() */

/* ------------------------------------------------------------ */
/* Sample:							*/
/* Input:  "compare_station|compare_datatype_1=a|b|c|d|e"	*/
/* Return: "a,b,c,d,e"						*/
/* ------------------------------------------------------------ */
LIST *dictionary_get_prefixed_indexed_data_list(
					DICTIONARY *dictionary,
					char *prompt_prefix,
					int dictionary_index,
					char delimiter )
{
	LIST *key_list;
	char *key;
	char *data;
	LIST *return_list;
	int str_len = 0;

	if ( !prompt_prefix )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty prompt_prefix.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	key_list = dictionary_get_key_list( dictionary );

	str_len = strlen( prompt_prefix );

	if ( !( key_list && list_reset( key_list ) ) )
	{
		return (LIST *)0;
	}
	
	do {
		key = list_get_pointer( key_list );

		if ( strncmp( key, prompt_prefix, str_len ) != 0 )
			continue;

		if ( get_index_from_string( key ) != dictionary_index )
			continue;

		data = dictionary_get_pointer( dictionary, key );
		return_list = list_string2list( data, delimiter );
		list_free_container( key_list );
		return return_list;
	} while( list_next( key_list ) );
	return (LIST *)0;
} /* dictionary_get_prefixed_indexed_data_list() */

void dictionary_remove_keys_with_prefix(
					DICTIONARY *dictionary,
					char *prefix )
{
	dictionary_delete_prefixed_keys( dictionary, prefix );
}

void dictionary_trim_prefix(		DICTIONARY *dictionary,
					char *prefix )
{
	LIST *key_list;
	char *key;
	char *data;
	char key_without_starting_prefix[ 1024 ];
	int str_len;

	str_len = strlen( prefix );

	key_list = dictionary_get_key_list( dictionary );
	if ( !key_list || !list_rewind( key_list ) ) return;

	do {
		key = list_get_pointer( key_list );

		data = dictionary_get( 	dictionary, 
					key );

		if ( strncmp( prefix, key, str_len ) == 0 )
		{
			strcpy( key_without_starting_prefix,
				key + str_len );

			dictionary_set_pointer( 
				dictionary,
				strdup( key_without_starting_prefix ),
				data );

			dictionary_delete( dictionary, key );
		}
	} while( list_next( key_list ) );
} /* dictionary_trim_prefix() */

DICTIONARY *dictionary_zap_index_zero_attribute_name_list(
					DICTIONARY *dictionary,
					LIST *attribute_name_list )
{
	char *attribute_name;
	char dictionary_key[ 1024 ];

	if ( list_rewind( attribute_name_list ) )
	{
		do {
			attribute_name =
				list_get_pointer( attribute_name_list );

			if ( dictionary_populated_key_exists_index_zero(
					dictionary,
					attribute_name ) )
			{
				sprintf(dictionary_key,
	 				"%s_0",
	 				attribute_name );

				hash_table_remove(
					dictionary->hash_table,
					dictionary_key );

				hash_table_remove(
					dictionary->hash_table,
					attribute_name );
			}
		} while( list_next( attribute_name_list ) );
	}
	return dictionary;
} /* dictionary_zap_index_zero_attribute_name_list() */

void dictionary_zap_where_key_contains_character(
					DICTIONARY *dictionary,
					char character )
{
	LIST *key_list;
	char *key;

	key_list = dictionary_get_key_list( dictionary );
	if ( !key_list || !list_rewind( key_list ) ) return;

	do {
		key = list_get_pointer( key_list );

		if ( timlib_exists_character( key, character ) )
		{
			dictionary_delete( dictionary, key );
		}
	} while( list_next( key_list ) );
} /* dictionary_zap_where_key_contains_character() */

LIST *dictionary_get_data_list(	LIST *attribute_name_list,
				DICTIONARY *dictionary,
				int row )
{
	LIST *data_list;
	char *data;
	char *attribute_name;

	if ( !list_rewind( attribute_name_list ) ) return (LIST *)0;

	data_list = list_new();

	do {
		attribute_name = list_get_pointer( attribute_name_list );

		if ( dictionary_get_index_data(
					&data,
					dictionary,
					attribute_name,
					row ) )
		{
			list_append_pointer( data_list, data );
		}

	} while( list_next( attribute_name_list ) );

	return data_list;

} /* dictionary_get_data_list() */

char *dictionary_get_attribute_where_clause(
			DICTIONARY *dictionary,
			LIST *attribute_name_list )
{
	LIST *key_list;
	char *key;
	char *data;
	char *translated_data;
	char where_clause[ 65536 ];
	char *where_ptr = where_clause;
	char return_date[ 128 ];
	char escaped_data[ 512 ];

	key_list = dictionary_get_key_list( dictionary );

	where_ptr += sprintf( where_ptr, "1 = 1" );

	if ( !list_rewind( key_list ) ) return strdup( where_clause );

	do {
		key = list_get_pointer( key_list );

		if ( list_exists_string( attribute_name_list, key ) )
		{
			data = dictionary_get_pointer( dictionary, key );

			translated_data =
				dictionary_get_pointer(
					dictionary,
					data );

			if ( translated_data ) data = translated_data;

			if ( date_convert_source_unknown(
				return_date,
				international,
				data ) )
			{
				where_ptr += sprintf(	where_ptr,
							" and %s = '%s'",
							key,
							return_date );
			}
			else
			{
				strcpy( escaped_data, data );
				timlib_escape_field( escaped_data );

				where_ptr += sprintf(	where_ptr,
							" and %s = '%s'",
							key,
							escaped_data );
			}
		}
	} while( list_next( key_list ) );

	return strdup( where_clause );

} /* dictionary_get_attribute_where_clause() */

void dictionary_add_login_name_if_necessary(
				DICTIONARY *dictionary,
				LIST *attribute_name_list,
				char *login_name )
{
	if ( list_exists_string( attribute_name_list, "login_name" ) )
	{
		char operator_entry[ 128 ];

		if ( !dictionary_exists_key_index( dictionary, 
						   "login_name",
						   0 ) )
		{
			dictionary_add_string(	dictionary,
						"login_name_0",
						login_name );

			sprintf(operator_entry,
		 		"%s%s",
		 		QUERY_RELATION_OPERATOR_STARTING_LABEL,
		 		"login_name" );

			dictionary_set_pointer(	dictionary,
						strdup( operator_entry ),
						EQUAL_OPERATOR );
		}
	}
} /* dictionary_add_login_name_if_necessary() */

void dictionary_list_output_to_file(	char *output_filename, 
					LIST *row_dictionary_list,
					LIST *attribute_name_list,
					char *heading_display )
{
	DICTIONARY *row_dictionary;
	FILE *output_file;

	output_file = fopen( output_filename, "w" );

	if ( !output_file )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot open %s\n",
			 __FILE__,
			 __FUNCTION__,
			 output_filename );
		exit( 1 );
	}

	if ( heading_display && *heading_display )
		fprintf( output_file, "%s\n", heading_display );

	dictionary_output_heading_to_file(
		output_file,
		attribute_name_list );

	if ( !list_rewind( row_dictionary_list ) )
	{
		fclose( output_file );
		return;
	}

	do {
		row_dictionary = list_get_pointer( row_dictionary_list );

		dictionary_row_output_to_file(	output_file,
						row_dictionary,
						attribute_name_list );

	} while( list_next( row_dictionary_list ) );

	fclose( output_file );

} /* dictionary_list_output_to_file() */

void dictionary_row_output_to_file(
				FILE *output_file, 
				DICTIONARY *row_dictionary,
				LIST *attribute_name_list )
{
	char *value;
	char *attribute_name;
	int first_time = 1;

	if ( !row_dictionary ) return;

	if ( !list_rewind( attribute_name_list ) ) return;

	do {
		attribute_name = list_get_string( attribute_name_list );

		value =
			dictionary_safe_fetch(
				row_dictionary,
				attribute_name );

		if ( first_time )
		{
			first_time = 0;
			fprintf( output_file, "\"%s\"", value );
		}
		else
		{
			fprintf( output_file,
				 ",\"%s\"",
				 value );
		}
	} while( next_item( attribute_name_list ) );

	fprintf( output_file, "\n" );

} /* dictionary_row_output_to_file() */

void dictionary_output_heading_to_file(	FILE *output_file, 
					LIST *attribute_name_list )
{
	char *attribute_name;
	int first_time = 1;

	if ( !list_rewind( attribute_name_list ) ) return;

	do {
		attribute_name = list_get_string( attribute_name_list );

		if ( first_time )
		{
			first_time = 0;
			fprintf( output_file, "\"%s\"", attribute_name );
		}
		else
		{
			fprintf(output_file,
				",\"%s\"",
				attribute_name );
		}
	} while( next_item( attribute_name_list ) );

	fprintf( output_file, "\n" );

} /* dictionary_output_heading_to_file() */

void dictionary_set_indexed_date_time_to_current(
					DICTIONARY *dictionary,
					LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	char date_time_string[ 32 ];
	char key[ 32 ];
	char *data;
	int highest_index;
	int index;

	if ( !list_rewind( attribute_list ) ) return;

	highest_index =
		dictionary_attribute_name_list_get_highest_index(
			dictionary,
			dictionary_get_attribute_name_list(
				attribute_list ),
			(char *)0 /* prepend_folder_name */ );

	if ( highest_index < 1 ) return;

	for ( index = 1; index <= highest_index; index++ )
	{
		list_rewind( attribute_list );

		do {
			attribute = list_get( attribute_list );

			if ( timlib_strcmp(	attribute->datatype,
						"date_time" ) != 0
			&&   timlib_strcmp(	attribute->datatype,
						"current_date_time" ) != 0 )
			{
				continue;
			}

			sprintf(key,
			 	"%s_%d",
			 	attribute->attribute_name,
				index );

			if ( ( data = dictionary_fetch( dictionary, key ) )
			&&     *data
			&&     strcmp( data, "/" ) != 0
			&&     !character_exists( data, ' ' ) )
			{
				sprintf(date_time_string,
					"%s %s",
					data,
					date_get_now_time_second(
						date_get_utc_offset() ) );

				dictionary_set_pointer(
					dictionary,
					key,
					strdup( date_time_string ) );
			}
		} while( list_next( attribute_list ) );

	} /* for( index ) */

} /* dictionary_set_indexed_date_time_to_current() */

void dictionary_set_date_time_to_current(
					DICTIONARY *dictionary,
					LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	char date_time_string[ 32 ];
	char key[ 32 ];
	char *data;

	if ( !list_rewind( attribute_list ) ) return;

	do {
		attribute = list_get( attribute_list );

		if ( timlib_strcmp(	attribute->datatype,
					"date_time" ) != 0
		&&   timlib_strcmp(	attribute->datatype,
					"current_date_time" ) != 0 )
		{
			continue;
		}

		strcpy( key, attribute->attribute_name );

		if ( ( data = dictionary_fetch(
				dictionary,
				key ) )
		&&     *data
		&&     strcmp( data, "/" ) != 0
		&&     !character_exists( data, ' ' ) )
		{
			sprintf(date_time_string,
				"%s %s",
				data,
				date_get_now_time_second(
					date_get_utc_offset() ) );

			dictionary_set_pointer(
				dictionary,
				key,
				strdup( date_time_string ) );
		}

		sprintf(key,
		 	"%s_0",
		 	attribute->attribute_name );

		if ( ( data = dictionary_fetch(
				dictionary,
				key ) )
		&&     *data
		&&     strcmp( data, "/" ) != 0
		&&     !character_exists( data, ' ' ) )
		{
			sprintf(date_time_string,
				"%s %s",
				data,
				date_get_now_time_second(
					date_get_utc_offset() ) );

			dictionary_set_pointer(
				dictionary,
				key,
				strdup( date_time_string ) );
		}

	} while( list_next( attribute_list ) );

} /* dictionary_set_date_time_to_current() */

/* Same code as attribute_get_attribute_name_list() */
/* ------------------------------------------------ */
LIST *dictionary_get_attribute_name_list( LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	LIST *attribute_name_list;

	attribute_name_list = list_new_list();
	if ( list_rewind( attribute_list ) )
		do {
			attribute = list_get_pointer( attribute_list );

			list_append_pointer(
					attribute_name_list,
					attribute->attribute_name );
		} while( list_next( attribute_list ) );
	return attribute_name_list;
} /* dictionary_get_attribute_name_list() */

DICTIONARY *dictionary_get_row_zero_dictionary(
					DICTIONARY *row_dictionary )
{
	DICTIONARY *row_zero_dictionary;
	char *key;
	char *data;
	int index;
	LIST *key_list;

	row_zero_dictionary = dictionary_small_new();

	key_list = dictionary_get_key_list( row_dictionary );

	if ( !list_rewind( key_list ) ) return row_zero_dictionary;

	do {
		key = list_get_pointer( key_list );

		index = get_index( key );

		if ( index ) continue;

		data = dictionary_fetch( row_dictionary, key );

		dictionary_set_pointer(
			row_zero_dictionary,
			key,
			data );

	} while( list_next( key_list ) );

	return row_zero_dictionary;

} /* dictionary_get_row_zero_dictionary() */

LIST *dictionary_data_list_attribute_name_list_merge_dictionary_list(
			LIST *primary_data_list,
			LIST *primary_attribute_name_list,
			char delimiter )
{
	LIST *dictionary_list;
	DICTIONARY *dictionary;
	char *primary_attribute_name;
	char *multi_attribute_data;
	char attribute_data[ 1024 ];
	int i;

	if ( !list_rewind( primary_data_list ) ) return (LIST *)0;
	if ( !list_length( primary_attribute_name_list ) ) return (LIST *)0;

	dictionary_list = list_new();

	do {
		multi_attribute_data = list_get( primary_data_list );

		dictionary = dictionary_small_new();
		i = 0;
		list_rewind( primary_attribute_name_list );

		do {
			primary_attribute_name =
				list_get( primary_attribute_name_list );

			if ( !piece(	attribute_data,
					delimiter,
					multi_attribute_data,
					i ) )
			{
				fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot piece (%d,%c) in %s.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 i,
					 delimiter,
					 multi_attribute_data );
				exit( 1 );
			}

			if ( character_exists(
				attribute_data,
				MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER ) )
			{
				char buffer[ 1024 ];

				piece(	buffer,
					MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER,
					attribute_data,
					0 );

				strcpy( attribute_data, buffer );
			}

			dictionary_set_pointer(
				dictionary,
				primary_attribute_name,
				strdup( attribute_data ) );

			i++;

		} while( list_next( primary_attribute_name_list ) );

		list_append_pointer( dictionary_list, dictionary );

	} while( list_next( primary_data_list ) );

	return dictionary_list;

} /* dictionary_data_list_attribute_name_list_merge_dictionary_list() */

LIST *dictionary_seek_delimited_list(
				DICTIONARY *dictionary,
				char *key )
{
	char *delimited_data;
	char delimiter;

	if ( ! ( delimited_data = dictionary_seek( dictionary, key ) ) )
		return (LIST *)0;

	if ( !*delimited_data ) return (LIST *)0;

	if ( ! ( delimiter = timlib_get_delimiter( delimited_data ) ) )
	{
		/* Could be anything. */
		/* ------------------ */
		delimiter = ';';
	}

	return list_delimiter_string_to_list( delimited_data, delimiter );

} /* dictionary_seek_delimited_list() */

LIST *dictionary_get_non_indexed_key_list(
				DICTIONARY *dictionary )
{
	LIST *non_indexed_key_list;
	LIST *key_list;
	char *key;
	char key_without_index[ 1024 ];

	key_list = dictionary_get_key_list( dictionary );

	if ( !list_rewind( key_list ) ) return (LIST *)0;

	non_indexed_key_list = list_new();

	do {
		key = list_get_pointer( key_list );

		timlib_trim_index( key_without_index, key );

		if ( !list_exists_string(
			non_indexed_key_list,
			key_without_index ) )
		{
			list_append_pointer(
				non_indexed_key_list,
				strdup( key_without_index ) );
		}

	} while( list_next( key_list ) );

	return non_indexed_key_list;

} /* dictionary_get_non_indexed_key_list() */

