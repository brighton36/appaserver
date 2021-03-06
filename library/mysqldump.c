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
#include "piece.h"
#include "mysqldump.h"

MYSQLDUMP *mysqldump_new(	char *mysqldump_database_count_file,
				char *mysqldump_database_yesterday_count_file )

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

	a->mysqldump_database_count_file = mysqldump_database_count_file;

	a->mysqldump_database_yesterday_count_file =
		mysqldump_database_yesterday_count_file;

	a->mysqldump_database_folder_list =
		mysqldump_fetch_folder_list(
			mysqldump_database_count_file );

	a->mysqldump_database_yesterday_folder_list =
		mysqldump_fetch_folder_list(
			mysqldump_database_yesterday_count_file );

	return a;

} /* mysqldump_new() */

MYSQLDUMP_FOLDER *mysqldump_seek_folder(
				LIST *folder_list,
				char *folder_name )
{
	MYSQLDUMP_FOLDER *mysqldump_folder;

	if ( !list_rewind( folder_list ) ) return (MYSQLDUMP_FOLDER *)0;

	do {
		mysqldump_folder = list_get_pointer( folder_list );

		if ( timlib_strcmp(	mysqldump_folder->folder_name,
					folder_name ) == 0 )
		{
			return mysqldump_folder;
		}

	} while( list_next( folder_list ) );

	return (MYSQLDUMP_FOLDER *)0;

} /* mysqldump_seek_folder() */

void mysqldump_set_percentage_drop(
				LIST *mysqldump_database_folder_list,
				LIST *mysqldump_database_yesterday_folder_list )
{
	MYSQLDUMP_FOLDER *audit_folder;
	MYSQLDUMP_FOLDER *prior_folder;
	int row_count_drop;

	if ( !list_length( mysqldump_database_yesterday_folder_list ) ) return;

	if ( !list_rewind( mysqldump_database_folder_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty mysqldump_database_folder_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		audit_folder = list_get( mysqldump_database_folder_list );

		if ( ! ( prior_folder =
				mysqldump_seek_folder(
				    mysqldump_database_yesterday_folder_list,
				    audit_folder->folder_name ) ) )
		{
			continue;
		}

		/* prior_folder->table_exists = 1; */

		row_count_drop =
			prior_folder->row_count -
			audit_folder->row_count;

		if ( row_count_drop <= 0 ) continue;

		audit_folder->percentage_drop =
			(double)row_count_drop / 
			(double)prior_folder->row_count;

	} while( list_next( mysqldump_database_folder_list ) );

} /* mysqldump_set_percentage_drop() */

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

	/* Need to retire. */
	/* --------------- */
	a->table_exists = 1;

	return a;

} /* mysqldump_folder_new() */

LIST *mysqldump_fetch_folder_list(
				char *database_filename )
{
	LIST *folder_list;
	MYSQLDUMP_FOLDER *mysqldump_folder;
	FILE *input_file;
	char input_buffer[ 128 ];
	char piece_buffer[ 64 ];

	if ( ! ( input_file = fopen( database_filename, "r" ) ) )
		return (LIST *)0;

	folder_list = list_new();

	while( timlib_get_line( input_buffer, input_file, 128 ) )
	{
		mysqldump_folder = mysqldump_folder_new();

		mysqldump_folder->folder_name =
			strdup( piece( piece_buffer, ',', input_buffer, 0 ) );

		if ( !piece( piece_buffer, ',', input_buffer, 1 ) )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot piece() using delimiter = ',': (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );


			fclose( input_file );
			exit( 1 );
		}

		mysqldump_folder->row_count = atoi( piece_buffer );

		list_append_pointer(
			folder_list,
			mysqldump_folder );
	}

	fclose( input_file );

	return folder_list;

} /* mysqldump_fetch_folder_list() */

#ifdef NOT_DEFINED

This was to check if the table no longer exists.
------------------------------------------------

LIST *mysqldump_get_table_not_exists_drop_name_list(
				LIST *mysqldump_database_yesterday_folder_list )
{
	MYSQLDUMP_FOLDER *mysqldump_folder;
	LIST *not_exists_name_list;

	if ( !list_rewind( mysqldump_database_yesterday_folder_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty mysqldump_database_yesterday_folder_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	not_exists_name_list = list_new();

	do {
		mysqldump_folder =
			list_get(
				mysqldump_database_yesterday_folder_list );

		if ( !mysqldump_folder->table_exists )
		{
			list_append_pointer(
				not_exists_name_list,
				mysqldump_folder->folder_name );
		}

	} while( list_next( mysqldump_database_yesterday_folder_list ) );

	return not_exists_name_list;

} /* mysqldump_get_table_not_exists_drop_name_list() */

#endif

LIST *mysqldump_get_reached_percentage_drop_name_list(
				LIST *mysqldump_database_folder_list,
				double percentage_drop_threshold )
{
	MYSQLDUMP_FOLDER *mysqldump_folder;
	LIST *reached_name_list;

	if ( !list_rewind( mysqldump_database_folder_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty mysqldump_database_folder_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	reached_name_list = list_new();

	do {
		mysqldump_folder = list_get( mysqldump_database_folder_list );

		if (	mysqldump_folder->percentage_drop >=
			percentage_drop_threshold )
		{
			list_append_pointer(
				reached_name_list,
				mysqldump_folder->folder_name );
		}

	} while( list_next( mysqldump_database_folder_list ) );

	return reached_name_list;

} /* mysqldump_get_reached_percentage_drop_name_list() */

LIST *mysqldump_get_folder_name_list( LIST *folder_list )
{
	MYSQLDUMP_FOLDER *mysqldump_folder;
	LIST *folder_name_list;

	if ( !list_rewind( folder_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty folder_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	folder_name_list = list_new();

	do {
		mysqldump_folder = list_get( folder_list );

		list_append_pointer(
			folder_name_list,
			mysqldump_folder->folder_name );

	} while( list_next( folder_list ) );

	return folder_name_list;

} /* mysqldump_get_folder_name_list() */

