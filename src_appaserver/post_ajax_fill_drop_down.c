/* $APPASERVER_HOME/src_appaserver/post_ajax_fill_drop_down.c	*/
/* ------------------------------------------------------------	*/
/* This process is triggered on the <submit> button of a	*/
/* forked ajax window.						*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "form.h"
#include "folder.h"
#include "session.h"
#include "related_folder.h"
#include "appaserver.h"
#include "operation_list.h"
#include "application.h"
#include "appaserver_user.h"
#include "query.h"
#include "query_attribute_statistics_list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session;
	char *role_name;
	char *mto1_folder_name;
	char *one2m_folder_name;
	char *value;
	FOLDER *one2m_folder;
	char *database_string = {0};

	if ( argc != 8 )
	{
		fprintf( stderr, 
"Usage: %s application login_name role session mto1_folder one2m_folder value\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	session = argv[ 4 ];
	mto1_folder_name = argv[ 5 ];
	one2m_folder_name = argv[ 6 ];
	value = argv[ 7 ];

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_relative_source_directory_to_path( application_name );


/*
	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( strcmp(	login_name,
			session_get_login_name(
					application_name,
					session ) ) != 0 )
	{
		session_access_failed_message_and_exit(
			application_name, session, login_name );
	}

	if ( !appaserver_user_exists_role(
					application_name,
					login_name,
					role_name ) )
	{
		session_access_failed_message_and_exit(
				application_name, session, login_name );
	}
*/

	one2m_folder =
		folder_new_folder( 	application_name,
					session,
					one2m_folder_name );

	folder_load(	&one2m_folder->insert_rows_number,
			&one2m_folder->lookup_email_output,
			&one2m_folder->row_level_non_owner_forbid,
			&one2m_folder->row_level_non_owner_view_only,
			&one2m_folder->populate_drop_down_process,
			&one2m_folder->post_change_process,
			&one2m_folder->folder_form,
			&one2m_folder->notepad,
			&one2m_folder->html_help_file_anchor,
			&one2m_folder->post_change_javascript,
			&one2m_folder->lookup_before_drop_down,
			&one2m_folder->data_directory,
			&one2m_folder->index_directory,
			&one2m_folder->no_initial_capital,
			&one2m_folder->subschema_name,
			application_name,
			session,
			one2m_folder->folder_name,
			1 /* override_row_restrictions */,
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

	one2m_folder->attribute_list =
		attribute_get_attribute_list(
			one2m_folder->application_name,
			one2m_folder->folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			(char *)0 /* role_name */ );

	printf( "Content-type: text/html\n\n" );

	printf( "Item 1^Item 2^Item 3\n" );

	return 0;

} /* main() */

