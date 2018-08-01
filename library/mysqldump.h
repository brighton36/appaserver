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
} MYSQLDUMP;

MYSQLDUMP *mysqldump_new(		void );

MYSQLDUMP_FOLDER *mysqldump_folder_new(	void );

#endif
