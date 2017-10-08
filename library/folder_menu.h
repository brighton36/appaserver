/* library/folder_menu.h				   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef FOLDER_MENU_H
#define FOLDER_MENU_H

#include "boolean.h"

#define FOLDER_MENU_PERMISSIONS "insert,update,lookup"

LIST *folder_menu_get_choose_folder_list(
					char *application_name,
					char *appaserver_data_directory,
					char *session,
					char *role_name,
					boolean with_count );

char *folder_menu_get_input_pipe_sys_string(
					char *application_name,
					char *session,
					char *appaserver_data_directory,
					char *role_name );

char *folder_menu_get_filename(
					char *application_name,
					char *session,
					char *appaserver_data_directory,
					char *role_name );

void folder_menu_create_spool_file(
					char *application_name,
					char *session,
					char *appaserver_data_directory,
					char *role_name,
					boolean with_count );

boolean folder_menu_file_exists(
					char *application_name,
					char *session,
					char *appaserver_data_directory,
					char *role_name );

void folder_menu_remove_file(
					char *application_name,
					char *session,
					char *appaserver_data_directory,
					char *role_name );

#endif
