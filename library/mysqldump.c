/* $APPASERVER_HOME/library/mysqldump.c			*/
/* ==================================================== */
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "timlib.h"
#include "mysqldump.h"

MYSQLDUMP *mysqldump_new(	char *audit_database_filename,
				char *prior_audit_database_filename )

{
	MYSQLDUMP *a;

	if ( ! ( a = calloc( 1, sizeof( MYSQLDUMP ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	a->audit_database_filename = audit_database_filename;
	a->prior_audit_database_filename = prior_audit_database_filename;

	a->audit_mysqldump_folder_list =
		mysqldump_fetch_folder_list(
			audit_database_filename );

	a->prior_mysqldump_folder_list =
		mysqldump_fetch_folder_list(
			prior_audit_database_filename );

	return a;

} /* mysqldump_new() */

MYSQLDUMP_FOLDER *mysqldump_folder_new(	void )
{
	MYSQLDUMP_FOLDER *a;

	if ( ! ( a = calloc( 1, sizeof( MYSQLDUMP_FOLDER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return a;

} /* mysqldump_folder_new() */

LIST *mysqldump_fetch_folder_list(
				char *database_filename )
{
	LIST *folder_list;
	MYSQLDUMP_FOLDER *mysqldump_folder;
	FILE *input_file;
	char input_buffer[ 128 ];

	if ( ! ( input_file = fopen( database_filename, "r" ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 database_filename );

		exit( 1 );
	}

	folder_list = list_new();

	while( timlib_get_line( input_buffer, input_file, 128 ) )
	{
	}

	fclose( input_file );

	return folder_list;

} /* mysqldump_fetch_folder_list() */
