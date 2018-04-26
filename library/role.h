/* library/role.h							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver role ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ROLE_H
#define ROLE_H

#include "boolean.h"

#define FOLDER_COUNT_YN_PIECE		0

typedef struct
{
	char *attribute_name;
	char *permission;
} ROLE_ATTRIBUTE_EXCLUDE;

typedef struct
{
	char *application_name;
	char *role_name;
	char folder_count_yn;
	char override_row_restrictions_yn;
	char grace_no_cycle_colors_yn;
	LIST *attribute_exclude_list;
} ROLE;

ROLE *role_new( 			char *application_name,
					char *role_name );

ROLE *role_new_role( 			char *application_name,
					char *role_name );

boolean role_fetch( 			char *folder_count_yn,
					char *override_row_restrictions,
					char *grace_no_cycle_colors_yn,
					char *application_name,
					char *role_name );

boolean role_get_override_row_restrictions(
				char override_row_restrictions_yn );

boolean role_get_exists_folder_count_y(
					char *application_name,
					char *login_name );

void role_free(				ROLE *role );

ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude_new(
					char *attribute_name,
					char *permission );

LIST *role_get_attribute_exclude_list(	char *application_name,
					char *role_name );

boolean role_exists_attribute_exclude_insert(
					LIST *attribute_exclude_list,
					char *attribute_name );

#endif
