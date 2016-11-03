/* role_folder.h 							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver role_folder ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ROLE_FOLDER_H
#define ROLE_FOLDER_H

#include "list.h"
#include "folder.h"

typedef struct
{
	char *application_name;
	char *session;
	char *role_name;
	char *folder_name;
	char insert_yn;
	char update_yn;
	char lookup_yn;
	char delete_yn;
} ROLE_FOLDER;

LIST *new_role_folder_list(		char *session,
					char *application_name,
					char *role_name );
ROLE_FOLDER *role_folder_new_role_folder( 
					char *application_name,
					char *session,
					char *role_name,
					char *folder_name );

void role_folder_load(		 	char *insert_yn,
					char *update_yn,
					char *lookup_yn,
					char *delete_yn,
					char *application_name,
					char *session,
					char *role_name,
					char *folder_name );

LIST *role_folder_get_insert_list(	char *application_name,
					char *session,
					char *role_name );
LIST *role_folder_get_update_list(	char *application_name,
					char *session,
					char *role_name );
LIST *role_folder_get_lookup_list(	char *application_name,
					char *session,
					char *role_name );
char *role_folder_display(		ROLE_FOLDER *role_folder );
void role_folder_populate_folder_insert_permission(
					LIST *folder_list, 
					LIST *role_folder_insert_list,
					char *application_name );
void role_folder_populate_folder_update_permission(
					LIST *folder_list, 
					LIST *role_folder_update_list,
					char *application_name );
void role_folder_populate_folder_lookup_permission(
					LIST *folder_list, 
					LIST *role_folder_lookup_list,
					char *application_name );
ROLE_FOLDER *role_folder_new(
					char *application_name,
					char *session,
					char *role_name,
					char *folder_name );

boolean role_folder_viewonly(		ROLE_FOLDER *role_folder );
#endif
