/* $APPASERVER_HOME/utility/mysqldump_fork_count_drop.c */
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "boolean.h"
#include "column.h"
#include "mysqldump.h"

/* Constants */
/* --------- */

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

} /* mysqldump_fork_count_drop() */

