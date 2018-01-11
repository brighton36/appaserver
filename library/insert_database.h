/* $APPASERVER_HOME/library/insert_database.h				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver insert_database ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef INSERT_DATABASE_H
#define INSERT_DATABASE_H

#include <stdio.h>
#include "list.h"
#include "dictionary.h"
#include "process.h"
#include "related_folder.h"

#define INSERT_DATABASE_DONT_REMOVE_TMP_FILE	0

/* Structures */
/* ---------- */
typedef struct
{
	char *application_name;
	char *session;
	char *folder_name;
	LIST *primary_attribute_name_list;
	LIST *attribute_name_list;
	LIST *data_list;
	DICTIONARY *row_dictionary;
	LIST *ignore_attribute_name_list;
	int insert_row_zero_only;
	int dont_remove_tmp_file;
	int rows_inserted;
	char *post_insert_process;
	char *login_name;
	char *tmp_file_name;
	FILE *tmp_file;
	LIST *attribute_list;
} INSERT_DATABASE;

/* Operations */
/* ---------- */
INSERT_DATABASE *insert_database_new(
				char *application_name,
				char *session,
				char *folder_name,
				LIST *primary_attribute_name_list,
				LIST *attribute_name_list,
				DICTIONARY *row_dictionary,
				DICTIONARY *ignore_dictionary,
				LIST *attribute_list );

void set_insert_row_zero_only(	INSERT_DATABASE *insert_database );

void insert_database_set_row_zero_only(
				INSERT_DATABASE *insert_database );

int insert_database_execute(	char **message,
				char *application_name,
				char *session,
				char *folder_name,
				char *role_name,
				LIST *primary_attribute_name_list,
				LIST *insert_required_attribute_name_list,
				LIST *attribute_name_list,
				DICTIONARY *row_dictionary,
				LIST *ignore_attribute_name_list,
				int insert_row_zero_only,
				int dont_remove_tmp_file,
				PROCESS *post_change_process,
				char *login_name,
				LIST *mto1_related_folder_list,
				LIST *common_non_primary_attribute_name_list,
				LIST *attribute_list,
				boolean exists_reference_number,
				char *tmp_file_directory );

FILE *insert_database_open_tmp_file(
				char **tmp_file_name,
				char *tmp_file_directory,
				char *session );

void insert_database_remove_tmp_file(
				char *tmp_file_name );

int insert_database_execute_insert_mysql(
				char **message,
				char *application_name,
				char *folder_name,
				LIST *attribute_name_list,
				char *tmp_file_name,
				LIST *ignore_attribute_name_list );

void build_insert_tmp_file_each_row(	
				DICTIONARY *row_dictionary,
				char *application_name,
				LIST *primary_attribute_name_list,
				LIST *insert_required_attribute_name_list,
				LIST *attribute_name_list,
				LIST *ignore_attribute_name_list,
				FILE *insert_tmp_file,
				LIST *mto1_related_folder_list,
				LIST *common_non_primary_attribute_name_list,
				LIST *attribute_list,
				boolean exists_reference_number );

void build_insert_tmp_file_row_zero(	
				DICTIONARY *row_dictionary,
				char *application_name,
				LIST *primary_attribute_name_list,
				LIST *attribute_name_list,
				LIST *ignore_attribute_name_list,
				FILE *insert_tmp_file,
				LIST *mto1_related_folder_list,
				LIST *common_non_primary_attribute_name_list,
				LIST *attribute_list,
				boolean exists_reference_number );

void insert_database_populate_missing_attribute_name_list(
				LIST **missing_attribute_name_list,
				int row,
				DICTIONARY *query_dictionary,
				LIST *ignore_attribute_name_list,
				LIST *check_attribute_name_list );

LIST *insert_database_get_missing_attribute_name_list(
				int row,
				DICTIONARY *row_dictionary,
				LIST *ignore_attribute_name_list,
				LIST *primary_attribute_name_list,
				LIST *insert_required_attribute_name_list );

void build_insert_data_string( 	
				DICTIONARY *row_dictionary,
				char *destination,
		     		int row,
				LIST *ignore_attribute_name_list,
				LIST *primary_attribute_name_list,
				LIST *attribute_name_list,
				LIST *mto1_related_folder_list,
				LIST *common_non_primary_attribute_name_list,
				char *application_name,
				LIST *attribute_list );

void insert_database_dont_remove_tmp_file(
				INSERT_DATABASE *insert_database );

int insert_database_ignore_button_pressed(
				LIST *ignore_attribute_name_list,
				char *attribute_name );

void insert_database_set_post_insert_process(
				INSERT_DATABASE *insert_database,
				char *post_insert_process );
void insert_database_set_login_name(
				INSERT_DATABASE *insert_database,
				char *s );
/*
int insert_database_execute_process(
				char **message,
				char *application_name,
				char *session,
				char *person,
				char *executable,
				DICTIONARY *dictionary );
*/
void build_insert_tmp_file_data_list(
				char *application_name,
				char *session,
				char *folder_name,
				LIST *attribute_name_list,
				LIST *data_list,
				FILE *insert_tmp_file );
INSERT_DATABASE *insert_database_calloc(
				char *application_name,
				char *session,
				char *folder_name );
int insert_database_execute_data_list(
				char **message,
				char *application_name,
				char *session,
				char *folder_name,
				LIST *primary_attribute_name_list,
				LIST *attribute_name_list,
				LIST *data_list,
				int dont_remove_tmp_file );

void insert_database_execute_post_change_process_row_zero(
				char *application_name,
				char *session,
				DICTIONARY *row_dictionary,
				LIST *ignore_attribute_name_list,
				LIST *primary_attribute_name_list,
				LIST *insert_required_attribute_name_list,
				PROCESS *post_change_process,
				char *login_name,
				char *folder_name,
				char *role_name );

void insert_database_execute_post_change_process_each_row(
				char *application_name,
				char *session,
				DICTIONARY *row_dictionary,
				LIST *ignore_attribute_name_list,
				LIST *primary_attribute_name_list,
				LIST *insert_required_attribute_name_list,
				PROCESS *post_change_process,
				char *login_name,
				char *folder_name,
				char *role_name,
				LIST *mto1_related_folder_list,
				boolean exists_reference_number,
				LIST *attribute_name_list );

char *insert_database_get_common_data(
				DICTIONARY *dictionary,
				char *application_name,
				LIST *mto1_related_folder_list,
				char *attribute_name,
				int row );

RELATED_FOLDER *insert_database_get_common_data_related_folder(
				char *attribute_name,
				LIST *mto1_related_folder_list );

char *insert_database_get_fetch_common_data_where_clause_string( 	
				LIST *primary_attribute_name_list,
				LIST *primary_data_list );

char *insert_database_fetch_common_data(
				char *application_name,
				char *folder_name,
				char *attribute_name,
				LIST *primary_attribute_name_list,
				LIST *primary_data_list );
boolean insert_database_non_primary_attribute_populated(
				int row,
				DICTIONARY *dictionary,
				LIST *non_primary_attribute_name_list );
void insert_database_set_reference_number(
				char *application_name,
				DICTIONARY *dictionary,
				LIST *attribute_list,
				int row );

LIST *insert_database_get_trim_indices_dictionary_key_list(
				DICTIONARY *dictionary );

LIST *insert_database_set_ignore_primary_key_to_null(
				DICTIONARY *row_dictionary,
				LIST *ignore_attribute_name_list,
				LIST *primary_attribute_name_list );

/*
void insert_database_set_null_operator_primary_key_to_null(
				DICTIONARY *row_dictionary,
				LIST *attribute_list );
*/

#endif
