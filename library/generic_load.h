/* library/generic_load.h				*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef GENERIC_LOAD_H
#define GENERIC_LOAD_H

#include <stdio.h>
#include "list.h"

/* Constants */
/* --------- */
/* #define DEBUG_MODE			1 */
#define SKIP_HEADER_ROWS		"skip_header_rows"
#define REPLACE_EXISTING_RECORDS_YN	"replace_existing_records_yn"
#define INSERT_DELIMITER		'^'
#define TEST_RECORD_OUTPUT_COUNT	10

/* Structures */
/* ---------- */
typedef struct
{
	char *attribute_name;
	boolean is_primary_key;
	boolean is_primary_key_date_datatype;
	boolean is_primary_key_time_datatype;
	int position;
	char *constant;
	char *datatype;
	boolean is_participating;
} GENERIC_LOAD_ATTRIBUTE;

typedef struct
{
	char *folder_name;
	LIST *generic_load_attribute_list;
	int primary_key_date_offset;
	int primary_key_time_offset;
} GENERIC_LOAD_FOLDER;

typedef struct
{
	FOLDER *folder;
	LIST *generic_load_folder_list;
} GENERIC_LOAD;


/* Prototypes */
/* ---------- */
char *generic_load_get_primary_key_list_string(
				LIST *generic_load_attribute_list );

void generic_load_reset_row_count(
				void );

boolean generic_load_skip_header_rows(
				int skip_header_rows );

boolean generic_load_delimiters_only(
				char *input_buffer,
				char delimiter );

boolean generic_load_has_participating(
				LIST *generic_load_attribute_list );

char *generic_load_get_heading_string(
				LIST *generic_load_attribute_list );

void generic_load_attribute_set_is_participating(
				LIST *generic_load_folder_list,
				char *delimiter,
				char *application_name,
				char *load_filename,
				int skip_header_rows );

boolean position_all_valid(	DICTIONARY *position_dictionary );

void generic_load_get_primary_key_date_time_offset(
				int *primary_key_date_offset,
				int *primary_key_time_offset,
				LIST *generic_load_attribute_list );

boolean generic_load_fix_time(
				char *input_buffer,
				char delimiter,
				int primary_key_time_offset );

void generic_load_replace_time_2400_with_0000(
				char *input_buffer,
				char delimiter,
				int primary_key_date_offset,
				int primary_key_time_offset );

char *generic_load_get_piece_buffer(
				char *input_buffer,
				char *delimiter,
				char *application_name,
				char *datatype,
				char *constant,
				int piece_offset,
				boolean is_primary_key );

GENERIC_LOAD_FOLDER *generic_load_get_folder(
				char *folder_name,
				LIST *attribute_list,
				DICTIONARY *position_dictionary,
				DICTIONARY *constant_dictionary,
				boolean primary_attributes_only,
				char *login_name,
				char *related_attribute_name );

GENERIC_LOAD *generic_load_new(
				void );

GENERIC_LOAD_FOLDER *generic_load_folder_new(
				char *folder_name );

GENERIC_LOAD_ATTRIBUTE *generic_load_attribute_new(
				char *attribute_name,
				boolean is_primary_key );

FOLDER *generic_load_get_database_folder(
				char *application_name,
				char *session,
				char *folder_name,
				char *role_name,
				boolean with_mto1_related_folders );

LIST *generic_load_get_folder_list(
				FOLDER *folder,
				DICTIONARY *position_dictionary,
				DICTIONARY *constant_dictionary,
				char *login_name );

/*
boolean generic_load_delete_from_database(
				char *application_name,
				char *load_filename,
				char delimiter,
				GENERIC_LOAD_FOLDER *generic_load_folder,
				int skip_header_rows_integer );
*/

int generic_load_output_to_database(
				char *application_name,
				char *load_filename,
				char delimiter,
				LIST *generic_load_folder_list,
				int skip_header_rows_integer );

int generic_load_output_test_only_report(
				char *load_filename,
				char delimiter,
				LIST *generic_load_attribute_list,
				int primary_key_date_offset,
				int primary_key_time_offset,
				char *application_name,
				boolean display_errors,
				char *folder_name,
				int skip_header_rows_integer,
				boolean with_sort_unique );

boolean generic_load_build_sys_string(
				char *sys_string,
				char *application_name,
				char *folder_name,
				LIST *generic_load_attribute_list,
				boolean sort_unique );

void generic_load_set_international_date(
				char *input_buffer,
				char *application_name,
				char delimiter,
				int primary_key_date_offset );

int generic_load_get_position_integer(	DICTIONARY *position_dictionary,
				char *attribute_name,
				char *related_attribute_name );

#endif
