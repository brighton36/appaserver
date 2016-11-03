/* src_appaserver/post_change_sort_order.c			*/
/* ------------------------------------------------------------	*/
/* This process is triggered if you select the sort order radio	*/
/* button on some of the lookup forms.				*/
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
void change_sort_order_state_one(
				char *application_name,
				FOLDER *folder,
				char *where_clause,
				char *login_name,
				char *session,
				char *database_string,
				char *role_name );

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

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

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

	folder = folder_new_folder( 	application_name,
					session,
					folder_name );

	role = role_new_role(	application_name,
				role_name );

	folder_load(	&folder->insert_rows_number,
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
			application_name,
			session,
			folder->folder_name,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

	folder->attribute_list =
		attribute_get_attribute_list(
		folder->application_name,
		folder->folder_name,
		(char *)0 /* attribute_name */,
		(LIST *)0 /* mto1_isa_related_folder_list */,
		(char *)0 /* role_name */ );

	if ( folder->row_level_non_owner_view_only )
		folder->row_level_non_owner_forbid = 1;

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
				role_name );
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
				char *role_name )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	FORM *form;
	char action_string[ 512 ];
	char *submit_control_string;
	char *sort_order_column;


	folder->primary_attribute_name_list =
		attribute_get_primary_attribute_name_list(
			folder->attribute_list );

	if ( attribute_list_exists(
				folder->attribute_list,
				SORT_ORDER_ATTRIBUTE_NAME ) )
	{
		sort_order_column = SORT_ORDER_ATTRIBUTE_NAME;
	}
	else
	{
		sort_order_column = DISPLAY_ORDER_ATTRIBUTE_NAME;
	}

	form = form_new( SORT_ORDER_ATTRIBUTE_NAME,
			 application_get_title_string( application_name ) );

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
		form->submit_control_string,
		form->table_border,
		(char *)0 /* caption_string */,
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_get_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */ );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		"
		 "			where=\"%s\"		"
		 "			order=%s		",
		 application_name,
		 list_display( folder->primary_attribute_name_list ),
		 folder->folder_name,
		 where_clause,
		 sort_order_column );

	input_pipe = popen( sys_string, "r" );

	printf( "<tr><td>\n" );

	printf( "<select multiple name=\"%s\" size=15>\n",
		SORT_ORDER_ELEMENT_NAME );

	while( get_line( input_buffer, input_pipe ) )
	{
		printf( "<option value=\"%s\">%s\n",
			input_buffer,
			input_buffer );
	}

	pclose( input_pipe );

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

	folder->primary_attribute_name_list =
		attribute_get_primary_attribute_name_list(
			folder->attribute_list );

	table_name = get_table_name( application_name, folder->folder_name );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql.e",
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

		if ( attribute_list_exists(
					folder->attribute_list,
					SORT_ORDER_ATTRIBUTE_NAME ) )
		{
			fprintf( output_pipe,
			 	 "%s^%s^%d\n",
			 	 data,
			 	 SORT_ORDER_ATTRIBUTE_NAME,
			 	 index );
		}
		else
		{
			fprintf( output_pipe,
			 	 "%s^%s^%d\n",
			 	 data,
			 	 DISPLAY_ORDER_ATTRIBUTE_NAME,
			 	 index );
		}
	}

	pclose( output_pipe );

} /* change_sort_order_state_two() */

