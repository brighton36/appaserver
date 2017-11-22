/* library/role.c 							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver role ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "role.h"
#include "appaserver_library.h"
#include "appaserver_error.h"

ROLE *role_new(
		char *application_name,
		char *role_name )
{
	return role_new_role( application_name, role_name );

} /* role_new() */

ROLE *role_new_role(
		char *application_name,
		char *role_name )
{
	ROLE *role;

	if ( !role_name || strcmp( role_name, "ignored" ) == 0 )
		return (ROLE *)0;

	if ( ! ( role = (ROLE *)calloc( 1, sizeof( ROLE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	role->application_name = application_name;
	role->role_name = role_name;

	role_fetch(
			&role->folder_count_yn,
			&role->override_row_restrictions_yn,
			&role->grace_no_cycle_colors_yn,
			application_name,
			role_name );

	return role;

} /* role_new_role() */

void role_free( ROLE *role )
{
	free( role );
}

boolean role_fetch( 		char *folder_count_yn,
				char *override_row_restrictions_yn,
				char *grace_no_cycle_colors_yn,
				char *application_name,
				char *role_name )
{
	char sys_string[ 1024 ];
	char piece_string[ 128 ];
	char *results;
	char *select;
	char where[ 128 ];
	char *folder_name;

	if ( !role_name ) return 0;

	folder_name = "role";

	if ( attribute_exists(	application_name,
				folder_name,
				"grace_no_cycle_colors_yn" ) )
	{
		select =
	"folder_count_yn,override_row_restrictions_yn,grace_no_cycle_colors_yn";
	}
	else
	{
		select =
		"folder_count_yn,override_row_restrictions_yn,'n'";
	}

	sprintf( where,
		 "role = '%s'",
		 role_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder_name,
		 where );

	results = pipe2string( sys_string );

	if ( !results ) return 0;

	piece( piece_string, '^', results, 0 );
	*folder_count_yn = *piece_string;

	piece( piece_string, '^', results, 1 );
	*override_row_restrictions_yn = *piece_string;

	piece( piece_string, '^', results, 2 );
	*grace_no_cycle_colors_yn = *piece_string;

	free( results );
	return 1;

} /* role_fetch() */

boolean role_get_override_row_restrictions(
		char override_row_restrictions_yn )
{
	return (override_row_restrictions_yn == 'y');
}

boolean role_get_exists_folder_count_y(
					char *application_name,
					char *login_name )
{
	char *role_table_name;
	char *role_appaserver_user_table_name;
	char sys_string[ 1024 ];
	char *results;

	role_table_name = get_table_name( application_name, "role" );
	role_appaserver_user_table_name =
		get_table_name( application_name, "role_appaserver_user" );

	sprintf( sys_string,
		 "echo \"select count(*)				 "
		 "	 from %s,%s					 "
		 "	 where %s.role = %s.role			 "
		 "	   and %s.login_name = '%s'			 "
		 "	   and folder_count_yn = 'y';\"			|"
		 "sql.e							 ",
		 role_table_name,
		 role_appaserver_user_table_name,
		 role_table_name,
		 role_appaserver_user_table_name,
		 role_appaserver_user_table_name,
		 login_name );

	results = pipe2string( sys_string );
	return atoi( results );

} /* role_get_exists_folder_count_y() */

ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude_new(
					char *attribute_name,
					char *permission )
{
	ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude;

	if ( ! ( role_attribute_exclude =
			(ROLE_ATTRIBUTE_EXCLUDE *)
				calloc( 1,
					sizeof( ROLE_ATTRIBUTE_EXCLUDE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	role_attribute_exclude->attribute_name = attribute_name;
	role_attribute_exclude->permission = permission;

	return role_attribute_exclude;

} /* role_attribute_exclude_new() */

LIST *role_get_attribute_exclude_list(	char *application_name,
					char *role_name )
{
	char sys_string[ 1024 ];
	char attribute_name[ 128 ];
	char permission[ 128 ];
	char *select;
	char where[ 128 ];
	FILE *input_pipe;
	char input_buffer[ 256 ];
	ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude;
	LIST *attribute_exclude_list = {0};

	select = "attribute,permission";

	sprintf( where,
		 "role = '%s'",
		 role_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=attribute_exclude	"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	attribute_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	permission,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		if ( !attribute_exclude_list )
			attribute_exclude_list = list_new();

		role_attribute_exclude =
			role_attribute_exclude_new(
				strdup( attribute_name ),
				strdup( permission ) );

		list_append_pointer(	attribute_exclude_list,
					role_attribute_exclude );
	}

	return attribute_exclude_list;

} /* role_get_attribute_exclude_list() */

boolean role_exists_attribute_exclude_insert(
				LIST *attribute_exclude_list,
				char *attribute_name )
{
	ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude;

	if ( !list_rewind( attribute_exclude_list ) )
		return 0;

	do {
		role_attribute_exclude = list_get( attribute_exclude_list );

		if ( strcmp(	role_attribute_exclude->attribute_name,
				attribute_name ) == 0 )
		{
			if ( strcmp(	role_attribute_exclude->permission,
					"insert" ) == 0 )
			{
				return 1;
			}
		}
	} while( list_next( attribute_exclude_list ) );

	return 0;

} /* role_exists_attribute_exclude_insert() */

