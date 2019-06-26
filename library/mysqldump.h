/* $APPASERVER_HOME/library/mysqldump.h			*/
/* ==================================================== */
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#ifndef MYSQLDUMP_H
#define MYSQLDUMP_H

#include "list.h"
#include "boolean.h"

typedef struct
{
	char *folder_name;
	int row_count;
	double percentage_drop;
	boolean table_exists;
} MYSQLDUMP_FOLDER;

typedef struct
{
	char *mysqldump_database_count_file;
	char *mysqldump_database_yesterday_count_file;
	LIST *mysqldump_database_folder_list;
	LIST *mysqldump_database_yesterday_folder_list;
	LIST *reached_percentage_drop_name_list;
} MYSQLDUMP;

MYSQLDUMP *mysqldump_new(	char *mysqldump_database_count_file,
				char *mysqldump_database_yesterday_count_file );

MYSQLDUMP_FOLDER *mysqldump_folder_new(	void );

LIST *mysqldump_fetch_folder_list(
					char *database_filename );

void mysqldump_set_percentage_drop(
			LIST *mysqldump_database_folder_list,
			LIST *mysqldump_database_yesterday_folder_list );

MYSQLDUMP_FOLDER *mysqldump_seek_folder(
					LIST *folder_list,
					char *folder_name );

LIST *mysqldump_get_reached_percentage_drop_name_list(
			LIST *mysqldump_database_folder_list,
			double percentage_drop_threshold );

LIST *mysqldump_get_table_not_exists_drop_name_list(
			LIST *mysqldump_database_yesterday_folder_list );

LIST *mysqldump_get_folder_name_list(	LIST *folder_list );

#endif
