/* delete_database.h							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver delete_database ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef DELETE_DATABASE_H
#define DELETE_DATABASE_H

#include <stdio.h>
#include "list.h"
#include "dictionary.h"
#include "process.h"
#include "related_folder.h"

/* Constants */
/* --------- */
#define DELETE_DATABASE_DATA_DELIMITER	'^'

/* Objects */
/* ------- */
typedef struct
{
	char *attribute_name;
	char *data;
} DELETE_ROW;

typedef struct
{
	char *folder_name;
	char *related_attribute_name;
	LIST *delete_row_list;
	PROCESS *post_change_process;
	boolean null_1tom_upon_delete;
} DELETE_FOLDER;

typedef struct
{
	char *application_name;
	char *session;
	char *login_name;
	char *role_name;
	LIST *where_data_list;
	FOLDER *folder;
	LIST *delete_folder_list;
	boolean delete_isa_only;
} DELETE_DATABASE;

DELETE_DATABASE *delete_database_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *where_data_carrot_list_string,
			boolean delete_isa_only );

LIST *delete_database_get_delete_folder_list(
			char *folder_name,
			char *post_change_process,
			LIST *attribute_list,
			LIST *one2m_related_folder_list,
			LIST *one2m_isa_related_folder_list,
			LIST *primary_attribute_name_list,
			boolean delete_isa_only,
			LIST *where_data_list );

DELETE_FOLDER *delete_database_delete_folder_new(
			char *folder_name,
			char *related_attribute_name,
			PROCESS *post_change_process );

DELETE_ROW *delete_database_delete_row_new(
			char *where_attribute_name,
			char *where_data );

void delete_database_execute_delete_folder_list(
			char *application_name,
			LIST *delete_folder_list,
			char *sql_executable );

void delete_database_execute_delete_row(
			char *application_name,
			char *folder_name,
			LIST *delete_row_list,
			char *sql_executable,
			PROCESS *post_change_process );

char *delete_database_get_where_data_delimited_string(
			LIST *delete_row_list,
			char delimiter,
			boolean get_where_attribute );

LIST *delete_database_get_delete_row_list(
			LIST *where_attribute_name_list,
			LIST *where_data_list,
			LIST *attribute_list );

void delete_database_populate_with_one2m_related_folder_list(
			LIST *delete_folder_list,
			LIST *one2m_related_folder_list,
			LIST *primary_attribute_name_list,
			LIST *where_data_list );

void delete_database_populate_with_mto1_isa_related_folder_list(
			LIST *delete_folder_list,
			LIST *mto1_isa_related_folder_list,
			LIST *where_data_list );

void delete_database_execute_update_to_null_row(
			char *application_name,
			char *folder_name,
			LIST *delete_row_list,
			LIST *primary_attribute_name_list,
			char *sql_executable );

void delete_database_append_delete_folder_list(
			LIST *delete_folder_list,
			DELETE_FOLDER *delete_folder );

boolean delete_folder_exists(
			LIST *delete_folder_list,
			DELETE_FOLDER *delete_folder );

void delete_database_convert_parameters(
			char **executable,
			char *application_name,
			LIST *delete_row_list );

char *delete_database_display(
			DELETE_DATABASE *delete_database );

#endif

