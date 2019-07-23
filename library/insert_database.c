/* $APPASERVER_HOME/library/insert_database.c				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver insert_database ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "insert_database.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "process.h"
#include "appaserver_library.h"
#include "related_folder.h"
#include "list.h"

INSERT_DATABASE *insert_database_calloc(
						char *application_name,
						char *session,
						char *folder_name )
{
	INSERT_DATABASE *i = (INSERT_DATABASE *)
				calloc( 1, sizeof( INSERT_DATABASE ) );
	i->application_name = application_name;
	i->session = session;
	i->folder_name = folder_name;
	return i;
} /* insert_database_calloc() */

INSERT_DATABASE *insert_database_new(
				char *application_name,
				char *session,
				char *folder_name,
				LIST *primary_attribute_name_list,
				LIST *attribute_name_list,
				DICTIONARY *row_dictionary,
				DICTIONARY *ignore_dictionary,
				LIST *attribute_list )
{
	INSERT_DATABASE *i;

	i = insert_database_calloc(
		application_name,
		session,
		folder_name );

	i->primary_attribute_name_list = primary_attribute_name_list;
	i->attribute_name_list = attribute_name_list;
	i->row_dictionary = row_dictionary;

	i->ignore_attribute_name_list =
		insert_database_get_trim_indices_dictionary_key_list(
			ignore_dictionary );

	i->ignore_attribute_name_list =
		insert_database_set_ignore_primary_key_to_null(
			i->row_dictionary,
			i->ignore_attribute_name_list,
			primary_attribute_name_list );

	i->attribute_list = attribute_list;

	return i;
} /* insert_database_new() */

void insert_database_set_row_zero_only(
				INSERT_DATABASE *insert_database )
{
	set_insert_row_zero_only( insert_database );
}

void set_insert_row_zero_only( INSERT_DATABASE *insert_database )
{
	insert_database->insert_row_zero_only = 1;
}

int insert_database_execute(	char **message,
				char *application_name,
				char *session,
				char *folder_name,
				char *role_name,
				LIST *primary_attribute_name_list,
				LIST *insert_required_attribute_name_list,
				LIST *attribute_name_list,
				DICTIONARY *row_dictionary,
				LIST *ignore_attribute_name_list,
				int insert_row_zero_only,
				int dont_remove_tmp_file,
				PROCESS *post_change_process,
				char *login_name,
				LIST *mto1_related_folder_list,
				LIST *common_non_primary_attribute_name_list,
				LIST *attribute_list,
				boolean exists_reference_number,
				char *tmp_file_directory )
{
	FILE *tmp_file;
	int rows_inserted;
	char *tmp_file_name = {0};

	tmp_file = insert_database_open_tmp_file(
				&tmp_file_name,
				tmp_file_directory,
				session );

	if ( !insert_row_zero_only )
	{
		build_insert_tmp_file_each_row(
				row_dictionary,
				application_name,
				primary_attribute_name_list,
				insert_required_attribute_name_list,
				attribute_name_list,
				ignore_attribute_name_list,
				tmp_file,
				mto1_related_folder_list,
				common_non_primary_attribute_name_list,
				attribute_list,
				exists_reference_number );

		fclose( tmp_file );

		rows_inserted =
			insert_database_execute_insert_mysql(
				message,
				application_name,
				folder_name,
				attribute_name_list,
				tmp_file_name,
				ignore_attribute_name_list );

		if ( rows_inserted && post_change_process )
		{
			insert_database_execute_post_change_process_each_row(
				message,
				application_name,
				session,
				row_dictionary,
				ignore_attribute_name_list,
				primary_attribute_name_list,
				insert_required_attribute_name_list,
				post_change_process,
				login_name,
				folder_name,
				role_name,
				mto1_related_folder_list,
				exists_reference_number,
				attribute_name_list );
		}
	}
	else
	{
		build_insert_tmp_file_row_zero(
				row_dictionary,
				application_name,
				primary_attribute_name_list,
				attribute_name_list,
				ignore_attribute_name_list,
				tmp_file,
				mto1_related_folder_list,
				common_non_primary_attribute_name_list,
				attribute_list,
				exists_reference_number );

		fclose( tmp_file );

		rows_inserted = 
			insert_database_execute_insert_mysql(
				message,
				application_name,
				folder_name,
				attribute_name_list,
				tmp_file_name,
				ignore_attribute_name_list );

		if ( rows_inserted && post_change_process )
		{
			insert_database_execute_post_change_process_row_zero(
				message,
				application_name,
				session,
				row_dictionary,
				ignore_attribute_name_list,
				primary_attribute_name_list,
				insert_required_attribute_name_list,
				post_change_process,
				login_name,
				folder_name,
				role_name );
		}
	}

	if ( !dont_remove_tmp_file )
	{
		insert_database_remove_tmp_file( tmp_file_name );
	}

	return rows_inserted;

} /* insert_database_execute() */

void insert_database_remove_tmp_file( char *tmp_file_name )
{
	char buffer[ 512 ];

	sprintf( buffer, 
		 "/bin/rm -f %s",
		 tmp_file_name );
	system( buffer );
}

FILE *insert_database_open_tmp_file(	char **tmp_file_name,
					char *tmp_file_directory,
					char *session )
{
	static char local_tmp_file_name[ 128 ];
	FILE *tmp_file;

	if ( tmp_file_directory && *tmp_file_directory )
	{
		sprintf(local_tmp_file_name, 
		 	"%s/appaserver_insert_%s", 
			tmp_file_directory,
		 	session );
	}
	else
	{
		sprintf( local_tmp_file_name, 
		 	"/tmp/appaserver_insert_%s", 
		 	session );
	}

	tmp_file = fopen( local_tmp_file_name, "w" );

	if ( !tmp_file )
	{
		fprintf( stderr, 
		"ERROR in %s/%s(): cannot open: %s\n",
			 __FILE__,
			 __FUNCTION__,
			 local_tmp_file_name );
		exit( 1 );
	}

	*tmp_file_name = local_tmp_file_name;
	return tmp_file;

} /* insert_database_open_tmp_file() */

void build_insert_tmp_file_each_row(
				DICTIONARY *row_dictionary,
				char *application_name,
				LIST *primary_attribute_name_list,
				LIST *insert_required_attribute_name_list,
				LIST *attribute_name_list,
				LIST *ignore_attribute_name_list,
				FILE *insert_tmp_file,
				LIST *mto1_related_folder_list,
				LIST *common_non_primary_attribute_name_list,
				LIST *attribute_list,
				boolean exists_reference_number )
{
	char data_pipe_string[ MAX_INPUT_LINE ];
	int row, highest_index;
	boolean must_populate_at_least_one_non_primary_attribute;
	LIST *non_primary_attribute_name_list;

	must_populate_at_least_one_non_primary_attribute =
		( related_folder_exists_automatic_preselection(
				mto1_related_folder_list ) ||
		exists_reference_number );

	non_primary_attribute_name_list =
		list_subtract_list(
			attribute_name_list,
			primary_attribute_name_list );

	highest_index = get_dictionary_key_highest_index( row_dictionary );

	/* Put each row to insert into the file */
	/* ------------------------------------ */
	for( row = 0; row <= highest_index; row++ )
	{
		if ( !insert_database_get_missing_attribute_name_list(
			row,
			row_dictionary /* query_dictionary */,
			ignore_attribute_name_list,
			primary_attribute_name_list,
			insert_required_attribute_name_list ) )
		{
			if ( must_populate_at_least_one_non_primary_attribute
			&&   !insert_database_non_primary_attribute_populated(
					row,
					row_dictionary,
					non_primary_attribute_name_list ) )
			{
				continue;
			}

			insert_database_set_reference_number(
					application_name,
					row_dictionary,
					attribute_list,
					row );

			build_insert_data_string( 	
					row_dictionary,
					data_pipe_string,
					row,
					ignore_attribute_name_list,
					primary_attribute_name_list,
					attribute_name_list,
					mto1_related_folder_list,
					common_non_primary_attribute_name_list,
					application_name,
					attribute_list );

			fprintf( insert_tmp_file, 
				 "%s\n", 
				 data_pipe_string );
		}
	}
} /* build_insert_tmp_file_each_row() */

void insert_database_execute_post_change_process_row_zero(
				char **message,
				char *application_name,
				char *session,
				DICTIONARY *row_dictionary,
				LIST *ignore_attribute_name_list,
				LIST *primary_attribute_name_list,
				LIST *insert_required_attribute_name_list,
				PROCESS *post_change_process,
				char *login_name,
				char *folder_name,
				char *role_name )
{
	if ( !post_change_process ) return;

	if ( !insert_database_get_missing_attribute_name_list(
		0 /* row */,
		row_dictionary /* query_dictionary */,
		ignore_attribute_name_list,
		primary_attribute_name_list,
		insert_required_attribute_name_list ) )
	{
		process_convert_parameters(
			&post_change_process->executable,
			application_name,
			session,
			"insert" /* state */,
			login_name,
			folder_name,
			role_name,
			(char *)0 /* target_frame */,
			row_dictionary /* parameter_dictionary */,
			row_dictionary /* where_clause_dictionary */,
			post_change_process->attribute_list,
			(LIST *)0 /* prompt_list */,
			primary_attribute_name_list,
			(LIST *)0 /* primary_data_list */,
			0 /* row */,
			post_change_process->process_name,
			(PROCESS_SET *)0,
			(char *)0 /* one2m_folder_name_for_processes */,
			(char *)0 /* operation_row_count_string */,
			(char *)0 /* prompt */ );

		*message = pipe2string( post_change_process->executable );
	}
} /* insert_database_execute_post_change_process_row_zero() */

void insert_database_execute_post_change_process_each_row(
				char **message,
				char *application_name,
				char *session,
				DICTIONARY *row_dictionary,
				LIST *ignore_attribute_name_list,
				LIST *primary_attribute_name_list,
				LIST *insert_required_attribute_name_list,
				PROCESS *post_change_process,
				char *login_name,
				char *folder_name,
				char *role_name,
				LIST *mto1_related_folder_list,
				boolean exists_reference_number,
				LIST *attribute_name_list )
{
	int row, highest_index;
	boolean must_populate_at_least_one_non_primary_attribute;
	LIST *non_primary_attribute_name_list;
	char *results;

	if ( !post_change_process ) return;

	must_populate_at_least_one_non_primary_attribute =
		( related_folder_exists_automatic_preselection(
				mto1_related_folder_list ) ||
		exists_reference_number );

	non_primary_attribute_name_list =
		list_subtract_list(
			attribute_name_list,
			primary_attribute_name_list );

	/* Get the highest row index */
	/* ------------------------- */
	highest_index = get_dictionary_key_highest_index( row_dictionary );

	for( row = 1; row <= highest_index; row++ )
	{
		if ( must_populate_at_least_one_non_primary_attribute
		&&   !insert_database_non_primary_attribute_populated(
				row,
				row_dictionary,
				non_primary_attribute_name_list ) )
		{
			continue;
		}

		if ( !insert_database_get_missing_attribute_name_list(
			row,
			row_dictionary /* query_dictionary */,
			ignore_attribute_name_list,
			primary_attribute_name_list,
			insert_required_attribute_name_list ) )
		{
			/* Reload the command_line each row */
			/* -------------------------------- */
			free( post_change_process->executable );

			process_load_executable(
				&post_change_process->executable,
				post_change_process->process_name,
				application_name );

			process_convert_parameters(
				&post_change_process->executable,
				application_name,
				session,
				"insert" /* state */,
				login_name,
				folder_name,
				role_name,
				(char *)0 /* target_frame */,
				row_dictionary /* parameter_dictionary */,
				row_dictionary /* where_clause_dictionary */,
				post_change_process->attribute_list,
				(LIST *)0 /* prompt_list */,
				(LIST *)0 /* primary_attribute_name_list */,
				(LIST *)0 /* primary_data_list */,
				row,
				post_change_process->process_name,
				(PROCESS_SET *)0,
				(char *)0
				/* one2m_folder_name_for_processes */,
				(char *)0 /* operation_row_count_string */,
				(char *)0 /* prompt */ );

			results =
				pipe2string(
					post_change_process->executable );

			if ( results && *results ) *message = results;
		}
	}
} /* insert_database_execute_post_change_process_each_row() */

void build_insert_tmp_file_row_zero(
				DICTIONARY *row_dictionary,
				char *application_name,
				LIST *primary_attribute_name_list,
				LIST *attribute_name_list,
				LIST *ignore_attribute_name_list,
				FILE *insert_tmp_file,
				LIST *mto1_related_folder_list,
				LIST *common_non_primary_attribute_name_list,
				LIST *attribute_list,
				boolean exists_reference_number )
{
	char data_pipe_string[ MAX_INPUT_LINE ];
	boolean must_populate_at_least_one_non_primary_attribute;
	LIST *non_primary_attribute_name_list;

	if ( !list_length( attribute_name_list ) )
	{
		fprintf(stderr,
			"ERROR: %s/%s()/%d got empty attribute_name_list",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	must_populate_at_least_one_non_primary_attribute =
		( related_folder_exists_automatic_preselection(
				mto1_related_folder_list ) ||
		exists_reference_number );

/*
{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: got must_populate_at_least_one_non_primary_attribute = %d\n",
__FILE__,
__FUNCTION__,
__LINE__,
must_populate_at_least_one_non_primary_attribute );
m2( application_name, msg );
}
*/

	non_primary_attribute_name_list =
		list_subtract_list(
			attribute_name_list,
			primary_attribute_name_list );

	if ( must_populate_at_least_one_non_primary_attribute
	&&   !insert_database_non_primary_attribute_populated(
			0 /* row */,
			row_dictionary,
			non_primary_attribute_name_list ) )
	{
		return;
	}

	insert_database_set_reference_number(
			application_name,
			row_dictionary,
			attribute_list,
			0 /* row */ );

	build_insert_data_string( 	
		row_dictionary,
		data_pipe_string,
		0 /* row */,
		ignore_attribute_name_list,
		primary_attribute_name_list,
		attribute_name_list,
		mto1_related_folder_list,
		common_non_primary_attribute_name_list,
		application_name,
		attribute_list );

	fprintf( insert_tmp_file, 
		 "%s\n", 
		 data_pipe_string );

} /* build_insert_tmp_file_row_zero() */

void insert_database_set_reference_number(
			char *application_name,
			DICTIONARY *dictionary,
			LIST *attribute_list,
			int row )
{
	ATTRIBUTE *attribute;

	if ( !list_rewind( attribute_list ) ) return;

	do {
		attribute = list_get_pointer( attribute_list );

		if ( strcmp( attribute->datatype, "reference_number" ) == 0 )
		{
			char *data;
			int results;

			results = dictionary_get_index_data(
						&data,
						dictionary,
						attribute->attribute_name,
						row );
			if ( results == -1 )
			{
				char reference_string[ 16 ];
				char key[ 16 ];

				sprintf(reference_string,
					"%d",
					appaserver_library_get_reference_number(
						application_name,
						1 /* insert_rows_number */ ) );
				sprintf(key,
					"%s_%d",
					attribute->attribute_name,
					row );
				dictionary_set_pointer(
					dictionary,
					strdup( key ),
					strdup( reference_string ) );
			}
		}
	} while( list_next( attribute_list ) );
} /* insert_database_set_reference_number() */

/* Returns the number of rows inserted */
/* ----------------------------------- */
int insert_database_execute_insert_mysql(
				char **message,
				char *application_name,
				char *folder_name,
				LIST *attribute_name_list,
				char *tmp_file_name,
				LIST *ignore_attribute_name_list )
{
	char sys_string[ 1024 ];
	char message_filename[ 128 ];
	char *results_string;
	int rows_inserted;
	LIST *insert_attribute_name_list;

	sprintf( message_filename, "/tmp/insert_mysql_error_%d", getpid() );

	insert_attribute_name_list =
		list_subtract_list(
			attribute_name_list,
			ignore_attribute_name_list );

	sprintf( sys_string, 
		 "insert_mysql %s %s '%s' %s 2>%s",
		 application_name, 
		 folder_name, 
		 list_display( insert_attribute_name_list ),
		 tmp_file_name,
		 message_filename );

	if ( ! ( results_string = pipe2string( sys_string ) ) )
		rows_inserted = 0;
	else
		rows_inserted = atoi( results_string );

	sprintf(	sys_string,
			"cat %s | html_paragraph_wrapper",
			message_filename );
	*message = pipe_multiple_lines2string( sys_string, LF );

	if ( !*message ) *message = "";

	sprintf( sys_string, "rm -f %s", message_filename );
	system( sys_string );
	return rows_inserted;

} /* insert_database_execute_insert_mysql() */

void build_insert_data_string( 	DICTIONARY *row_dictionary,
				char *destination,
		     		int row,
				LIST *ignore_attribute_name_list,
				LIST *primary_attribute_name_list,
				LIST *attribute_name_list,
				LIST *mto1_related_folder_list,
				LIST *common_non_primary_attribute_name_list,
				char *application_name,
				LIST *attribute_list )
{
	char *attribute_name;
	char *data_ptr;
	char data[ 65536 ];
	int results;
	int first_time = 1;
	ATTRIBUTE *attribute;

	if ( !list_rewind( attribute_name_list ) ) return;

	do {
		attribute_name =
			list_get_pointer(
				attribute_name_list );

		if ( insert_database_ignore_button_pressed(
					ignore_attribute_name_list,
					attribute_name ) )
		{
			continue;
		}

		results = dictionary_get_index_data(
					&data_ptr,
					row_dictionary,
					attribute_name,
					row );

		if ( results == -1 )
		{
			if ( list_exists_string(
				common_non_primary_attribute_name_list,
				attribute_name ) )
			{
				if ( ! (data_ptr =
					insert_database_get_common_data(
					row_dictionary,
					application_name,
					mto1_related_folder_list,
					attribute_name,
					row ) ) )
				{
					fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get common data in row = %d for attribute = %s.\n",
						 __FILE__,
						 __FUNCTION__,
						 __LINE__,
						 row,
						 attribute_name );
					exit( 1 );
				}
			}
			else
			{
				data_ptr = "";
			}
		}

		timlib_strcpy( data, data_ptr, 65535 );

		if ( strcmp( data, FORBIDDEN_NULL ) == 0 )
		{
			strcpy( data, NULL_STRING );
		}
		else
		if ( *data )
		{
			search_replace_special_characters( data );
			escape_special_characters( data );
		}
		else
		{
			strcpy( data, "" );
		}

		/* If there is no data and it's a primary attribute */
		/* then make it "null".				    */
		/* ------------------------------------------------ */
		if ( ( !*data )
		&&   list_exists_string( primary_attribute_name_list,
					 attribute_name ) )
		{
			strcpy( data, NULL_STRING );
		}

		attribute =
			attribute_seek_attribute(
				attribute_list,
				attribute_name );

		if ( attribute
		&&   *data
		&& ( timlib_strcmp(	attribute->datatype,
					"float" ) == 0
		||   timlib_strcmp(	attribute->datatype,
					"integer" ) == 0 ) )
		{
			if ( strcmp( data, NULL_STRING ) != 0 )
			{
				strcpy(	data,
					timlib_trim_money_characters(
					data ) );
			}
		}

		if ( attribute
		&&   *data
		&& ( timlib_strcmp(	attribute->datatype,
					"password" ) == 0 ) )
		{
			char tmp[ 128 ];

			sprintf( tmp, "password('%s')", data );
			strcpy( data, tmp );
		}

		/* Build the output string */
		/* ----------------------- */
		if ( first_time )
		{
			first_time = 0;
			destination += sprintf(
				destination,
				"%s",
				data );
		}
		else
		{
			destination += sprintf(
				destination,
				"^%s",
				data );
		}
	} while( list_next( attribute_name_list ) );
} /* build_insert_data_string() */

LIST *insert_database_get_missing_attribute_name_list(
				int row,
				DICTIONARY *query_dictionary,
				LIST *ignore_attribute_name_list,
				LIST *primary_attribute_name_list,
				LIST *insert_required_attribute_name_list )
{
	LIST *missing_attribute_name_list = {0};

	missing_attribute_name_list = list_new_list();

	insert_database_populate_missing_attribute_name_list(
				&missing_attribute_name_list,
				row,
				query_dictionary,
				ignore_attribute_name_list,
				primary_attribute_name_list );

	insert_database_populate_missing_attribute_name_list(
				&missing_attribute_name_list,
				row,
				query_dictionary,
				ignore_attribute_name_list,
				insert_required_attribute_name_list );

	if ( list_length( missing_attribute_name_list ) )
		return missing_attribute_name_list;
	else
		return (LIST *)0;

} /* insert_database_get_missing_attribute_name_list() */

void insert_database_populate_missing_attribute_name_list(
				LIST **missing_attribute_name_list,
				int row,
				DICTIONARY *query_dictionary,
				LIST *ignore_attribute_name_list,
				LIST *check_attribute_name_list )
{
	char *data, *attribute_name;
	int results;

	if ( !list_rewind( check_attribute_name_list ) ) return;

	do {
		attribute_name = list_get_pointer( check_attribute_name_list );

		if ( insert_database_ignore_button_pressed(
					ignore_attribute_name_list,
					attribute_name ) )
		{
			results = 1;
		}
		else
		{
			results = 
				dictionary_get_index_data_if_populated(
					&data,
					query_dictionary,
					attribute_name,
					row );
		}
		if ( !results )
		{
			if ( !*missing_attribute_name_list )
			{
				*missing_attribute_name_list =
					list_new_list();
			}
			list_append_pointer(
				*missing_attribute_name_list,
				attribute_name );
		}
	} while( list_next( check_attribute_name_list ) );
} /* insert_database_populate_missing_attribute_name_list() */

int insert_database_ignore_button_pressed(
					LIST *ignore_attribute_name_list,
					char *attribute_name )
{
	return list_exists_string(
			ignore_attribute_name_list,
			attribute_name );
}

void insert_database_set_post_insert_process(
				INSERT_DATABASE *insert_database,
				char *post_insert_process )
{
	insert_database->post_insert_process = post_insert_process;
}

void insert_database_set_login_name( 	INSERT_DATABASE *insert_database,
					char *s )
{
	insert_database->login_name = s;
}

char *insert_database_get_common_data(
				DICTIONARY *dictionary,
				char *application_name,
				LIST *mto1_related_folder_list,
				char *attribute_name,
				int row )
{
	RELATED_FOLDER *related_folder;
	LIST *primary_data_list;
	LIST *primary_attribute_name_list;
	char key[ 128 ];
	char *results;

	if ( ! (related_folder =
		insert_database_get_common_data_related_folder(
			attribute_name,
			mto1_related_folder_list ) ) )
	{
		return (char *)0;
	}

	primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			related_folder->folder->attribute_list );

	primary_data_list =
		dictionary_using_list_get_index_data_list(
			dictionary,
			primary_attribute_name_list,
			row );

	results = insert_database_fetch_common_data(
			application_name,
			related_folder->folder->folder_name,
			attribute_name,
			primary_attribute_name_list,
			primary_data_list );

	sprintf( key, "%s_%d", attribute_name, row );
	dictionary_set_pointer( dictionary, strdup( key ), strdup( results ) );

	return results;

} /* insert_database_get_common_data() */

RELATED_FOLDER *insert_database_get_common_data_related_folder(
			char *attribute_name,
			LIST *mto1_related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_related_folder_list ) )
		return (RELATED_FOLDER *)0;

	do {
		related_folder =
			list_get_pointer(
				mto1_related_folder_list );

		if ( list_exists_string(
			folder_get_non_primary_attribute_name_list(
				related_folder->folder->attribute_list ),
			attribute_name ) )
		{
			return related_folder;
		}
	} while( list_next( mto1_related_folder_list ) );

	return (RELATED_FOLDER *)0;

} /* insert_database_get_common_data_related_folder() */

char *insert_database_fetch_common_data(
			char *application_name,
			char *folder_name,
			char *attribute_name,
			LIST *primary_attribute_name_list,
			LIST *primary_data_list )
{
	char sys_string[ 1024 ];
	char *where_clause;
	static char common_data[ 1024 ];

	where_clause =
		insert_database_get_fetch_common_data_where_clause_string( 	
				primary_attribute_name_list,
				primary_data_list );

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=%s			"
		"			where=\"%s\"			",
		application_name,
		attribute_name,
		folder_name,
		where_clause );

	strcpy( common_data, pipe2string( sys_string ) );
	return common_data;

} /* insert_database_fetch_common_data() */

char *insert_database_get_fetch_common_data_where_clause_string( 	
				LIST *primary_attribute_name_list,
				LIST *primary_data_list )
{
	char where_clause[ 65536 ];
	char *where_ptr = where_clause;
	char *primary_attribute_name;
	char *primary_data;
	char data_escaped[ 1024 ];
	char buffer[ 1024 ];

	where_ptr += sprintf( where_ptr, "1 = 1" );

	if ( list_reset( primary_attribute_name_list )
	&&   list_reset( primary_data_list ) )
	{
		do {
			primary_data =
				list_get_pointer( primary_data_list );
			primary_attribute_name =
				list_get_pointer( primary_attribute_name_list );

			escape_character( data_escaped,
					  primary_data,
					  '\'' );

			strcpy( buffer, data_escaped );
			escape_character( data_escaped,
					  buffer,
					  '$' );

			where_ptr +=
				sprintf(
					where_ptr,
					" and %s='%s'",
					primary_attribute_name,
					data_escaped );

			list_next( primary_data_list );
		} while( list_next( primary_attribute_name_list ) );
	}
	return strdup( where_clause );
} /* insert_database_get_fetch_common_data_where_clause_string() */

boolean insert_database_non_primary_attribute_populated(
				int row,
				DICTIONARY *dictionary,
				LIST *non_primary_attribute_name_list )
{
	char *non_primary_attribute_name;
	char *data;
	int results;

	if ( !list_rewind( non_primary_attribute_name_list ) ) return 0;

	do {
		non_primary_attribute_name =
			list_get_pointer(
				non_primary_attribute_name_list );

		results = dictionary_get_index_data(
					&data,
					dictionary,
					non_primary_attribute_name,
					row );
		if ( results != -1 )
		{
 			return 1;
		}
	} while( list_next( non_primary_attribute_name_list ) );

	return 0;

} /* insert_database_non_primary_attribute_populated() */

LIST *insert_database_get_trim_indices_dictionary_key_list(
			DICTIONARY *dictionary )
{
	LIST *dictionary_key_list;

	dictionary_key_list =
		list_trim_indices(
			dictionary_get_key_list( dictionary ) );

	return dictionary_key_list;

} /* insert_database_get_trim_indices_dictionary_key_list() */

LIST *insert_database_set_ignore_primary_key_to_null(
			DICTIONARY *row_dictionary,
			LIST *ignore_attribute_name_list,
			LIST *primary_attribute_name_list )
{
	int highest_index;
	int row;
	char *attribute_name;
	char key[ 128 ];
	LIST *new_ignore_attribute_name_list;

	if ( !list_length( ignore_attribute_name_list ) )
		return ignore_attribute_name_list;

	new_ignore_attribute_name_list = list_new();

	highest_index = get_dictionary_key_highest_index( row_dictionary );

	if ( highest_index )
	{
		for( row = 1; row <= highest_index; row++ )
		{
			list_rewind( ignore_attribute_name_list );
			do {
				attribute_name =
					list_get_pointer(
						ignore_attribute_name_list );

				if ( list_exists_string(
					primary_attribute_name_list,
					attribute_name ) )
				{
					sprintf( key,
						 "%s_%d",
						 attribute_name,
						 row );

					dictionary_set_pointer(
						row_dictionary,
						strdup( key ),
						NULL_STRING );
				}
				else
				{
					if ( !list_exists_string(
						new_ignore_attribute_name_list,
						attribute_name ) )
					{
						list_append_pointer(
						new_ignore_attribute_name_list,
						attribute_name );
					}
				}
			} while( list_next( ignore_attribute_name_list ) );
		}
	}
	else
	{
		do {
			attribute_name =
				list_get_pointer(
					ignore_attribute_name_list );

			if ( list_exists_string(
				primary_attribute_name_list,
				attribute_name ) )
			{
				sprintf( key,
					 "%s_0",
					 attribute_name );

				dictionary_set_pointer(
					row_dictionary,
					strdup( key ),
					NULL_STRING );
			}
			else
			{
				list_append_pointer(
					new_ignore_attribute_name_list,
					attribute_name );
			}
		} while( list_next( ignore_attribute_name_list ) );
	}

	return new_ignore_attribute_name_list;

} /* insert_database_set_ignore_primary_key_to_null() */

