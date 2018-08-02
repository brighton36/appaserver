/* $APPASERVER_HOME/library/mysqldump.h			*/
/* ==================================================== */
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#ifndef MYSQLDUMP_H
#define MYSQLDUMP_H

#include "list.h"

typedef struct
{
	char *folder_name;
	int row_count;
	double percentage_drop;
} MYSQLDUMP_FOLDER;

typedef struct
{
	char *audit_database_filename;
	char *prior_audit_database_filename;
	LIST *audit_mysqldump_folder_list;
	LIST *prior_mysqldump_folder_list;
	LIST *reached_percentage_drop_name_list;
} MYSQLDUMP;

MYSQLDUMP *mysqldump_new(		char *audit_database_filename,
					char *prior_audit_database_filename );

MYSQLDUMP_FOLDER *mysqldump_folder_new(	void );

LIST *mysqldump_fetch_folder_list(
					char *database_filename );

void mysqldump_set_percentage_drop(
					LIST *audit_mysqldump_folder_list,
					LIST *prior_mysqldump_folder_list );

MYSQLDUMP_FOLDER *mysqldump_seek_folder(
					LIST *folder_list,
					char *folder_name );

LIST *mysqldump_get_reached_percentage_drop_name_list(
					LIST *audit_mysqldump_folder_list,
					double percentage_drop_threshold );

#endif
