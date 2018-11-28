/* $APPASERVER_HOME/library/ajax.h			*/
/* ==================================================== */
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#ifndef AJAX_RELATED_FOLDER_H
#define AJAX_RELATED_FOLDER_H

#include "list.h"
#include "folder.h"
#include "related_folder.h"

typedef struct
{
	FOLDER *display_folder;
	LIST *ajax_mto1_related_folder_list;
	FOLDER *ajax_drop_down_folder;
} AJAX_RELATED_FOLDER;

AJAX_RELATED_FOLDER *ajax_related_folder_new(
				FOLDER *display_folder );

LIST *ajax_get_mto1_related_folder_list(
				LIST *mto1_recursive_related_folder_list );

FOLDER *ajax_get_drop_down_folder(
				LIST *ajax_mto1_related_folder_list );

#endif
