/* library/prompt_recursive.h						*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PROMPT_RECURSIVE_H
#define PROMPT_RECURSIVE_H

/* Includes */
/* -------- */
#include "list.h"
#include "folder.h"
#include "related_folder.h"

/* Constants */
/* --------- */

/* Data structures */
/* --------------- */
typedef struct
{
	FOLDER *folder;
	int recursive_level;
} PROMPT_RECURSIVE_MTO1_FOLDER;

typedef struct
{
	FOLDER *prompt_folder;
	boolean prompt_folder_drop_down_multi_select;
	LIST *prompt_recursive_mto1_folder_list;
} PROMPT_RECURSIVE_FOLDER;

typedef struct
{
	char *query_folder_name;
	LIST *prompt_recursive_folder_list;
} PROMPT_RECURSIVE;

/* Prototypes */
/* ---------- */
PROMPT_RECURSIVE *prompt_recursive_new(
				char *application_name,
				char *query_folder_name,
				LIST *mto1_related_folder_list );

LIST *prompt_recursive_get_prompt_recursive_folder_list(
				char *application_name,
				LIST *mto1_related_folder_list );

PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder_new(
				char *application_name,
				FOLDER *prompt_folder,
				boolean prompt_folder_drop_down_multi_select );

LIST *prompt_recursive_get_mto1_folder_list(
				char *application_name,
				char *prompt_folder_name,
				LIST *primary_attribute_name_list );

PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder_new(
				FOLDER *folder,
				int recursive_level );

char *prompt_recursive_display(	PROMPT_RECURSIVE *prompt_recursive );

char *prompt_recursive_get_gray_drop_downs_javascript(
			FOLDER *prompt_folder,
			boolean prompt_folder_drop_down_multi_select,
			LIST *prompt_recursive_mto1_folder_list );

char *prompt_recursive_get_grandchild_query_folder_name(
				PROMPT_RECURSIVE *prompt_recursive,
				DICTIONARY *query_dictionary );

boolean prompt_recursive_folder_is_queried(
				LIST *primary_attribute_name_list,
				DICTIONARY *query_dictionary );

#endif
