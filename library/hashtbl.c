/* library/hashtbl.c					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include "hashtbl.h"
#include "timlib.h"

int hash_table_free_container( HASH_TABLE *h )
{
	int i;

	for( i = 0; i < h->cell_array_size; i++ )
		if ( h->cell_array[ i ] )
			list_free_container( h->cell_array[ i ] );

	free( h );
	return 1;

} /* hash_table_free_container() */

char *hash_table_get_string( HASH_TABLE *h, char *key )
{
	return (char *)hash_table_get( h, key );
}

void *hash_table_fetch_pointer( HASH_TABLE *h, char *key )
{
	return hash_table_get( h, key );
}

void *hash_table_fetch( HASH_TABLE *h, char *key )
{
	return hash_table_get( h, key );
}

void *hash_table_get_pointer( HASH_TABLE *h, char *key )
{
	return hash_table_get( h, key );
}

void *hash_table_seek( HASH_TABLE *h, char *key )
{
	return hash_table_get( h, key );
}

void *hash_table_get(	HASH_TABLE *h, 
			char *key )
{
	int duplicate_indicator;
	return hash_table_retrieve_other_data(	
				h, 
				key, 
				&duplicate_indicator );
}

int hash_table_length( HASH_TABLE *h )
{
	return hash_table_len( h );
}

int hash_table_len( HASH_TABLE *h )
{
	int i;
	int length = 0;
	HASH_TABLE_CELL *cell;

	if ( !h ) return 0;

	for( i = 0; i < h->cell_array_size; i++ )
	{
		if ( h->cell_array[ i ] )
		{
			if ( list_rewind( h->cell_array[ i ] ) )
			{
				do {
					cell = list_get_pointer(
							h->cell_array[ i ] );
					if ( cell ) length++;
				} while( list_next( h->cell_array[ i ] ) );
			}

			/* length += list_count( h->cell_array[ i ] ); */
		}
	}
	return length;
} /* hash_table_len() */

HASH_TABLE *hash_table_new_hash_table( enum hash_table_size hash_table_size )
{
	return hash_table_init( hash_table_size );
}

HASH_TABLE *hash_table_new( enum hash_table_size hash_table_size )
{
	return hash_table_init( hash_table_size );
}

HASH_TABLE *hash_table_init( enum hash_table_size hash_table_size )
{
	HASH_TABLE *h;

	h = (HASH_TABLE *)calloc( 1, sizeof( HASH_TABLE ) );
	if ( !h )
	{
		fprintf( stderr, 
			 "%s/%s(): Memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	if ( hash_table_size == hash_table_small )
		h->cell_array_size = HASH_TABLE_SMALL;
	else
	if ( hash_table_size == hash_table_medium )
		h->cell_array_size = HASH_TABLE_MEDIUM;
	else
	if ( hash_table_size == hash_table_large )
		h->cell_array_size = HASH_TABLE_LARGE;
	else
	if ( hash_table_size == hash_table_huge )
		h->cell_array_size = HASH_TABLE_HUGE;
	else
	if ( hash_table_size == hash_table_super )
		h->cell_array_size = HASH_TABLE_SUPER;
	else
	if ( hash_table_size == hash_table_duper )
		h->cell_array_size = HASH_TABLE_DUPER;
	else
		h->cell_array_size = HASH_TABLE_MEDIUM;

	h->cell_array =
		(LIST **)calloc(	h->cell_array_size,
					sizeof( LIST * ) );
	if ( !h->cell_array )
	{
		fprintf( stderr, 
			 "%s/%s(): Memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	return h;

} /* hash_table_init() */

int hash_table_all_null_data( HASH_TABLE *h )
{
	int i;
	HASH_TABLE_CELL *cell;
	LIST *list;

	for( i = 0; i < h->cell_array_size; i++ )
		if ( h->cell_array[ i ] )
		{
			list = h->cell_array[ i ];
			list_rewind( list );
			do {
				cell = (HASH_TABLE_CELL *)list_get( list );
				if ( cell->other_data )
					return 0;
			} while( next_item( list ) );
		}
	return 1;
} /* hash_table_all_null_data() */


int hash_table_set_pointer( HASH_TABLE *h, char *key, void *other_data )
{
	return hash_table_insert( h, key, other_data );
}

int hash_table_add_pointer( HASH_TABLE *h, char *key, void *other_data )
{
	return hash_table_insert( h, key, other_data );
}

int hash_table_insert_back( HASH_TABLE *h, char *key, void *other_data )
{
	char *other_data_indicator;
	int duplicate_indicator;

	/* Make sure key already exists in the table */
	/* ----------------------------------------- */
	other_data_indicator = 
		hash_table_retrieve_other_data(	
			h, 
			key, 
			&duplicate_indicator );

	/* If key doesn't exist in hash table */
	/* ---------------------------------- */
	if ( !other_data_indicator )
	{
		fprintf( stderr, 
			 "%s/%s(): key not found: (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 key );
		exit( 1 );
	}

	/* Set the new value */
	/* ----------------- */
	h->last_cell_retrieved->other_data = other_data;

	return 1;

} /* hash_table_insert_back() */

int hash_table_set_string( HASH_TABLE *h, char *key, char *s )
{
	/* return hash_table_set( h, key, s ); */
	return hash_table_set( h, key, strdup( s ) );
}

int hash_table_set( HASH_TABLE *h, char *key, void *other_data )
{
	char *other_data_indicator;
	int duplicate_indicator;

	other_data_indicator = 
		hash_table_retrieve_other_data(	
			h, 
			key, 
			&duplicate_indicator );

	/* If key doesn't exist in hash table */
	/* ---------------------------------- */
	if ( !other_data_indicator )
	{
		return hash_table_insert( h, key, other_data );
	}

/*
	if ( h->last_cell_retrieved->other_data )
		free( h->last_cell_retrieved->other_data );
*/

	/* Set the new value */
	/* ----------------- */
	h->last_cell_retrieved->other_data = other_data;

	return 1;

} /* hash_table_set() */

int hash_table_set_unique_key( HASH_TABLE *h, char *key, void *other_data )
{
	unsigned int cell_array_offset;
	HASH_TABLE_CELL cell;
	LIST *list;

	cell_array_offset =
		hash_table_get_hash_value( key, h->cell_array_size );
	cell.key = key;
	cell.other_data = other_data;
	cell.tried_to_insert_duplicate_key = 0;

	/* Dereference it for simplicity */
	/* ----------------------------- */
	list = h->cell_array[ cell_array_offset ];

	/* If need a new list */
	/* ------------------ */
	if ( !list )
	{
		list = create_list();
		h->cell_array[ cell_array_offset ] = list;
	}

	/* Append cell item to list */
	/* ------------------------ */
	if ( !add_item( list,
			&cell,
			sizeof ( HASH_TABLE_CELL ),
			ADD_TAIL ) )
	{
		fprintf( stderr,
			 "%s/%s(): Add to list failed.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}


	return 1;

} /* hash_table_set_unique_key() */

int hash_table_insert( HASH_TABLE *h, char *key, void *other_data )
{
	unsigned int cell_array_offset;
	HASH_TABLE_CELL cell;
	LIST *list;
	char *other_data_indicator;
	int duplicate_indicator;

	/* First see if key already exists in the table */
	/* -------------------------------------------- */
	other_data_indicator = 
		hash_table_retrieve_other_data(	
			h, 
			key, 
			&duplicate_indicator );

	/* If key already exists in hash table */
	/* ----------------------------------- */
	if ( other_data_indicator )
	{
		h->last_cell_retrieved->tried_to_insert_duplicate_key = 1;
		return 1;
	}

	/* Set other elements */
	/* ------------------ */
	cell_array_offset =
		hash_table_get_hash_value( key, h->cell_array_size );
	cell.key = key;
	cell.other_data = other_data;
	cell.tried_to_insert_duplicate_key = 0;

	/* Dereference it for simplicity */
	/* ----------------------------- */
	list = h->cell_array[ cell_array_offset ];

	/* If need a new list */
	/* ------------------ */
	if ( !list )
	{
		list = create_list();
		h->cell_array[ cell_array_offset ] = list;
	}

	/* Append cell item to list */
	/* ------------------------ */
	if ( !add_item( list,
			&cell,
			sizeof ( HASH_TABLE_CELL ),
			ADD_TAIL ) )
	{
		fprintf( stderr,
			 "%s/%s(): Add to list failed.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	return 1;

} /* hash_table_insert() */

unsigned int hash_table_get_hash_value( char *key, int cell_array_size )
{
	unsigned int hash_value = 0;

	while( *key )
		hash_value = ( hash_value << 5 ) + tolower( *key++ );

	return hash_value % cell_array_size;

} /* hash_table_get_hash_value() */


int compare_key( HASH_TABLE_CELL *cell_ptr, char *key )
{
	return strcasecmp( cell_ptr->key, key );
}


void *hash_table_retrieve_other_data(	HASH_TABLE *h, 
					char *key, 
					int *duplicate_indicator )
{
	unsigned int cell_array_offset;
	HASH_TABLE_CELL *cell_ptr;
	LIST *list;

	if ( !h ) return (void *)0;

	cell_array_offset =
		hash_table_get_hash_value( key, h->cell_array_size );

	/* Zap duplicate_indicator */
	/* ----------------------- */
	*duplicate_indicator = 0;

	/* Dereference it for simplicity */
	/* ----------------------------- */
	list = h->cell_array[ cell_array_offset ];

	/* If no list then not found */
	/* ------------------------- */
	if ( !list ) return (void *)0;

	/* Check list */
	/* ---------- */
        if ( !go_head( list ) ) return (void *)0;
        do {
                cell_ptr = (HASH_TABLE_CELL *) retrieve_item_ptr( list );

                if ( cell_ptr && strcasecmp( cell_ptr->key, key ) == 0 )
		{
			/* Set the last cell retrieved */
			/* --------------------------- */
			h->last_cell_retrieved = cell_ptr;

			*duplicate_indicator = 
				cell_ptr->tried_to_insert_duplicate_key;
			return cell_ptr->other_data;
		}

	} while ( next_item( list ) );
	return (void *)0;
} /* hash_table_retrieve_other_data() */

int hash_table_free( HASH_TABLE *h )
{
	int i;

	for( i = 0; i < h->cell_array_size; i++ )
		if ( h->cell_array[ i ] )
			destroy_list( h->cell_array[ i ] );

	free( h );

	return 1;

} /* hash_table_free() */


char *hash_table_index_zero_display_delimiter(
					char *destination, 
					HASH_TABLE *h,
					int delimiter )
{
	char *destination_ptr = destination;
	int i;
	LIST *list;
	HASH_TABLE_CELL *cell_ptr;
	int first_time = 1;
	char data_to_output[ 4096 ];
	char buffer[ 4096 ];

	*destination_ptr = '\0';

	for ( i = 0; i < h->cell_array_size; i++ )
	{
		if ( h->cell_array[ i ] )
		{
			list = h->cell_array[ i ];
			go_head( list );
			do
			{
				cell_ptr = (HASH_TABLE_CELL *)
						retrieve_item_ptr( list );

				if ( !cell_ptr ) continue;

				if ( get_index( cell_ptr->key ) ) continue;

				strcpy( buffer, cell_ptr->other_data );

				escape_character(	data_to_output,
							buffer,
							delimiter );

				strcpy( buffer, data_to_output );

				escape_character(	data_to_output,
							buffer,
							'=' );

				if ( first_time )
				{
					first_time = 0;
					
					destination_ptr += sprintf( 
						destination_ptr, 
						"%s=%s", 
						cell_ptr->key,
						data_to_output );
				}
				else
				{
					destination_ptr += sprintf( 
						destination_ptr, 
						"%c%s=%s", 
						delimiter,
						cell_ptr->key,
						data_to_output );
				}
			} while( next_item( list ) );
		}
	}
	return destination;
} /* hash_table_index_zero_display_delimiter() */

char *hash_table_display_delimiter( 	char *destination, 
					HASH_TABLE *h,
					int delimiter )
{
	char *destination_ptr = destination;
	int i;
	LIST *list;
	HASH_TABLE_CELL *cell_ptr;
	int first_time = 1;
	char data_to_output[ 4096 ];
	char buffer[ 4096 ];

	*destination_ptr = '\0';

	for ( i = 0; i < h->cell_array_size; i++ )
	{
		if ( h->cell_array[ i ] )
		{
			list = h->cell_array[ i ];
			go_head( list );
			do
			{
				cell_ptr = (HASH_TABLE_CELL *)
						retrieve_item_ptr( list );

				if ( !cell_ptr ) continue;
				if ( !cell_ptr->other_data ) continue;

				strcpy( buffer, cell_ptr->other_data );

				escape_character(	data_to_output,
							buffer,
							delimiter );

				strcpy( buffer, data_to_output );

				escape_character(	data_to_output,
							buffer,
							'=' );

				if ( first_time )
				{
					first_time = 0;
					
					destination_ptr += sprintf( 
						destination_ptr, 
						"%s=%s", 
						cell_ptr->key,
						data_to_output );
				}
				else
				{
					destination_ptr += sprintf( 
						destination_ptr, 
						"%c%s=%s", 
						delimiter,
						cell_ptr->key,
						data_to_output );
				}
			} while( next_item( list ) );
		}
	}

	return destination;

} /* hash_table_display_delimiter() */

char *hash_table_display( char *destination, HASH_TABLE *h )
{
	return hash_table_display_delimiter( destination, h, ',' );
}



void hash_table_display_key_list( HASH_TABLE *h )
{
	int i;
	LIST *list;
	HASH_TABLE_CELL *cell_ptr;
	boolean first_time = 1;

	for ( i = 0; i < h->cell_array_size; i++ )
	{
		if ( h->cell_array[ i ] )
		{
			list = h->cell_array[ i ];
			go_head( list );
			do
			{
				cell_ptr = (HASH_TABLE_CELL *)
						retrieve_item_ptr( list );

				if ( cell_ptr )
				{
					if ( first_time )
					{
						fprintf(stderr,
							"%s", 
							cell_ptr->key );
						first_time = 0;
					}
					else
					{
						fprintf(stderr,
							"^%s", 
							cell_ptr->key );
					}
				}
			} while( next_item( list ) );
		}
	}
	fprintf( stderr, "\n" );

} /* hash_table_display_key_list() */

void display_hash_table( HASH_TABLE *h, char *output_file )
{
	int i;
	LIST *list;
	HASH_TABLE_CELL *cell_ptr;
	FILE *output_fp;

	if ( strcmp( output_file, "stdout" ) == 0 )
		output_fp = stdout;
	else
	if ( strcmp( output_file, "stderr" ) == 0 )
		output_fp = stderr;
	else
	{
		output_fp = fopen( output_file, "w" );
		if ( !output_fp )
		{
			fprintf(stderr,
				"%s/%s() cannot open %s for write.\n",
				__FILE__,
				__FUNCTION__,
				output_file );
			exit( 1 );
		}
	}

	for ( i = 0; i < h->cell_array_size; i++ )
	{
		if ( h->cell_array[ i ] )
		{
			list = h->cell_array[ i ];
			go_head( list );
			do
			{
				cell_ptr = (HASH_TABLE_CELL *)
						retrieve_item_ptr( list );

				if ( cell_ptr )
				{
					fprintf( output_fp, "%s|%s\n", 
						cell_ptr->key,	
						(char *)cell_ptr->other_data );
				}
			} while( next_item( list ) );
		}
	}

	if ( output_fp != stdout && output_fp != stderr )
	{
		fclose( output_fp );
	}

} /* display_hash_table() */

LIST *hash_table_get_key_list( HASH_TABLE *h )
{
	return get_hashtbl_key_list( h );
}

LIST *get_hashtbl_key_list( HASH_TABLE *h )
{
	LIST *list;
	LIST *return_list = list_new();
	HASH_TABLE_CELL *cell_ptr;
	int i;

	for ( i = 0; i < h->cell_array_size; i++ )
	{
		if ( h->cell_array[ i ] )
		{
			list = h->cell_array[ i ];
			go_head( list );
			do
			{
				cell_ptr = (HASH_TABLE_CELL *)
						retrieve_item_ptr( list );

				if ( !cell_ptr ) continue;

				list_append_pointer(return_list,
						    cell_ptr->key );
			} while( next_item( list ) );
		}
	}
	return return_list;
} /* get_hashtbl_key_list() */

LIST *hash_table_get_ordered_key_list( HASH_TABLE *h )
{
	LIST *list;
	LIST *return_list = list_new();
	HASH_TABLE_CELL *cell_ptr;
	int i;

	for ( i = 0; i < h->cell_array_size; i++ )
	{
		if ( h->cell_array[ i ] )
		{
			list = h->cell_array[ i ];
			go_head( list );
			do
			{
				cell_ptr = (HASH_TABLE_CELL *)
						retrieve_item_ptr( list );

				if ( !cell_ptr ) continue;

				list_add_pointer_in_order(
							return_list,
						    	cell_ptr->key,
							strcasecmp );
			} while( next_item( list ) );
		}
	}
	return return_list;
} /* hash_table_get_ordered_key_list() */

boolean hash_table_exists_key( HASH_TABLE *h, char *key )
{
	int duplicate_indicator;
	return (boolean)hash_table_retrieve_other_data(	
						h, 
						key, 
						&duplicate_indicator );
}

boolean hash_table_exists( HASH_TABLE *h, char *key )
{
	int duplicate_indicator;
	return (boolean)hash_table_retrieve_other_data(	
						h, 
						key, 
						&duplicate_indicator );
}

int hash_table_remove_data( HASH_TABLE *h, char *key )
{
	return hash_table_remove( h, key );
}

int hash_table_remove(	HASH_TABLE *h, char *key )
{
	unsigned int cell_array_offset;
	HASH_TABLE_CELL *cell_ptr;
	LIST *list;

	cell_array_offset =
		hash_table_get_hash_value( key, h->cell_array_size );

	list = h->cell_array[ cell_array_offset ];

	if ( !list || !go_head( list ) ) return 0;

        do {
                cell_ptr = (HASH_TABLE_CELL *) retrieve_item_ptr( list );

                if ( cell_ptr && strcasecmp( cell_ptr->key, key ) == 0 )
		{
			list_set_current( list, (char *)0 );
			return 1;
		}

	} while ( next_item( list ) );
	return 0;
} /* hash_table_remove() */

