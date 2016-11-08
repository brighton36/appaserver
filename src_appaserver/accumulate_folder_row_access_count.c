/* ---------------------------------------------	*/
/* accumulate_folder_row_access_count.c			*/
/* ---------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "folder.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "appaserver.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void accumulate_folder_row_access_count(
				char *application_name,
				char *folder_name,
				int folder_row_access_count );

int main( int argc, char **argv )
{
	char *application_name;
	char *folder_name;
	register int folder_row_access_count = 0;
	char buffer[ 8192 ];

	if ( argc != 3 )
	{
		fprintf( stderr, "Usage: %s application folder\n", argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	folder_name = argv[ 2 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

/*
	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );
*/

	while( get_line( buffer, stdin ) )
	{
		folder_row_access_count++;
		printf( "%s\n", buffer );
	}

	accumulate_folder_row_access_count(
					application_name,
					folder_name,
					folder_row_access_count );

	exit( 0 );
} /* main() */

void accumulate_folder_row_access_count(
				char *application_name,
				char *folder_name,
				int folder_row_access_count )
{
	char sys_string[ 1024 ];
	char *table_name;
	FOLDER *folder;
	char *session = "99";

	folder = folder_new_folder(
				application_name,
				session,
				folder_name );

	folder_load(	&folder->insert_rows_number,
			&folder->lookup_email_output,
			&folder->row_level_non_owner_forbid,
			&folder->row_level_non_owner_view_only,
			&folder->populate_drop_down_process,
			&folder->post_change_process,
			&folder->folder_form,
			&folder->notepad,
			&folder->html_help_file_anchor,
			&folder->row_access_count,
			&folder->lookup_before_drop_down,
			&folder->data_directory,
			&folder->index_directory,
			&folder->no_initial_capital,
			&folder->subschema_name,
			application_name,
			session,
			folder->folder_name,
			(LIST *)0 /* mto1_related_folder_list */ );


	table_name = get_table_name( application_name, "folder" );

	sprintf( sys_string,
		 "nohup echo \"update %s			 "
		 "	 set row_access_count = %d		 "
		 "	 where folder = '%s';\"			|"
		 "sql.e >/dev/null &				 ",
		 table_name,
		 folder_row_access_count + folder->row_access_count,
		 folder_name );

	system( sys_string );
} /* accumulate_folder_row_access_count() */

