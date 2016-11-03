/* library/delete_database.h						*/
/* -------------------------------------------------------------------- */
/* This is the appaserver delete_database ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef DELETE_DATABASE_H
#define DELETE_DATABASE_H

#include <stdio.h>
#include "list.h"
#include "boolean.h"
#include "process.h"
#include "related_folder.h"

/* Constants */
/* --------- */
#define DELETE_DATABASE_DATA_DELIMITER	'^'

/* Objects */
/* ------- */
typedef struct
{
	LIST *primary_attribute_data_list;
} DELETE_ROW;

typedef struct
{
	FOLDER *folder;
	LIST *primary_attribute_name_list;
	boolean null_1tom_upon_delete;
	LIST *delete_row_list;
	LIST *update_null_attribute_name_list;
} DELETE_FOLDER;

typedef struct
{
	char *application_name;
	boolean dont_delete_mto1_isa;
	LIST *delete_folder_list;
	char *root_folder_name;
	LIST *root_primary_attribute_data_list;
} DELETE_DATABASE;

DELETE_DATABASE *delete_database_new(
			char *application_name,
			boolean dont_delete_mto1_isa,
			char *root_folder_name,
			char *root_primary_attribute_data_list_string,
			char delimiter );

LIST *delete_database_get_delete_folder_list(
			LIST *delete_folder_list,
			char *application_name,
			boolean dont_delete_mto1_isa,
			char *folder_name,
			LIST *primary_attribute_data_list );

DELETE_FOLDER *delete_database_delete_folder_new(
			char *application_name,
			char *folder_name,
			boolean null_1tom_upon_delete,
			boolean dont_delete_mto1_isa );

DELETE_ROW *delete_database_delete_row_new(
			LIST *primary_attribute_data_list );

void delete_database_execute_delete_folder_list(
			char *application_name,
			LIST *delete_folder_list,
			char *sql_executable );

void delete_database_execute_delete_row(
			char *application_name,
			char *folder_name,
			LIST *delete_row_list,
			LIST *primary_attribute_name_list,
			char *sql_executable,
			PROCESS *post_change_process );

void delete_database_execute_update_to_null_row(
			char *application_name,
			char *folder_name,
			LIST *delete_row_list,
			LIST *primary_attribute_name_list,
			LIST *update_null_attribute_name_list,
			char *sql_executable );

void delete_database_convert_parameters(
			char **executable,
			char *application_name,
			LIST *delete_row_list );

char *delete_database_display(
			DELETE_DATABASE *delete_database );

LIST *delete_database_get_delete_row_list(
			LIST *delete_folder_list,
			char *application_name,
			char *folder_name,
			LIST *one2m_related_folder_list,
			LIST *mto1_isa_related_folder_list,
			LIST *primary_attribute_name_list,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			boolean dont_delete_mto1_isa );

LIST *delete_database_get_one2m_delete_folder_list(
			LIST *delete_folder_list,
			char *folder_name,
			char *application_name,
			LIST *one2m_related_folder_list,
			LIST *primary_attribute_name_list,
			LIST *primary_attribute_data_list,
			boolean dont_delete_mto1_isa );

boolean delete_database_folder_exists(
			LIST *delete_folder_list,
			char *folder_name );

LIST *delete_database_get_mto1_isa_delete_folder_list(
			LIST *delete_folder_list,
			char *folder_name,
			char *application_name,
			LIST *mto1_isa_related_folder_list,
			LIST *primary_attribute_data_list,
			boolean dont_delete_mto1_isa );

void delete_database_execute_delete_folder_list(
			char *application_name,
			LIST *delete_folder_list,
			char *sql_executable );

void delete_database_set_parameter_dictionary(
			DICTIONARY *parameter_dictionary,
			LIST *primary_attribute_name_list,
			LIST *primary_attribute_data_list );

void delete_database_execute_delete_row_pipe(
			FILE *output_pipe,
			DICTIONARY *parameter_dictionary,
			char *application_name,
			char *folder_name,
			LIST *delete_row_list,
			LIST *primary_attribute_name_list,
			PROCESS *post_change_process );

void delete_database_execute_update_to_null_row_pipe(
			FILE *output_pipe,
			LIST *delete_row_list,
			LIST *primary_attribute_name_list,
			LIST *update_null_attribute_name_list );

#endif

