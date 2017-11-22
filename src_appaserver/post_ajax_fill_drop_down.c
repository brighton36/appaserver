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
	FOLDER *mto1_folder;
	RELATED_FOLDER *related_folder;
	char *database_string = {0};
	char sys_string[ 1024 ];
	char *where;
	char *results;
	char formatted_results[ 1024 ];

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
*/

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

	session_update_access_date_time( application_name, session );
	appaserver_library_purge_temporary_files( application_name );

	mto1_folder =
		folder_with_load_new(
				application_name,
				session,
				mto1_folder_name,
				role_new( application_name, role_name ) );

	form_output_content_type();

/*
{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: for mto1_folder = %s, got mto1_related_folder_list = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
mto1_folder->folder_name,
related_folder_list_display( mto1_folder->mto1_related_folder_list, mto1, '\n' ) );
m2( application_name, msg );
}
*/

	if ( ! ( related_folder =
			related_folder_mto1_seek(
				mto1_folder->mto1_related_folder_list,
				one2m_folder_name ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot seek one2m_folder_name = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 one2m_folder_name );
		exit( 1 );
	}

	where = query_get_simple_where_clause(
			(FOLDER *)0,
			related_folder->
				foreign_attribute_name_list
				/* where_attribute_name_list */,
			list_string2list(
				value,
				'^' )
				/* where_attribute_data_list */,
			(char *)0 /* login_name */ );

	sprintf( sys_string,
		 "get_folder_data	application=%s	 "
		 "			select=%s	 "
		 "			folder=%s	 "
		 "			where=\"%s\"	 "
		 "			order=select	|"
		 "joinlines.e '^'			 ",
		 application_name,
		 mto1_folder->folder_name,
		 list_display( mto1_folder->primary_attribute_name_list ),
		 where );

	results = pipe2string( sys_string );

	if ( results && *results )
	{
		format_initial_capital( formatted_results, results );

		printf( "%s^%s|Select^%s\n",
			NULL_OPERATOR,
			results,
			formatted_results );
	}
	else
	{
		printf( "\n" );
	}

	return 0;

} /* main() */

