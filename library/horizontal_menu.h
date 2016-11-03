/* library/horizontal_menu.h				   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef HORIZONTAL_MENU_H
#define HORIZONTAL_MENU_H

#include "list.h"
#include "process.h"

typedef struct
{
	char *folder_name;
	long row_count;
} HORIZONTAL_MENU_FOLDER;

typedef struct
{
	char *subschema_name;
	LIST *horizontal_menu_folder_list;
} HORIZONTAL_MENU_SUBSCHEMA;

typedef struct
{
	char *process_group_name;
	LIST *process_name_list;
} HORIZONTAL_MENU_PROCESS_GROUP;

typedef struct
{
	LIST *lookup_subschema_list;
	LIST *insert_subschema_list;
	LIST *process_group_list;
	LIST *lookup_orphan_folder_list;
	LIST *insert_orphan_folder_list;
	LIST *orphan_process_name_list;
} HORIZONTAL_MENU;

HORIZONTAL_MENU *horizontal_menu_new(	LIST *folder_list,
					LIST *process_record_list );

HORIZONTAL_MENU_SUBSCHEMA *horizontal_menu_subschema_new(
					char *subschema_name );

HORIZONTAL_MENU_FOLDER *horizontal_menu_folder_new(
					char *folder_name,
					long count );

HORIZONTAL_MENU_SUBSCHEMA *horizontal_menu_get_or_set_subschema(
					LIST *subschema_list,
					char *subschema_name );

HORIZONTAL_MENU_PROCESS_GROUP *horizontal_menu_process_group_new(
					char *process_group_name );

HORIZONTAL_MENU_PROCESS_GROUP *horizontal_menu_get_or_set_process_group(
					LIST *process_group_list,
					char *process_group_name );

LIST *horizontal_menu_get_subschema_list(
					LIST *folder_list,
					char *state );

LIST *horizontal_menu_get_process_group_list(
					LIST *process_record_list );

LIST *horizontal_menu_get_orphan_folder_list(
					LIST *folder_list,
					char *state );

LIST *horizontal_menu_get_orphan_process_name_list(
					LIST *process_record_list );

void horizontal_menu_add_process_group(
				LIST *process_group_list,
				HORIZONTAL_MENU_PROCESS_GROUP *process_groups );

void horizontal_menu_add_subschema(	LIST *subschema_list,
					HORIZONTAL_MENU_SUBSCHEMA *subschema );

int horizontal_menu_subschema_compare(	HORIZONTAL_MENU_SUBSCHEMA *subschema1,
					HORIZONTAL_MENU_SUBSCHEMA *subschema2 );

int horizontal_menu_process_group_compare(
				HORIZONTAL_MENU_PROCESS_GROUP *process_group1,
				HORIZONTAL_MENU_PROCESS_GROUP *process_group2 );

char *horizontal_menu_subschema_list_display(
					LIST *subschema_list );
#endif
