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
#include "mysqldump.h"

MYSQLDUMP *mysqldump_new( void )
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

