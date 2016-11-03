/* library/horizontal_menu.c				   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "horizontal_menu.h"
#include "dictionary2file.h"
#include "list.h"
#include "piece.h"
#include "folder.h"

HORIZONTAL_MENU *horizontal_menu_new(	LIST *folder_list,
					LIST *process_record_list )
{
	HORIZONTAL_MENU *horizontal_menu;

	if ( ! ( horizontal_menu =
		calloc( 1, sizeof( HORIZONTAL_MENU ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_length( folder_list ) )
	{
		horizontal_menu->lookup_subschema_list =
			horizontal_menu_get_subschema_list(
				folder_list,
				"lookup" );
	
		horizontal_menu->insert_subschema_list =
			horizontal_menu_get_subschema_list(
				folder_list,
				"insert" );
	
		horizontal_menu->lookup_orphan_folder_list =
			horizontal_menu_get_orphan_folder_list(
				folder_list,
				"lookup" );
	
		horizontal_menu->insert_orphan_folder_list =
			horizontal_menu_get_orphan_folder_list(
				folder_list,
				"insert" );
	}

	if ( list_length( process_record_list ) )
	{
		horizontal_menu->process_group_list =
			horizontal_menu_get_process_group_list(
				process_record_list );

		horizontal_menu->orphan_process_name_list =
			horizontal_menu_get_orphan_process_name_list(
				process_record_list );

	}
	return horizontal_menu;

} /* horizontal_menu_new() */

LIST *horizontal_menu_get_subschema_list(
					LIST *folder_list,
					char *state )
{
	FOLDER *folder;
	HORIZONTAL_MENU_SUBSCHEMA *subschema;
	HORIZONTAL_MENU_FOLDER *horizontal_menu_folder;
	LIST *subschema_list = list_new();

	if ( !list_rewind( folder_list ) ) return subschema_list;

	do {
		folder = list_get_pointer( folder_list );

		subschema = (HORIZONTAL_MENU_SUBSCHEMA *)0;

		if ( strcmp( state, "lookup" ) == 0 )
		{
			if ( folder->subschema_name
			&&   *folder->subschema_name )
			{
				subschema =
					horizontal_menu_get_or_set_subschema(
						subschema_list,
						folder->subschema_name );
			}
		}
		else
		if ( strcmp( state, "insert" ) == 0 )
		{
			if ( folder->insert_permission
			&&   folder->subschema_name
			&&   *folder->subschema_name )
			{
				subschema =
					horizontal_menu_get_or_set_subschema(
						subschema_list,
						folder->subschema_name );
			}
		}
		else
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: invalid state = (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				state );
			exit( 1 );
		}

		if ( subschema )
		{
			horizontal_menu_folder =
				horizontal_menu_folder_new(
					folder->folder_name,
					folder->row_count );

			list_append_pointer(
				subschema->horizontal_menu_folder_list,
				horizontal_menu_folder );
		}

	} while( list_next( folder_list ) );

	return subschema_list;
} /* horizontal_menu_get_subschema_list() */

LIST *horizontal_menu_get_process_group_list(
					LIST *process_record_list )
{
	char *process_record;
	char process_name[ 128 ];
	char process_group_name[ 128 ];
	HORIZONTAL_MENU_PROCESS_GROUP *process_group;
	LIST *process_group_list = list_new();

	if ( !list_rewind( process_record_list ) ) return process_group_list;

	do {
		process_record = list_get_pointer( process_record_list );

		piece(	process_name,
			FOLDER_DATA_DELIMITER,
			process_record,
			0 );
		piece(	process_group_name,
			FOLDER_DATA_DELIMITER,
			process_record,
			1 );

		if ( *process_group_name )
		{
			process_group =
				horizontal_menu_get_or_set_process_group(
					process_group_list,
					process_group_name );
			list_append_pointer(
				process_group->process_name_list,
				strdup( process_name ) );
		}

	} while( list_next( process_record_list ) );
	return process_group_list;
} /* horizontal_menu_get_process_group_list() */

LIST *horizontal_menu_get_orphan_folder_list(
					LIST *folder_list,
					char *state )
{
	FOLDER *folder;
	HORIZONTAL_MENU_FOLDER *horizontal_menu_folder;
	LIST *orphan_folder_list = list_new();

	if ( !list_rewind( folder_list ) ) return orphan_folder_list;

	do {
		folder = list_get_pointer( folder_list );

		if ( strcmp( state, "lookup" ) == 0 )
		{
			if ( !folder->subschema_name
			||   !*folder->subschema_name )
			{
				horizontal_menu_folder =
					horizontal_menu_folder_new(
						folder->folder_name,
						folder->row_count );

				list_append_pointer(
					orphan_folder_list,
					horizontal_menu_folder );
			}
		}
		else
		if ( strcmp( state, "insert" ) == 0 )
		{
			if ( folder->insert_permission
			&&   ( !folder->subschema_name
			||     !*folder->subschema_name ) )
			{
				horizontal_menu_folder =
					horizontal_menu_folder_new(
						folder->folder_name,
						folder->row_count );

				list_append_pointer(
					orphan_folder_list,
					horizontal_menu_folder );
			}
		}
		else
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: invalid state = (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				state );
			exit( 1 );
		}

	} while( list_next( folder_list ) );
	return orphan_folder_list;
} /* horizontal_menu_get_orphan_folder_name_list() */

LIST *horizontal_menu_get_orphan_process_name_list(
					LIST *process_record_list )
{
	char *process_record;
	char process_name[ 128 ];
	char process_group_name[ 128 ];
	LIST *orphan_process_name_list = list_new();

	if ( !list_rewind( process_record_list ) )
		return orphan_process_name_list;

	do {
		process_record = list_get_pointer( process_record_list );

		piece(	process_name,
			FOLDER_DATA_DELIMITER,
			process_record,
			0 );
		piece(	process_group_name,
			FOLDER_DATA_DELIMITER,
			process_record,
			1 );

		if ( !*process_group_name )
		{
			list_append_pointer(
				orphan_process_name_list,
				strdup( process_name ) );
		}

	} while( list_next( process_record_list ) );
	return orphan_process_name_list;
} /* horizontal_menu_get_orphan_process_name_list() */

HORIZONTAL_MENU_PROCESS_GROUP *horizontal_menu_get_or_set_process_group(
					LIST *process_group_list,
					char *process_group_name )
{
	HORIZONTAL_MENU_PROCESS_GROUP *process_group;

	if ( !list_rewind( process_group_list ) )
	{
		process_group =
			horizontal_menu_process_group_new(
				strdup( process_group_name ) );

		horizontal_menu_add_process_group(
				process_group_list,
				process_group );

		return process_group;
	}
	
	do {
		process_group = list_get_pointer( process_group_list );

		if ( strcmp(	process_group->process_group_name,
				process_group_name ) == 0 )
		{
			return process_group;
		}
	} while( list_next( process_group_list ) );

	process_group =
		horizontal_menu_process_group_new(
			strdup( process_group_name ) );

	horizontal_menu_add_process_group(
			process_group_list,
			process_group );

	return process_group;
} /* horizontal_menu_get_or_set_process_group() */

HORIZONTAL_MENU_SUBSCHEMA *horizontal_menu_get_or_set_subschema(
					LIST *subschema_list,
					char *subschema_name )
{
	HORIZONTAL_MENU_SUBSCHEMA *subschema;

	if ( !list_rewind( subschema_list ) )
	{
		subschema = horizontal_menu_subschema_new( subschema_name );
		horizontal_menu_add_subschema( subschema_list, subschema );
		return subschema;
	}
	
	do {
		subschema = list_get_pointer( subschema_list );

		if ( strcmp( subschema->subschema_name, subschema_name ) == 0 )
			return subschema;

	} while( list_next( subschema_list ) );
	subschema = horizontal_menu_subschema_new( subschema_name );
	horizontal_menu_add_subschema( subschema_list, subschema );
	return subschema;
} /* horizontal_menu_get_or_set_subschema() */

void horizontal_menu_add_subschema(	LIST *subschema_list,
					HORIZONTAL_MENU_SUBSCHEMA *subschema )
{
	list_add_pointer_in_order(
				subschema_list,
				subschema,
				horizontal_menu_subschema_compare );

} /* horizontal_menu_add_subschema() */

void horizontal_menu_add_process_group(
				LIST *process_group_list,
				HORIZONTAL_MENU_PROCESS_GROUP *process_group )
{
	list_add_pointer_in_order(
				process_group_list,
				process_group,
				horizontal_menu_process_group_compare );

} /* horizontal_menu_add_process_group() */

char *horizontal_menu_subschema_list_display( LIST *subschema_list )
{
	char buffer[ 4096 ];
	char *buffer_pointer = buffer;
	HORIZONTAL_MENU_SUBSCHEMA *subschema;
	int first_time = 1;

	if ( !list_rewind( subschema_list ) ) return "";

	*buffer_pointer = '\0';

	do {
		subschema = list_get_pointer( subschema_list );
		
		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			buffer_pointer +=
				sprintf( buffer_pointer,
					 "," );
		}

		buffer_pointer +=
			sprintf( buffer_pointer,
				 "%s",
				 subschema->subschema_name );
	} while( list_next( subschema_list ) );

	return strdup( buffer );
} /* horizontal_menu_subschema_list_display() */

HORIZONTAL_MENU_PROCESS_GROUP *horizontal_menu_process_group_new(
					char *process_group_name )
{
	HORIZONTAL_MENU_PROCESS_GROUP *process_group;

	process_group = calloc( 1, sizeof( HORIZONTAL_MENU_PROCESS_GROUP ) );
	process_group->process_group_name = process_group_name;
	process_group->process_name_list = list_new();
	return process_group;
}

HORIZONTAL_MENU_SUBSCHEMA *horizontal_menu_subschema_new(
					char *subschema_name )
{
	HORIZONTAL_MENU_SUBSCHEMA *subschema;

	subschema = calloc( 1, sizeof( HORIZONTAL_MENU_SUBSCHEMA ) );
	subschema->subschema_name = subschema_name;
	subschema->horizontal_menu_folder_list = list_new();
	return subschema;
}

HORIZONTAL_MENU_FOLDER *horizontal_menu_folder_new(
					char *folder_name,
					long row_count )
{
	HORIZONTAL_MENU_FOLDER *folder;

	folder = calloc( 1, sizeof( HORIZONTAL_MENU_FOLDER ) );
	folder->folder_name = folder_name;
	folder->row_count = row_count;
	return folder;
}

int horizontal_menu_subschema_compare(
			HORIZONTAL_MENU_SUBSCHEMA *subschema1,
			HORIZONTAL_MENU_SUBSCHEMA *subschema2 )
{
	return strcmp(	subschema1->subschema_name,
			subschema2->subschema_name );
}

int horizontal_menu_process_group_compare(
			HORIZONTAL_MENU_PROCESS_GROUP *process_group1,
			HORIZONTAL_MENU_PROCESS_GROUP *process_group2 )
{
	return strcmp(	process_group1->process_group_name,
			process_group2->process_group_name );
}

