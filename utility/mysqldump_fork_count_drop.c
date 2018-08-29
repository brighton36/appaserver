/* $APPASERVER_HOME/utility/mysqldump_fork_count_drop.c */
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "boolean.h"
#include "column.h"
#include "timlib.h"
#include "mysqldump.h"

/* Constants */
/* --------- */
#define PERCENTAGE_DROP_THRESHOLD	0.20

/* Prototypes */
/* ---------- */
void mysqldump_fork_count_drop(
				char *audit_database_filename,
				char *prior_audit_database_filename );

int main( int argc, char **argv )
{
	char *audit_database_filename;
	char *prior_audit_database_filename;

	if ( argc != 3 )
	{
		fprintf( stderr,
	"Usage: %s audit_database_filename prior_audit_database_filename\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	timlib_error_stderr( argc, argv );

	audit_database_filename = argv[ 1 ];
	prior_audit_database_filename = argv[ 2 ];

	mysqldump_fork_count_drop(
		audit_database_filename,
		prior_audit_database_filename );

	return 0;

} /* main() */

void mysqldump_fork_count_drop(
				char *audit_database_filename,
				char *prior_audit_database_filename )
{
	MYSQLDUMP *mysqldump;

	mysqldump =
		mysqldump_new(
			audit_database_filename,
			prior_audit_database_filename );

	mysqldump_set_percentage_drop(
		mysqldump->audit_mysqldump_folder_list,
		mysqldump->prior_mysqldump_folder_list );

	mysqldump->reached_percentage_drop_name_list =
		mysqldump_get_reached_percentage_drop_name_list(
			mysqldump->audit_mysqldump_folder_list,
			PERCENTAGE_DROP_THRESHOLD );

	list_append_list(
		mysqldump->reached_percentage_drop_name_list,
		mysqldump_get_table_not_exists_drop_name_list(
			mysqldump->prior_mysqldump_folder_list ) );

	if ( list_length( mysqldump->reached_percentage_drop_name_list ) )
	{
		printf( "%s\n",
			list_display(
				mysqldump->
					reached_percentage_drop_name_list ) );
	}

} /* mysqldump_fork_count_drop() */
