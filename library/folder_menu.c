/* library/folder_menu.c				   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary2file.h"
#include "timlib.h"
#include "folder_menu.h"
#include "list.h"
#include "appaserver_error.h"
#include "folder.h"
#include "piece.h"

void folder_menu_create_spool_file(
					char *application_name,
					char *session,
					char *appaserver_data_directory,
					char *role_name,
					boolean with_count )
{
	char sys_string[ 1024 ];
	char *filename;

	if ( !appaserver_data_directory || !*appaserver_data_directory )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty appaserver_data_directory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	filename = folder_menu_get_filename(
			application_name,
			session,
			appaserver_data_directory,
			role_name );

	if ( !filename )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get folder_menu_get_filename()\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( with_count )
	{
		sprintf( sys_string, 
"get_folder_count_list.sh %s \"%s\" %s >%s 2>>%s",
		 	application_name,
			role_name,
			FOLDER_MENU_PERMISSIONS,
			filename,
			appaserver_error_get_filename(
				application_name ) );
	}
	else
	{
		sprintf( sys_string,
"get_permission_folder_list.sh %s \"%s\" %s >%s 2>>%s",
		 	application_name,
			role_name,
			FOLDER_MENU_PERMISSIONS,
			filename,
			appaserver_error_get_filename(
				application_name ) );
	}

	system( sys_string );

} /* folder_menu_create_spool_file() */

LIST *folder_menu_get_choose_folder_list(
					char *application_name,
					char *appaserver_data_directory,
					char *session,
					char *role_name,
					boolean with_count )
{
	FOLDER *folder;
	char *sys_string;
	LIST *folder_list;
	LIST *folder_count_record_list;
	char *folder_count_record;
	char folder_name[ 128 ], count_string[ 128 ];

	folder_list = list_new();

	if ( !folder_menu_file_exists(
				application_name,
				session,
				appaserver_data_directory,
				role_name ) )
	{
		folder_menu_create_spool_file(
			application_name,
			session,
			appaserver_data_directory,
			role_name,
			with_count );
	}

	sys_string = folder_menu_get_input_pipe_sys_string(
					application_name,
					session,
					appaserver_data_directory,
					role_name );

	folder_count_record_list = pipe2list( sys_string );

	if ( list_rewind( folder_count_record_list ) )
	{
		do {
			folder_count_record =
				list_get_string( folder_count_record_list );

			piece( folder_name, '^', folder_count_record, 0 );

			folder = folder_new_folder(
						application_name,
						session,
						strdup( folder_name ) );

			if ( !folder ) continue;

			if ( with_count )
			{
				piece(	count_string, 
					'^', 
					folder_count_record, 
					1 );

				folder->row_count = atol( count_string );
			}

			folder_load(
				&folder->insert_rows_number,
				&folder->lookup_email_output,
				&folder->row_level_non_owner_forbid,
				&folder->row_level_non_owner_view_only,
				&folder->populate_drop_down_process,
				&folder->post_change_process,
				&folder->folder_form,
				&folder->notepad,
				&folder->html_help_file_anchor,
				&folder->post_change_javascript,
				&folder->row_access_count,
				&folder->lookup_before_drop_down,
				&folder->data_directory,
				&folder->index_directory,
				&folder->no_initial_capital,
				&folder->subschema_name,
				folder->application_name,
				folder->session,
				folder->folder_name,
				0 /* not override_row_restrictions */,
				(char *)0 /* role_name */,
				(LIST *)0 /* mto1_related_folder_list */ );

			list_append_pointer( folder_list, folder );
		} while( list_next( folder_count_record_list ) );
	}
	list_free_string_list( folder_count_record_list );
	return folder_list;
} /* folder_menu_get_choose_folder_list() */

char *folder_menu_get_input_pipe_sys_string(
					char *application_name,
					char *session,
					char *appaserver_data_directory,
					char *role_name )
{
	char sys_string[ 1024 ];
	char *filename;

	filename = folder_menu_get_filename(
			application_name,
			session,
			appaserver_data_directory,
			role_name );

	if ( !filename )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get folder_menu_get_filename()\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf( sys_string, "cat %s", filename );

	return strdup( sys_string );

} /* folder_menu_get_input_pipe_sys_string() */

char *folder_menu_get_filename(
				char *application_name,
				char *session,
				char *appaserver_data_directory,
				char *role_name )
{
	char filename[ 1024 ];

	if ( !appaserver_data_directory
	||   !application_name
	||   !role_name
	||   !session )
	{
		return (char *)0;
/*
		fprintf(stderr,
			"ERROR in %s/%s()/%d: insufficient input.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
*/
	}

/*
	sprintf( filename,
		 "%s/%s/%s_%s_%s.txt",
		 appaserver_data_directory,
		 DICTIONARY2FILE_DIRECTORY,
		 application_name,
		 role_name,
		 session );
*/
	sprintf( filename,
		 "%s/%s_%s_%s.txt",
		 appaserver_data_directory,
		 application_name,
		 role_name,
		 session );

	return strdup( filename );

} /* folder_menu_get_filename() */

void folder_menu_remove_file(
				char *application_name,
				char *session,
				char *appaserver_data_directory,
				char *role_name )
{
	char *filename;
	char sys_string[ 1024 ];

	if ( !( filename = folder_menu_get_filename(
				application_name,
				session,
				appaserver_data_directory,
				role_name ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get filename.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf( sys_string,
		 "rm %s",
		 filename );

	system( sys_string );

} /* folder_menu_remove_file() */

boolean folder_menu_file_exists(
				char *application_name,
				char *session,
				char *appaserver_data_directory,
				char *role_name )
{
	char *filename;

	if ( ( filename = folder_menu_get_filename(
				application_name,
				session,
				appaserver_data_directory,
				role_name ) ) )
	{
		return timlib_file_exists( filename );
	}
	else
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get filename.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

} /* folder_menu_file_exists() */

