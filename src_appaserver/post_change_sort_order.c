/* $APPASERVER_HOME/src_appaserver/post_change_sort_order.c	*/
/* ------------------------------------------------------------	*/
/* This process is triggered if you select the sort order radio	*/
/* button on the lookup forms with attribute names of		*/
/* sort_order, display_order, or sequence_number.		*/
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
#include "document.h"
#include "application.h"
#include "appaserver_user.h"
#include "dictionary.h"
#include "query.h"
#include "query_attribute_statistics_list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "post2dictionary.h"
#include "environ.h"
#include "decode_html_post.h"
#include "role.h"
#include "role_folder.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
LIST *get_option_list(
				char *application_name,
				LIST *display_attribute_name_list,
				char *folder_name,
				char *where_clause,
				char *sort_order_column,
				boolean with_padding );

void post_change_sort_order_post_change_process_execute(
				PROCESS *post_change_process,
				DICTIONARY *post_dictionary,
				char *folder_name,
				char *application_name );

int get_sort_starting_number(
				char *application_name,
				DICTIONARY *post_dictionary,
				char *folder_name,
				LIST *primary_attribute_name_list,
				char *sort_attribute_name,
				char *dictionary_indexed_prefix );

void change_sort_order_state_one(
				char *application_name,
				FOLDER *folder,
				char *where_clause,
				char *login_name,
				char *session,
				char *database_string,
				char *role_name,
				DICTIONARY *ignore_dictionary );

void change_sort_order_state_two(
				char *application_name,
				FOLDER *folder,
				DICTIONARY *post_dictionary );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *folder_name;
	DOCUMENT *document;
	char decoded_dictionary_string[ MAX_INPUT_LINE ];
	char *dictionary_string;
	DICTIONARY *post_dictionary;
	FOLDER *folder;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *where_clause;
	ROLE *role;
	ROLE_FOLDER *role_folder;
	char *role_name;
	char *database_string = {0};
	char *state;
	DICTIONARY_APPASERVER *dictionary_appaserver;
	QUERY *query;

	if ( argc < 7 )
	{
		fprintf( stderr, 
"Usage: %s login_name application session folder role state [dictionary]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	application_name = argv[ 2 ];

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

	session = argv[ 3 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	state = argv[ 6 ];

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( argc == 8 )
	{
		dictionary_string = argv[ 7 ];

		decode_html_post(	decoded_dictionary_string, 
					dictionary_string );

		post_dictionary = 
			dictionary_index_string2dictionary( 
				decoded_dictionary_string );
	}
	else
	{
		post_dictionary =
			post2dictionary(
				stdin,
				appaserver_parameter_file->
					appaserver_data_directory,
				session );
	}

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				post_dictionary,
				(char *)0 /* application_name */,
				(LIST *)0 /* attribute_list */,
				(LIST *)0 /* operation_name_list */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: exiting early.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	post_dictionary = dictionary_appaserver->working_post_dictionary;

	role_folder = role_folder_new_role_folder(
					application_name,
					session,
					role_name,
					folder_name );

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( role_folder->delete_yn != 'y'
	||   strcmp(	login_name,
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

	role = role_new_role(	application_name,
				role_name );

	folder = folder_with_load_new( 	application_name,
					session,
					folder_name,
					role );

	if ( folder->row_level_non_owner_view_only )
		folder->row_level_non_owner_forbid = 1;

	query = query_folder_new(
			application_name,
			login_name,
			folder_name,
			dictionary_appaserver->query_dictionary,
			role_new( application_name, role_name ) );

#ifdef NOT_DEFINED
	query =	query_new(	application_name,
				login_name,
				folder_name,
				(LIST *)0 /* attribute_list */,
				dictionary_appaserver->query_dictionary,
				(DICTIONARY *)0 /* sort_dictionary */,
				role_new( application_name, role_name ),
				(LIST *)0 /* where_attribute_name_list */,
				(LIST *)0 /* where_attribute_data_list */,
				0 /* max_rows */,
				0 /* not include_root_folder */,
				(LIST *)0
					/* one2m_subquery_folder_name_list */,
				(LIST *)0
					/* mto1_join_folder_name_list */,
				(RELATED_FOLDER *)0
					/* root_related_folder */ );
#endif

	where_clause = query->query_output->where_clause;

	document = document_new( "", application_name );
	document->output_content_type = 1;

	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "sort_order" );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	if ( strcmp( state, "one" ) == 0 )
	{
		change_sort_order_state_one(
				application_name,
				folder,
				where_clause,
				login_name,
				session,
				database_string,
				role_name,
				dictionary_appaserver->
					ignore_dictionary );
	}
	else
	if ( strcmp( state, "two" ) == 0 )
	{
		change_sort_order_state_two(
				application_name,
				folder,
				post_dictionary );
		printf( "<h3>Update Complete</h3\n" );
	}
	else
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: invalid state = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 state );
		exit( 1 );
	}

	document_close();

	exit( 0 );
} /* main() */

void change_sort_order_state_one(
				char *application_name,
				FOLDER *folder,
				char *where_clause,
				char *login_name,
				char *session,
				char *database_string,
				char *role_name,
				DICTIONARY *ignore_dictionary )
{
	FORM *form;
	char action_string[ 512 ];
	char *submit_control_string;
	char *sort_order_column = {0};
	LIST *display_attribute_name_list;
	LIST *ignore_attribute_name_list;
	LIST *option_list;
	LIST *display_list;
	char *option;
	char *display;

	display_attribute_name_list =
		attribute_get_attribute_name_list(
			folder->attribute_list );

	ignore_attribute_name_list =
		appaserver_library_get_no_display_pressed_attribute_name_list(
			ignore_dictionary, 
			display_attribute_name_list );

	display_attribute_name_list = 
		list_subtract( 	display_attribute_name_list, 
				ignore_attribute_name_list );

	sort_order_column =
		appaserver_library_get_sort_attribute_name(
			folder->attribute_list );

	form = form_new( SORT_ORDER_ATTRIBUTE_NAME,
			 application_get_title_string(
				application_name ) );

	sprintf(	action_string,
			"%s/post_change_sort_order?%s+%s+%s+%s+%s+two",
			appaserver_parameter_file_get_cgi_directory(),
			login_name,
			timlib_get_parameter_application_name(
				application_name,
				database_string ),
			session,
			folder->folder_name,
			role_name );

	form->action_string = action_string;

	form_output_heading(
		form->login_name,
		form->application_name,
		form->database_string,
		form->session,
		form->form_name,
		form->post_process,
		form->action_string,
		form->folder_name,
		form->role_name,
		form->state,
		form->insert_update_key,
		form->target_frame,
		0 /* output_submit_reset_buttons_in_heading */,
		0 /* not with_prelookup_skip_button */,
		form->submit_control_string,
		form->table_border,
		(char *)0 /* caption_string */,
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_get_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */ );

	option_list =
		get_option_list(
			application_name,
			display_attribute_name_list,
			folder->folder_name,
			where_clause,
			sort_order_column,
			0 /* not with padding */ );

	display_list =
		get_option_list(
			application_name,
			display_attribute_name_list,
			folder->folder_name,
			where_clause,
			sort_order_column,
			1 /* with padding */ );

#ifdef NOT_DEFINED
	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=%s		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		 "
		 "			order=%s		|"
		 "cat						 ",
		 application_name,
		 list_display( display_attribute_name_list ),
		 folder->folder_name,
		 where_clause,
		 sort_order_column,
		 FOLDER_DATA_DELIMITER );

	input_pipe = popen( sys_string, "r" );
#endif

	printf( "<tr><td>\n" );

	printf( "<select multiple name=\"%s\" size=15>\n",
		SORT_ORDER_ELEMENT_NAME );

	if ( list_rewind( option_list ) )
	{
		list_rewind( display_list );

		do {
			option = list_get_pointer( option_list );
			display = list_get_pointer( display_list );

			printf( "<option value=\"%s\">%s\n",
				option,
				display );

			list_next( display_list );

		} while( list_next( option_list ) );
	}

/*
	while( get_line( input_buffer, input_pipe ) )
	{
		printf( "<option value=\"%s\">%s\n",
			input_buffer,
			input_buffer );
	}

	pclose( input_pipe );
*/

	printf( "</select>\n" );

	printf(
"<td><input type=button value=Up onClick=sort_order_move_up();>\n" );
	printf(
"<td><input type=button value=Down onClick=sort_order_move_down();>\n" );
	printf( "<tr><td>\n" );

	submit_control_string = "sort_order_set_all_selected() &&";

	form_output_submit_button(
			submit_control_string,
			"Submit",
			0 /* form_number */,
			(LIST *)0 /* pair_one2m_related_folder_name_list */ );

	printf( "</table>\n" );
	printf( "</form>\n" );

} /* change_sort_order_state_one() */

void change_sort_order_state_two(
				char *application_name,
				FOLDER *folder,
				DICTIONARY *post_dictionary )
{
	char *table_name;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	int index;
	char key[ 128 ];
	char *data;
	int sort_starting_number;
	char *sort_attribute_name;
	int length_primary_attribute_name_list;
	char primary_data[ 1024 ];

	sort_attribute_name =
		appaserver_library_get_sort_attribute_name(
			folder->attribute_list );

	sort_starting_number =
		get_sort_starting_number(
			application_name,
			post_dictionary,
			folder->folder_name,
			folder->primary_attribute_name_list,
			sort_attribute_name,
			SORT_ORDER_ATTRIBUTE_NAME );

	folder->primary_attribute_name_list =
		attribute_get_primary_attribute_name_list(
			folder->attribute_list );

	length_primary_attribute_name_list =
		list_length( folder->primary_attribute_name_list );

	table_name = get_table_name( application_name, folder->folder_name );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y	|"
		 "sql.e						 ",
		 table_name,
		 list_display( folder->primary_attribute_name_list ) );

	output_pipe = popen( sys_string, "w" );

	for( index = 1;; index++ )
	{
		sprintf( key, "%s_%d", SORT_ORDER_ATTRIBUTE_NAME, index );

		if ( ! ( data = dictionary_fetch( post_dictionary, key ) ) )
		{
			break;
		}

		fprintf( output_pipe,
		 	 "%s^%s^%d\n",
			 piece_multiple(
				primary_data,
				'^',
				data,
				length_primary_attribute_name_list ),
		 	 sort_attribute_name,
		 	 sort_starting_number++ );
	}

	pclose( output_pipe );

	if ( folder->post_change_process )
	{
		post_change_sort_order_post_change_process_execute(
			folder->post_change_process,
			post_dictionary,
			folder->folder_name,
			application_name );
	}

} /* change_sort_order_state_two() */

int get_sort_starting_number(
			char *application_name,
			DICTIONARY *post_dictionary,
			char *folder_name,
			LIST *primary_attribute_name_list,
			char *sort_attribute_name,
			char *dictionary_indexed_prefix )
{
	char sys_string[ 65536 ];
	char *where_clause;
	char select[ 128 ];
	char *results;

	sprintf( select,
		 "min(%s)",
		 sort_attribute_name );

	where_clause =
		query_get_dictionary_where_clause(
			post_dictionary,
			primary_attribute_name_list,
			dictionary_indexed_prefix );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=\"%s\"	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder_name,
		 where_clause );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: fetch returned null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return atoi( results );

} /* get_sort_starting_number() */

void post_change_sort_order_post_change_process_execute(
			PROCESS *post_change_process,
			DICTIONARY *post_dictionary,
			char *folder_name,
			char *application_name )
{
	process_convert_parameters(
		&post_change_process->executable,
		application_name,
		(char *)0 /* database_string */,
		(char *)0 /* session */,
		(char *)0 /* state */,
		(char *)0 /* login_name */,
		folder_name,
		(char *)0 /* role_name */,
		(char *)0 /* target_frame */,
		post_dictionary /* parameter_dictionary */,
		(LIST *)0 /* attribute_list */,
		(LIST *)0 /* prompt_list */,
		(LIST *)0 /* primary_attribute_name_list */,
		(LIST *)0 /* primary_data_list */,
		0 /* row */,
		post_change_process->process_name,
		(PROCESS_SET *)0,
		(char *)0
		/* one2m_folder_name_for_processes */,
		(char *)0 /* operation_row_count_string */,
		(char *)0 /* prompt */ );

	system( post_change_process->executable );

} /* post_change_sort_order_post_change_process_execute() */

LIST *get_option_list(
			char *application_name,
			LIST *display_attribute_name_list,
			char *folder_name,
			char *where_clause,
			char *sort_order_column,
			boolean with_padding )
{
	char padding_process[ 128 ];
	char sys_string[ 1024 ];

if ( with_padding ){};

#ifdef NOT_DEFINED
	if ( with_padding )
	{
		sprintf( padding_process,
			 "delimiter2padded_columns.e '%c' 1 y	|"
			 "sed 's/ /\\&nbsp;/g'			|"
			 "cat					 ",
			 FOLDER_DATA_DELIMITER );
	}
	else
	{
		strcpy( padding_process, "cat" );
	}
#endif
	strcpy( padding_process, "cat" );

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=%s		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		 "
		 "			order=%s		|"
		 "%s						|"
		 "cat						 ",
		 application_name,
		 list_display( display_attribute_name_list ),
		 folder_name,
		 where_clause,
		 sort_order_column,
		 padding_process );

	return pipe2list( sys_string );

} /* get_option_list() */

