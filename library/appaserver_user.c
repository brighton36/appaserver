/* appaserver_user.c 							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver user ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_user.h"
#include "appaserver_library.h"
#include "piece.h"
#include "folder.h"
#include "timlib.h"

static APPASERVER_USER *global_appaserver_user = {0};

APPASERVER_USER *appaserver_user_new_appaserver_user(
				char *application_name,
				char *login_name )
{
	char sys_string[ 1024 ];
	char piece_buffer[ 1024 ];
	char *input_string;
	APPASERVER_USER *appaserver_user;
	char *table_name;
	char select_string[ 128 ];
	char where_string[ 128 ];
	char *select_frameset;
	char *select_person;

	table_name = get_table_name( application_name, "appaserver_user" );

	if ( folder_exists_attribute(	application_name,
					"appaserver_user",
					"person_full_name" ) )
	{
		select_person = "person_full_name";
	}
	else
	{
		select_person = "null";
	}

	if ( folder_exists_attribute(	application_name,
					"appaserver_user",
					"frameset_menu_horizontal_yn" ) )
	{
		select_frameset = "frameset_menu_horizontal_yn";
	}
	else
	{
		select_frameset = "null";
	}

	sprintf( select_string,
		 "%s,%s,password",
		 select_person,
		 select_frameset );

	sprintf( where_string, "login_name = '%s'", login_name );

	sprintf(	sys_string,
			"echo \"select %s from %s where %s;\" | sql.e '%c'",
			select_string,
			table_name,
			where_string,
			APPASERVER_USER_RECORD_DELIMITER );

	input_string = pipe2string( sys_string );

	if ( !input_string )
	{
		fprintf(stderr,
	"Warning in %s/%s/%d(): cannot get record for login_name = (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			login_name );
		return (APPASERVER_USER *)0;
	}

	if ( character_count( APPASERVER_USER_RECORD_DELIMITER,
			      input_string ) != 2 )
	{
		fprintf(stderr,
			"ERROR in %s/%s/%d(): invalid input = (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			input_string );
		exit( 1 );
	}

	appaserver_user =
		(APPASERVER_USER *)
			calloc( 1, sizeof( APPASERVER_USER ) );

	piece(	piece_buffer,
		APPASERVER_USER_RECORD_DELIMITER,
		input_string,
		0 );
	appaserver_user->person_full_name = strdup( piece_buffer );

	piece(	piece_buffer,
		APPASERVER_USER_RECORD_DELIMITER,
		input_string,
		1 );
	appaserver_user->password = strdup( piece_buffer );

	piece(	piece_buffer,
		APPASERVER_USER_RECORD_DELIMITER,
		input_string,
		2 );
	appaserver_user->frameset_menu_horizontal_yn = *piece_buffer;

	return appaserver_user;

} /* appaserver_user_new_appaserver_user() */

LIST *appaserver_user_get_session_list(	char *application_name,
					char *login_name )
{
	char sys_string[ 1024 ];
	char where_string[ 128 ];

	sprintf( where_string, "login_name = '%s'", login_name );

	sprintf(	sys_string,
			"get_folder_data	application=%s		  "
			"			folder=appaserver_sessions"
			"			select=appaserver_session "
			"			where=\"%s\"		  ",
			application_name,
			where_string );

	return pipe2list( sys_string );

} /* appaserver_user_get_session_list() */

char *appaserver_user_get_person_full_name(
			char *application_name, char *login_name )
{
	if ( !global_appaserver_user )
	{
		global_appaserver_user =
			appaserver_user_new_appaserver_user(
				application_name, login_name );
	}

	if ( !global_appaserver_user )
		return "";
	else
		return global_appaserver_user->person_full_name;

} /* appaserver_user_get_person_full_name() */

char *appaserver_user_get_password( char *application_name, char *login_name )
{
	if ( !global_appaserver_user )
	{
		global_appaserver_user =
			appaserver_user_new_appaserver_user(
				application_name, login_name );
	}

	if ( !global_appaserver_user )
		return "";
	else
		return global_appaserver_user->password;

} /* appaserver_user_get_password() */

char appaserver_user_get_frameset_menu_horizontal_yn(
						char *application_name,
						char *login_name )
{
	if ( !global_appaserver_user )
	{
		global_appaserver_user =
			appaserver_user_new_appaserver_user(
				application_name, login_name );
	}

	if ( !global_appaserver_user )
		return '\0';
	else
		return global_appaserver_user->frameset_menu_horizontal_yn;
}

boolean appaserver_user_exists_session(	char *application_name,
					char *login_name,
					char *session )
{
	if ( !global_appaserver_user )
	{
		global_appaserver_user =
			appaserver_user_new_appaserver_user(
				application_name, login_name );
	}

	if ( !global_appaserver_user->session_list )
	{
		global_appaserver_user->session_list =
			appaserver_user_get_session_list(
				application_name,
				login_name );
	}

	return list_exists_string(
		global_appaserver_user->session_list,
		session );

} /* appaserver_user_exists_session() */

boolean appaserver_user_exists_role(	char *application_name,
					char *login_name,
					char *role_name )
{
	if ( !global_appaserver_user )
	{
		global_appaserver_user =
			appaserver_user_new_appaserver_user(
				application_name, login_name );
	}

	if ( !global_appaserver_user->role_list )
	{
		global_appaserver_user->role_list =
			appaserver_user_get_role_list(
				application_name,
				login_name );
	}

	return list_exists_string(
		global_appaserver_user->role_list,
		role_name );

} /* appaserver_user_exists_role() */

LIST *appaserver_user_get_role_list(
				char *application_name,
				char *login_name )
{
	char where[ 128 ];
	char sys_string[ 1024 ];

	sprintf( where, "login_name = '%s'", login_name );
	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=role			"
		 "			folder=role_appaserver_user	"
		 "			where=\"%s\"			",
		 application_name,
		 where );
	return pipe2list( sys_string );
} /* appaserver_user_get_role_list() */

