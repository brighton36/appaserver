/* library/pair_one2m.h					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef PAIR_ONE2M_H
#define PAIR_ONE2M_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"
#include "folder.h"
#include "dictionary.h"

/* Constants */
/* --------- */
#define PAIR_ONE2M_SUBMIT_FOLDER		"pair_one2m_submit_folder"
#define PAIR_ONE2M_DUPLICATE_STATE_KEY		"pair_one2m_duplicate_state"
#define PAIR_ONE2M_FOLDER			"pair_one2m_folder"
#define PAIR_ONE2M_FOLDER_LIST			"pair_one2m_folder_list"
#define PAIR_ONE2M_OMIT				"pair_one2m_omit"

/* Structures */
/* ---------- */
typedef struct
{
	boolean is_participating;
	boolean insert_is_completed;
	boolean submit_button_on_top_frame;
	boolean continuing_series;
	char *next_folder_name;
	char *specified_folder_name;
	FOLDER *pair_insert_folder;
	boolean inserted_duplicate;
	LIST *pair_one2m_related_folder_list;
	LIST *pair_one2m_folder_name_list;
	LIST *pair_inserted_folder_name_list;
} PAIR_ONE2M;

/* Prototypes */
/* ---------- */
char *pair_one2m_get_pair_one2m_submit_element_name(
				boolean with_suffix_zero );

PAIR_ONE2M *pair_one2m_new(
			LIST *ignore_attribute_name_list	/* out only */,
			DICTIONARY *pair_one2m_dictionary	/* in/out   */,
			char *application_name			/* in only  */,
			LIST *pair_one2m_related_folder_list	/* in only  */,
			LIST *posted_attribute_name_list	/* in only  */,
			ROLE *role				/* in only  */,
			char *session				/* in only  */);

LIST *pair_one2m_get_pair_inserted_folder_name_list(
				DICTIONARY *pair_one2m_dictionary );

boolean pair_one2m_get_inserted_duplicate(
				DICTIONARY *pair_one2m_dictionary );

char *pair_one2m_get_uninserted_pair_one2m_related_folder_name(
				LIST *pair_inserted_folder_name_list,
				LIST *related_folder_list );

char *pair_one2m_get_uninserted_pair_mto1_related_folder_name(
				LIST *pair_inserted_folder_name_list,
				LIST *related_folder_list );

char *pair_one2m_get_next_folder_name(
				LIST *pair_inserted_folder_name_list,
				LIST *pair_one2m_related_folder_list );

#endif
