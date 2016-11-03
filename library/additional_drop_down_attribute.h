/* library/additional_drop_down_attribute.h				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver role ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ADDITIONAL_DROP_DOWN_ATTRIBUTE_H
#define ADDITIONAL_DROP_DOWN_ATTRIBUTE_H

#include "boolean.h"
#include "list.h"

typedef struct
{
	char *folder_name;
	char *attribute_name;
	boolean prepend;
} ADDITIONAL_USER_DROP_DOWN_ATTRIBUTE;

typedef struct
{
	char *application_name;
	char *login_name;
	char *drop_down_folder_name;
	char *folder_name;
	char *attribute_name;
	LIST *additional_user_drop_down_attribute_list;
} ADDITIONAL_DROP_DOWN_ATTRIBUTE;

ADDITIONAL_DROP_DOWN_ATTRIBUTE *additional_drop_down_attribute_new(
			char *application_name,
			char *login_name,
			char *drop_down_folder_name );

ADDITIONAL_USER_DROP_DOWN_ATTRIBUTE *additional_user_drop_down_attribute_new(
			char *folder_name,
			char *attribute_name );

void additional_drop_down_attribute_populate_folder_attribute(
			char **folder_name,
			char **attribute_name,
			char *application_name,
			char *drop_down_folder_name );

LIST *additional_drop_down_attribute_get_user_list(
			char *application_name,
			char *drop_down_folder_name,
			char *login_name );

boolean additional_drop_down_exists_attribute(
			LIST *user_list,
			char *attribute_name );

void additional_drop_down_attribute_get_select_clause(
			char *select_clause,
			char **first_attribute_name,
			char *application_name,
			char *constant_select,
			char *select_folder_name,
			ADDITIONAL_DROP_DOWN_ATTRIBUTE *
				additional_drop_down_attribute );
#endif
