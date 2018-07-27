/* $APPASERVER_HOME/utility/mysqldump_fork_count_drop.c */
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "boolean.h"
#include "column.h"
#include "timlib.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

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

	return 0;

} /* main() */

