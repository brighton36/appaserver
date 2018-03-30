/* ---------------------------------------------------------------	*/
/* src_appaserver/post_report_writer.c					*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "list_usage.h"
#include "operation_list.h"
#include "piece.h"
#include "column.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "attribute.h"
#include "environ.h"
#include "application.h"
#include "boolean.h"
#include "application_constants.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "post2dictionary.h"
#include "appaserver_user.h"
#include "dictionary.h"
#include "related_folder.h"
#include "session.h"
#include "role.h"
#include "date.h"
#include "date_convert.h"
#include "query.h"
#include "query_attribute_statistics_list.h"
#include "report_writer.h"

/* Constants */
/* --------- */
#define ROOT_FOLDER_APPLICATION_CONSTANT_KEY	"report_writer_root_folder"
#define MAX_ROWS				5000
#define INSERT_UPDATE_KEY			"prompt"
#define DROP_DOWN_ELEMENT_NAME			"report_writer"
#define COOKIE_KEY_PREFIX			"report"
#define FORM_NAME				"report_writer"
#define ONE2M_MTO1_FOLDER_LIST_PREFIX		"one2m_mto1_folder_list_"

#define OUTPUT_FILE_SPREADSHEET		"%s/%s/%s_%d.csv"
#define PREPEND_HTTP_FTP_FILE_SPREADSHEET "%s://%s/%s/%s_%d.csv"
#define FTP_FILE_SPREADSHEET		"/%s/%s_%d.csv"

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
RELATED_FOLDER *state_four_get_root_related_folder(
				char *application_name,
				LIST *one2m_subquery_folder_name_list,
				LIST *mto1_recursive_related_folder_list );

LIST *state_four_get_element_list(
				char *application_name,
				char *folder_name,
				LIST *include_attribute_name_list,
				LIST *append_isa_attribute_list,
				LIST *operation_list_list );

void separate_one2m_mto1_folder_name_list(
				LIST **one2m_subquery_folder_name_list,
				LIST **mto1_join_folder_name_list,
				LIST *one2m_mto1_related_folder_name_list,
				LIST *mto1_recursive_related_folder_list );

LIST *state_four_get_one2m_mto1_folder_name_list(
				DICTIONARY *post_dictionary );

LIST *state_three_get_folder_element_list(
			LIST *exclude_attribute_name_list,
			char *login_name,
			char *application_name,
			char *session,
			char *role_name,
			char *folder_name,
			LIST *append_isa_attribute_list,
			LIST *mto1_append_isa_related_folder_list,
			char *form_name,
			boolean with_append_exclude );

LIST *state_three_get_element_list(
				char *login_name,
				char *application_name,
				char *session,
				char *role_name,
				char *folder_name,
				LIST *append_isa_attribute_list,
				LIST *mto1_append_isa_related_folder_list,
				LIST *exclude_attribute_name_list,
				char *form_name,
				LIST *one2m_mto1_related_folder_name_list );

void state_two_append_one2m_option_data_list(
				LIST *option_data_list,
				LIST *one2m_recursive_related_folder_list,
				LIST *mto1_recursive_related_folder_list,
				char *folder_name );

void state_two_append_mto1_option_data_list(
				LIST *option_data_list,
				LIST *mto1_recursive_related_folder_list );

void state_two_append_mto1_one2m_option_data_list(
				LIST *option_data_list,
				LIST *mto1_recursive_related_folder_list,
				char *folder_name );

void post_state_one(		char *application_name,
				char *session,
				char *login_name,
				char *role_name,
				char *process_name,
				char *database_string,
				char *process_title_initial_capital,
				char *appaserver_mount_point );

void post_state_two(		char *application_name,
				char *session,
				char *login_name,
				char *role_name,
				char *process_name,
				char *appaserver_data_directory,
				char *appaserver_mount_point,
				char *database_string,
				char *process_title_initial_capital );

void post_state_three(		char *application_name,
				char *session,
				char *login_name,
				char *role_name,
				char *process_name,
				char *folder_name,
				char *appaserver_data_directory,
				char *appaserver_mount_point,
				char *database_string,
				char *process_title_initial_capital );

void post_table_state_four(	char *application_name,
				char *session,
				char *login_name,
				char *role_name,
				char *database_string,
				char *folder_name,
				char *appaserver_mount_point,
				DICTIONARY *post_dictionary );

void post_spreadsheet_state_four(
				char *application_name,
				char *session,
				char *login_name,
				char *role_name,
				char *folder_name,
				char *appaserver_mount_point,
				DICTIONARY *post_dictionary );

void post_statistics_state_four(char *application_name,
				char *session,
				char *login_name,
				char *role_name,
				char *folder_name,
				char *appaserver_mount_point,
				char *database_management_system,
				DICTIONARY *post_dictionary );

int main( int argc, char **argv )
{
	char *application_name;
	char *session;
	char *login_name;
	char *role_name;
	char *process_name;
	char *folder_name;
	char *state;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char process_title_initial_capital[ 256 ];

	if ( argc != 8 )
	{
		fprintf( stderr, 
"Usage: %s application session login_name role process folder state\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	process_name = argv[ 5 ];
	folder_name = argv[ 6 ];
	state = argv[ 7 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_relative_source_directory_to_path( application_name );

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( !session_access_process(
			application_name,
			session,
			process_name,
			login_name,
			role_name ) )
	{
		session_access_failed_message_and_exit(
			application_name, session, login_name );
	}

	if ( ( strcmp( state, "three" ) == 0
	||     strcmp( state, "four" ) == 0 )
	&&   !session_access_folder(
				application_name,
				session,
				folder_name,
				role_name,
				"lookup" ) )
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

	appaserver_parameter_file = appaserver_parameter_file_new();

	format_initial_capital( process_title_initial_capital,
				process_name );

	if ( strcmp( state, "one" ) == 0 )
	{
		post_state_one(	application_name,
				session,
				login_name,
				role_name,
				process_name,
				database_string,
				process_title_initial_capital,
				appaserver_parameter_file->
					appaserver_mount_point );
	}
	else
	if ( strcmp( state, "two" ) == 0 )
	{
		post_state_two(	application_name,
				session,
				login_name,
				role_name,
				process_name,
				appaserver_parameter_file->
					appaserver_data_directory,
				appaserver_parameter_file->
					appaserver_mount_point,
				database_string,
				process_title_initial_capital );
	}
	else
	if ( strcmp( state, "three" ) == 0 )
	{
		post_state_three(	application_name,
					session,
					login_name,
					role_name,
					process_name,
					folder_name,
					appaserver_parameter_file->
						appaserver_data_directory,
					appaserver_parameter_file->
						appaserver_mount_point,
					database_string,
					process_title_initial_capital );
	}
	else
	if ( strcmp( state, "four" ) == 0 )
	{
		DICTIONARY *post_dictionary;
		char *output_medium;

		post_dictionary =
			post2dictionary(
				stdin,
				appaserver_parameter_file->
					appaserver_data_directory,
				session );
 
		dictionary_trim_multi_drop_down_index(
			post_dictionary,
			ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER );

		output_medium =
			dictionary_safe_fetch(
				post_dictionary,
				"output_medium_0" );

		if ( strcmp( output_medium, "table" ) == 0 )
		{
			post_table_state_four(
					application_name,
					session,
					login_name,
					role_name,
					database_string,
					folder_name,
					appaserver_parameter_file->
						appaserver_mount_point,
					post_dictionary );
		}
		else
		if ( strcmp( output_medium, "spreadsheet" ) == 0 )
		{
			post_spreadsheet_state_four(
					application_name,
					session,
					login_name,
					role_name,
					folder_name,
					appaserver_parameter_file->
						appaserver_mount_point,
					post_dictionary );
		}
		else
		{
			post_statistics_state_four(
					application_name,
					session,
					login_name,
					role_name,
					folder_name,
					appaserver_parameter_file->
						appaserver_mount_point,
					appaserver_parameter_file->
						database_management_system,
					post_dictionary );
		}
	}

	return 0;

} /* main() */

void post_state_one(	char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *process_name,
			char *database_string,
			char *process_title_initial_capital,
			char *appaserver_mount_point )
{
	DOCUMENT *document;
	FORM *form;
	ELEMENT *element;
	LIST *folder_name_list;
	char action_string[ 1024 ];
	LIST *remember_keystrokes_non_multi_element_name_list;

	if ( strcmp( role_name, "zealot" ) == 0 )
	{
		folder_name_list =
			folder_get_zealot_folder_name_list(
				application_name );
	}
	else
	{
		folder_name_list =
			folder_get_select_folder_name_list(
					application_name,
					login_name,
					role_name );
	}

	document = document_new(
			application_get_title_string(
				application_name ),
			application_name );

	document->output_content_type = 1;

	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "cookie" );
	document_set_javascript_module( document, "form" );
	document_set_javascript_module( document, "form_cookie" );
	document_set_javascript_module( document, "keystrokes" );

	document_output_html_stream( stdout );

	printf( "<head>\n" );

	document_output_each_javascript_source(
				application_name,
				document->javascript_module_list,
				appaserver_mount_point,
				application_get_relative_source_directory(
					application_name ) );

	printf( "</head>\n<body>\n" );

	printf( "<h1>%s</h1>\n", process_title_initial_capital );

	form = form_new( FORM_NAME,
			 (char *)0 /* application_title */ );

	form_set_folder_parameters(	form,
					(char *)0 /* state */,
					login_name,
					application_name,
					database_string,
					session,
					process_name,
					role_name );

	form_set_target_frame( form, PROMPT_FRAME );
	form_set_output_row_zero_only( form );
	form->regular_element_list = list_new();

	sprintf(action_string,
		"%s/post_report_writer?%s+%s+%s+%s+%s+folder+two",
		appaserver_parameter_file_get_cgi_directory(),
		timlib_get_parameter_application_name(
		 	application_name,
			database_string ),
		session,
		login_name,
		role_name,
		process_name );

	form->table_border = 1;

	form_output_heading(
		form->login_name,
		form->application_name,
		form->database_string,
		form->session,
		form->form_name,
		form->post_process,
		action_string,
		form->folder_name,
		form->role_name,
		form->state,
		form->insert_update_key,
		form->target_frame,
		0 /* output_submit_reset_buttons */,
		0 /* not with_prelookup_skip_button */,
		form->submit_control_string,
		form->table_border,
		(char *)0 /* caption_string */,
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_get_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(LIST *)0 /* form_button_list */ );

	if ( !list_length( folder_name_list ) )
	{
		printf( "</form>\n" );
		document_close();
		exit( 0 );
	}

	element = element_new( drop_down, DROP_DOWN_ELEMENT_NAME );

	element_drop_down_set_option_data_list(
		element->drop_down,
		folder_name_list );

	list_append_pointer( form->regular_element_list, element );

	form_output_body(
		&form->current_reference_number,
		form->hidden_name_dictionary,
		form->output_row_zero_only,
		form->row_dictionary_list,
		form->regular_element_list,
		form->viewonly_element_list,
		(char *)0 /* spool_filename */,
		0 /* row_level_non_owner_view_only */,
		application_name,
		login_name,
		(char *)0 /* attribute_not_null */,
		(char *)0 /* appaserver_user_foreign_login_name */ );

	remember_keystrokes_non_multi_element_name_list =
		element_list2remember_keystrokes_non_multi_element_name_list(
			form->regular_element_list );

	form->onload_control_string =
		form_get_remember_keystrokes_onload_control_string(
			form->form_name,
			remember_keystrokes_non_multi_element_name_list,
			(LIST *)0
			     /* remember_keystrokes_multi_element_name_list */,
			(char *)0 /* post_change_javascript */,
			(char *)0 /* cookie_key_prefix */,
			COOKIE_KEY_PREFIX );

	form_append_remember_keystrokes_submit_control_string(
			&form->onclick_keystrokes_save_string,
			form,
			remember_keystrokes_non_multi_element_name_list,
			(LIST *)0
			     /* remember_keystrokes_multi_element_name_list */,
			(char *)0 /* cookie_key_prefix */,
			COOKIE_KEY_PREFIX );

	form_output_trailer_post_change_javascript(
		1 /* output_submit_reset_buttons */,
		0 /* output_insert_flag */,
		form->submit_control_string,
		form->html_help_file_anchor,
		form->onload_control_string,
		(char *)0 /* preprompt_button_control_string */,
		application_name,
		0 /* not with_dynarch_menu */,
		0 /* form_number */,
		(char *)0 /* post_change_javascript */,
		(LIST *)0 /* pair_one2m_related_folder_name_list */,
		(LIST *)0 /* form_button_list */ );

	document_close();

} /* post_state_one() */

void post_state_two(	char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *process_name,
			char *appaserver_data_directory,
			char *appaserver_mount_point,
			char *database_string,
			char *process_title_initial_capital )
{
	DOCUMENT *document;
	FORM *form;
	REPORT_WRITER *report_writer;
	char action_string[ 1024 ];
	LIST *one2m_recursive_related_folder_list;
	LIST *mto1_recursive_related_folder_list;
	LIST *option_data_list;
	ELEMENT *element;
	char *folder_name;
	DICTIONARY *post_dictionary;
	char key[ 128 ];
	LIST *remember_keystrokes_multi_element_name_list;
	char *destination_multi_select_element_name;
	char element_name[ 128 ];
	char *onload_control_string;

	post_dictionary =
		post2dictionary(
			stdin,
			appaserver_data_directory,
			session );
 
	sprintf( key, "%s_0", DROP_DOWN_ELEMENT_NAME );

	folder_name = dictionary_safe_fetch( post_dictionary, key );

	if ( !*folder_name
	||   strcmp( folder_name, "folder_name" ) == 0
	||   strcmp( folder_name, "select" ) == 0 )
	{
/*
	char buffer[ 256 ];
	char title[ 256 ];
		sprintf(title,
	 		"%s %s",
	 		application_get_title_string( application_name ),
			process_title_initial_capital );
*/

		document =
			document_output_menu_new(
				application_name,
				database_string,
				login_name,
				session,
				role_name,
				appaserver_mount_point,
				(char *)0 /* onload_control_string */,
				0 /* not exists_date_attribute */ );

		printf( "<h1>%s</h1>\n", process_title_initial_capital );
		printf( "<h3>Please select a folder.</h3>\n" );
		document_close();
		exit( 0 );
	}

	report_writer =
		report_writer_new(
			application_name,
			session,
			folder_name,
			role_new( application_name, role_name ) );

	form = form_new( FORM_NAME,
			 (char *)0 /* application_title */ );

	form_set_folder_parameters(	form,
					(char *)0 /* state */,
					login_name,
					application_name,
					database_string,
					session,
					process_name,
					role_name );

	form_set_target_frame( form, PROMPT_FRAME );
	form_set_output_row_zero_only( form );
	form->regular_element_list = list_new();

	element = element_new( prompt, "<big>Join</big>" );
	list_append_pointer( form->regular_element_list, element );

	one2m_recursive_related_folder_list =
		report_writer->folder->one2m_recursive_related_folder_list;

	mto1_recursive_related_folder_list =
		report_writer->folder->mto1_recursive_related_folder_list;

	sprintf(element_name,
		"%s%s",
		QUERY_DROP_DOWN_ORIGINAL_STARTING_LABEL,
		DROP_DOWN_ELEMENT_NAME );

	element = element_new( drop_down, strdup( element_name ) );
	element->drop_down->multi_select = 1;
	element->drop_down->multi_select_element_name =
		DROP_DOWN_ELEMENT_NAME;

	option_data_list = list_new();

	state_two_append_one2m_option_data_list(
		option_data_list,
		one2m_recursive_related_folder_list,
		mto1_recursive_related_folder_list,
		folder_name );

	if ( list_length( mto1_recursive_related_folder_list ) )
	{
		state_two_append_mto1_option_data_list(
			option_data_list,
			mto1_recursive_related_folder_list );
	}

	element_drop_down_set_option_data_list(
		element->drop_down,
		option_data_list );

	list_append_pointer( form->regular_element_list, element );

	remember_keystrokes_multi_element_name_list =
		element_list2remember_keystrokes_multi_element_name_list(
			form->regular_element_list );

	onload_control_string =
		form_get_remember_keystrokes_onload_control_string(
			FORM_NAME,
			(LIST *)0
			  /* remember_keystrokes_non_multi_element_name_list */,
			remember_keystrokes_multi_element_name_list,
			(char *)0 /* post_change_javascript */,
			(char *)0 /* cookie_key_prefix */,
			COOKIE_KEY_PREFIX );

	document =
		document_output_menu_new(
			application_name,
			database_string,
			login_name,
			session,
			role_name,
			appaserver_mount_point,
			onload_control_string,
			0 /* not exists_date_attribute */ );

/*
	sprintf(title,
	 	"%s %s %s",
	 	application_get_title_string( application_name ),
	 	format_initial_capital( buffer,
				 	folder_name ),
		process_title_initial_capital );
*/

	printf( "<h1>%s</h1>\n", process_title_initial_capital );

	sprintf(action_string,
		"%s/post_report_writer?%s+%s+%s+%s+%s+%s+three",
		appaserver_parameter_file_get_cgi_directory(),
		timlib_get_parameter_application_name(
		 	application_name,
			database_string ),
		session,
		login_name,
		role_name,
		process_name,
		folder_name );

	form->table_border = 1;

	form_output_heading(
		form->login_name,
		form->application_name,
		form->database_string,
		form->session,
		form->form_name,
		form->post_process,
		action_string,
		form->folder_name,
		form->role_name,
		form->state,
		form->insert_update_key,
		form->target_frame,
		0 /* output_submit_reset_buttons */,
		0 /* not with_prelookup_skip_button */,
		form->submit_control_string,
		form->table_border,
		(char *)0 /* caption_string */,
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_get_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(LIST *)0 /* form_button_list */ );

	form_output_body(
		&form->current_reference_number,
		form->hidden_name_dictionary,
		form->output_row_zero_only,
		form->row_dictionary_list,
		form->regular_element_list,
		form->viewonly_element_list,
		(char *)0 /* spool_filename */,
		0 /* row_level_non_owner_view_only */,
		application_name,
		login_name,
		(char *)0 /* attribute_not_null */,
		(char *)0 /* appaserver_user_foreign_login_name */ );

	form->onload_control_string = document->onload_control_string;

	list_rewind( form->regular_element_list );

	while ( ( destination_multi_select_element_name =
		element_get_destination_multi_select_element_name(
			form->regular_element_list ) ) )
	{
		char submit_control_string[ 128 ];

		sprintf(submit_control_string,
			"post_change_multi_select_all('%s');",
			destination_multi_select_element_name );

		form_append_submit_control_string(
				form->submit_control_string,
				submit_control_string );
	}

	form_append_remember_keystrokes_submit_control_string(
			&form->onclick_keystrokes_save_string,
			form,
			(LIST *)0
			  /* remember_keystrokes_non_multi_element_name_list */,
			remember_keystrokes_multi_element_name_list,
			(char *)0 /* cookie_key_prefix */,
			COOKIE_KEY_PREFIX );

	form_output_trailer_post_change_javascript(
		1 /* output_submit_reset_buttons */,
		0 /* output_insert_flag */,
		form->submit_control_string,
		form->html_help_file_anchor,
		form->onload_control_string,
		(char *)0 /* preprompt_button_control_string */,
		application_name,
		0 /* not with_dynarch_menu */,
		0 /* form_number */,
		(char *)0 /* post_change_javascript */,
		(LIST *)0 /* pair_one2m_related_folder_name_list */,
		(LIST *)0 /* form_button_list */ );

	document_close();

} /* post_state_two() */

void state_two_append_one2m_option_data_list(
			LIST *option_data_list,
			LIST *one2m_recursive_related_folder_list,
			LIST *mto1_recursive_related_folder_list,
			char *folder_name )
{
	RELATED_FOLDER *related_folder;

	if ( list_rewind( one2m_recursive_related_folder_list ) )
	{
		do {
			related_folder =
				list_get(
					one2m_recursive_related_folder_list );

			list_add_string_in_order(
				option_data_list,
				related_folder->
					one2m_related_folder->
					folder_name );

		} while( list_next( one2m_recursive_related_folder_list ) );
	}

	if ( list_length( mto1_recursive_related_folder_list ) )
	{
		state_two_append_mto1_one2m_option_data_list(
			option_data_list,
			mto1_recursive_related_folder_list,
			folder_name );
	}

} /* state_two_append_one2m_option_data_list() */

void state_two_append_mto1_one2m_option_data_list(
			LIST *option_data_list,
			LIST *mto1_recursive_related_folder_list,
			char *folder_name )
{
	RELATED_FOLDER *related_folder;
	LIST *related_one2m_recursive_related_folder_list;
	RELATED_FOLDER *one2m_related_folder;

	if ( !list_rewind( mto1_recursive_related_folder_list ) ) return;

	do {
		related_folder =
			list_get(
				mto1_recursive_related_folder_list );

		if ( !list_rewind(related_folder->
				 folder->
				 one2m_recursive_related_folder_list ) )
		{
			continue;
		}

		related_one2m_recursive_related_folder_list =
			related_folder->
				folder->
				one2m_recursive_related_folder_list;
		do {
			one2m_related_folder =
				list_get(
				related_one2m_recursive_related_folder_list );

			if ( strcmp(	one2m_related_folder->
						one2m_related_folder->
						folder_name,
					folder_name ) == 0 )
			{
				continue;
			}

			if ( !list_length(
				attribute_get_non_primary_attribute_list(
				one2m_related_folder->
					one2m_related_folder->
						attribute_list ) ) )
			{
				continue;
			}

			if ( list_exists_string(
				option_data_list,
				one2m_related_folder->
					one2m_related_folder->
					folder_name ) )
			{
				continue;
			}

			list_add_string_in_order(
				option_data_list,
				one2m_related_folder->
					one2m_related_folder->
					folder_name );

		} while( list_next( 
				related_one2m_recursive_related_folder_list ) );

	} while( list_next( mto1_recursive_related_folder_list ) );

} /* state_two_append_mto1_one2m_option_data_list() */

void state_two_append_mto1_option_data_list(
			LIST *option_data_list,
			LIST *mto1_recursive_related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_recursive_related_folder_list ) ) return;

	do {
		related_folder =
			list_get(
				mto1_recursive_related_folder_list );

		if ( list_length(
			attribute_get_non_primary_attribute_list(
				related_folder->folder->attribute_list ) ) )
		{
/* More work is needed.
			if ( related_folder->related_attribute_name
			&&   *related_folder->related_attribute_name
			&&   strcmp( related_folder->related_attribute_name,
					"null" ) != 0 )
			{
				char option_data[ 256 ];

				sprintf( option_data,
					 "%s (%s)",
					related_folder->folder->folder_name,
					related_folder->
						related_attribute_name );
				list_set_pointer(
					option_data_list,
					strdup( option_data ) );
			}
			else
			{
				list_set_pointer(
					option_data_list,
					related_folder->folder->folder_name );
			}
*/
			if ( related_folder->related_attribute_name
			&&   *related_folder->related_attribute_name
			&&   strcmp( related_folder->related_attribute_name,
					"null" ) != 0 )
			{
				continue;
			}

			if ( !list_exists_string(
				option_data_list,
				related_folder->
					folder->
					folder_name ) )
			{
				list_add_string_in_order(
					option_data_list,
					related_folder->folder->folder_name );
			}
		}

	} while( list_next( mto1_recursive_related_folder_list ) );

} /* state_two_append_mto1_option_data_list() */

void post_state_three(	char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *process_name,
			char *folder_name,
			char *appaserver_data_directory,
			char *appaserver_mount_point,
			char *database_string,
			char *process_title_initial_capital )
{
	DOCUMENT *document;
	FORM *form;
	DICTIONARY *post_dictionary;
	LIST *exclude_attribute_name_list;
	char action_string[ 1024 ];
	FOLDER *folder;
	LIST *one2m_mto1_related_folder_name_list;
	LIST *remember_keystrokes_non_multi_element_name_list;
	LIST *remember_keystrokes_multi_element_name_list;
	char *destination_multi_select_element_name;
	char *onload_control_string;

	folder = folder_with_load_new( 	application_name,
					session,
					folder_name,
					role_new(
						application_name,
						role_name ) );

	post_dictionary =
		post2dictionary(
			stdin,
			appaserver_data_directory,
			session );
 
	dictionary_trim_multi_drop_down_index(
		post_dictionary,
		ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER );

	one2m_mto1_related_folder_name_list =
		dictionary_get_index_list(
			post_dictionary,
			DROP_DOWN_ELEMENT_NAME );

	exclude_attribute_name_list =
		appaserver_get_exclude_attribute_name_list(
			folder->append_isa_attribute_list,
			"lookup" );

	form = form_new( FORM_NAME,
			 (char *)0 /* application_title */ );

	form_set_folder_parameters(	form,
					(char *)0 /* state */,
					login_name,
					application_name,
					database_string,
					session,
					process_name,
					role_name );

	form_set_target_frame( form, EDIT_FRAME );
	form_set_output_row_zero_only( form );

	form->regular_element_list =
		state_three_get_element_list(
			login_name,
			application_name,
			session,
			role_name,
			folder_name,
			folder->append_isa_attribute_list,
			folder->mto1_append_isa_related_folder_list,
			exclude_attribute_name_list,
			FORM_NAME,
			one2m_mto1_related_folder_name_list );

	remember_keystrokes_non_multi_element_name_list =
		element_list2remember_keystrokes_non_multi_element_name_list(
			form->regular_element_list );

	remember_keystrokes_multi_element_name_list =
		element_list2remember_keystrokes_multi_element_name_list(
			form->regular_element_list );

	onload_control_string =
		form_get_remember_keystrokes_onload_control_string(
			FORM_NAME,
			remember_keystrokes_non_multi_element_name_list,
			remember_keystrokes_multi_element_name_list,
			(char *)0 /* post_change_javascript */,
			(char *)0 /* cookie_key_prefix */,
			COOKIE_KEY_PREFIX );

	document =
		document_output_menu_new(
			application_name,
			database_string,
			login_name,
			session,
			role_name,
			appaserver_mount_point,
			onload_control_string,
			attribute_exists_date_attribute(
				folder->
				append_isa_attribute_list ) );

/*
	char title[ 256 ];
	char buffer[ 256 ];
	sprintf(title,
	 	"%s %s %s",
	 	application_get_title_string( application_name ),
	 	format_initial_capital( buffer,
				 	folder_name ),
		process_title_initial_capital );
*/

	printf( "<h1>%s</h1>\n", process_title_initial_capital );

	sprintf(action_string,
		"%s/post_report_writer?%s+%s+%s+%s+%s+%s+four",
		appaserver_parameter_file_get_cgi_directory(),
		timlib_get_parameter_application_name(
		 	application_name,
			database_string ),
		session,
		login_name,
		role_name,
		process_name,
		folder_name );

	form->table_border = 1;

	form_output_heading(
		form->login_name,
		form->application_name,
		form->database_string,
		form->session,
		form->form_name,
		form->post_process,
		action_string,
		form->folder_name,
		form->role_name,
		form->state,
		form->insert_update_key,
		form->target_frame,
		0 /* output_submit_reset_buttons */,
		0 /* not with_prelookup_skip_button */,
		form->submit_control_string,
		form->table_border,
		(char *)0 /* caption_string */,
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_get_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(LIST *)0 /* form_button_list */ );

	form_output_body(
		&form->current_reference_number,
		form->hidden_name_dictionary,
		form->output_row_zero_only,
		form->row_dictionary_list,
		form->regular_element_list,
		form->viewonly_element_list,
		(char *)0 /* spool_filename */,
		0 /* row_level_non_owner_view_only */,
		application_name,
		login_name,
		(char *)0 /* attribute_not_null */,
		(char *)0 /* appaserver_user_foreign_login_name */ );

	list_rewind( form->regular_element_list );

	while ( ( destination_multi_select_element_name =
		element_get_destination_multi_select_element_name(
			form->regular_element_list ) ) )
	{
		char submit_control_string[ 128 ];

		sprintf(submit_control_string,
			"post_change_multi_select_all('%s');",
			destination_multi_select_element_name );

		form_append_submit_control_string(
				form->submit_control_string,
				submit_control_string );
	}

	form_append_remember_keystrokes_submit_control_string(
			&form->onclick_keystrokes_save_string,
			form,
			remember_keystrokes_non_multi_element_name_list,
			remember_keystrokes_multi_element_name_list,
			(char *)0 /* cookie_key_prefix */,
			COOKIE_KEY_PREFIX );

	output_dictionary_as_hidden(
		dictionary_add_prefix(
			post_dictionary,
			ONE2M_MTO1_FOLDER_LIST_PREFIX ) );

	form->onload_control_string = document->onload_control_string;

	form_output_trailer_post_change_javascript(
		1 /* output_submit_reset_buttons */,
		0 /* output_insert_flag */,
		form->submit_control_string,
		form->html_help_file_anchor,
		form->onload_control_string,
		(char *)0 /* preprompt_button_control_string */,
		application_name,
		1 /* with_back_to_top_button */,
		0 /* form_number */,
		(char *)0 /* post_change_javascript */,
		(LIST *)0 /* pair_one2m_related_folder_name_list */,
		(LIST *)0 /* form_button_list */ );

	document_close();

} /* post_state_three() */

LIST *state_three_get_element_list(
			char *login_name,
			char *application_name,
			char *session,
			char *role_name,
			char *folder_name,
			LIST *append_isa_attribute_list,
			LIST *mto1_append_isa_related_folder_list,
			LIST *exclude_attribute_name_list,
			char *form_name,
			LIST *one2m_mto1_related_folder_name_list )
{
	LIST *return_list;
	FOLDER *folder;
	ROLE *role;
	ELEMENT *element;

	role = role_new( application_name, role_name );

	return_list = list_new_list();

	element = element_new( prompt_heading, folder_name );
	list_append_pointer( return_list, element );

	element = element_new( linebreak, (char *)0 );
	list_append_pointer( return_list, element );

	list_append_list(
		return_list,
		state_three_get_folder_element_list(
			exclude_attribute_name_list,
			login_name,
			application_name,
			session,
			role_name,
			folder_name,
			append_isa_attribute_list,
			mto1_append_isa_related_folder_list,
			form_name,
			1 /* with_append_exclude */ ) );

	if ( list_rewind( one2m_mto1_related_folder_name_list ) )
	{
		char *folder_name;

		do {
			folder_name =
				list_get(
					one2m_mto1_related_folder_name_list );

			element = element_new( linebreak, (char *)0 );
			list_append_pointer( return_list, element );

/* Not implemented, yet.
	char without_foreign_attribute_name[ 128 ];
	char with_foreign_attribute_name[ 128 ];

			*with_foreign_attribute_name = '\0';

			if ( character_exists( folder_name, '(' ) )
			{
				strcpy( with_foreign_attribute_name,
					folder_name );

				column( without_foreign_attribute_name,
					0,
					folder_name );

				folder_name =
					strdup(
					     without_foreign_attribute_name );
			}

			if ( *with_foreign_attribute_name )
			{
				element =
					element_new(
						prompt_heading,
						strdup(
						with_foreign_attribute_name ) );
			}
			else
			{
				element =
					element_new(
						prompt_heading,
						folder_name );
			}
*/

			element = element_new(	prompt_heading,
						folder_name );

			list_append_pointer( return_list, element );

			element = element_new( linebreak, (char *)0 );
			list_append_pointer( return_list, element );

			folder =
				folder_with_load_new(
					application_name,
					session,
					folder_name,
					role );

			list_append_list(
				return_list,
				state_three_get_folder_element_list(
					exclude_attribute_name_list,
					login_name,
					application_name,
					session,
					role_name,
					folder_name,
					folder->append_isa_attribute_list,
					folder->
					mto1_append_isa_related_folder_list,
					form_name,
					0 /* not with_append_exclude */ ) );

		} while( list_next( one2m_mto1_related_folder_name_list ) );
	}

	/* ==================== */
	/* Create Output Medium */
	/* ==================== */

	/* Create the double line break */
	/* ---------------------------- */
	element = element_new( linebreak, "" );
	list_append_pointer(
			return_list, 
			element );
	element = element_new(
			prompt,
			"<br>" );
	list_append_pointer(
			return_list, 
			element );
	element = element_new( linebreak, "" );
	list_append_pointer(
			return_list, 
			element );

	/* Create the prompt element */
	/* ------------------------- */
	element = element_new(
			prompt,
			"Output Medium" );

	list_append_pointer(
			return_list, 
			element );

	/* Create the drop down element */
	/* ---------------------------- */
	element = element_new(
			drop_down,
			"output_medium" );

	element->drop_down->option_data_list = list_new();
	list_append_pointer(	element->drop_down->option_data_list,
				"statistics" );
	list_append_pointer(	element->drop_down->option_data_list,
				"table" );
	list_append_pointer(	element->drop_down->option_data_list,
				"spreadsheet" );

	list_append_pointer(
			return_list, 
			element );

	return return_list;

} /* state_three_get_element_list() */

LIST *state_three_get_folder_element_list(
			LIST *exclude_attribute_name_list,
			char *login_name,
			char *application_name,
			char *session,
			char *role_name,
			char *folder_name,
			LIST *append_isa_attribute_list,
			LIST *mto1_append_isa_related_folder_list,
			char *form_name,
			boolean with_append_exclude )
{
	LIST *return_list;
	ATTRIBUTE *attribute;
	RELATED_FOLDER *related_folder;
	LIST *element_list;
	LIST *foreign_attribute_name_list = {0};

	return_list = list_new_list();

	if ( !list_rewind( append_isa_attribute_list ) ) return return_list;

	do {
		attribute = list_get_pointer( append_isa_attribute_list );

		if ( list_exists_string(
				exclude_attribute_name_list,
				attribute->attribute_name ) )
		{
			continue;
		}

		related_folder =
		       related_folder_attribute_consumes_related_folder(
				&foreign_attribute_name_list,
				(LIST *)0 /* done_attribute_name_list */,
			        (LIST *)0 /* omit_update_attribute_name_list */,
				mto1_append_isa_related_folder_list,
				attribute->attribute_name,
				(LIST *)0 /* include_attribute_name_list */ );

		if ( !related_folder )
			goto skip_drop_down;

		if ( related_folder->folder->lookup_before_drop_down )
			goto skip_drop_down;

		if ( list_length(	related_folder->
					folder->
					primary_attribute_name_list ) > 1 )
		{
			goto skip_drop_down;
		}

		list_append_list(
			return_list,
			related_folder_get_drop_down_element_list(
			       (RELATED_FOLDER **)0,
			       application_name,
			       session,
			       role_name,
			       login_name,
			       related_folder->folder->folder_name,
			       related_folder->folder->
					populate_drop_down_process,
			       related_folder->folder->
					attribute_list,
			       foreign_attribute_name_list,
			       1 /* omit_drop_down_new_push_button */,
			       1 /* omit_ignore_push_buttons */,
			       (DICTIONARY *)0
					/* preprompt_dictionary */,
	  		       (char *)0
			       /* no_display_push_button_prefix */,
	  		       (char *)0
			       /* no_display_push_button_heading */,
			       (char *)0 /* post_change_java... */,
			       related_folder->
					hint_message,
			       (LIST *)0 /* role_folder_insert_list */,
			       form_name,
			       0 /* max_drop_down_size */,
			       (LIST *)0 /* common_non_primary_a... */,
			       0 /* not is_primary_attribute */,
			       0 /* row_level_non_owner_view_only */,
			       related_folder->
					folder->
					row_level_non_owner_forbid,
			       related_folder->
					related_attribute_name,
			       1 /* drop_down_multi_select */,
			       related_folder->
					folder->
					no_initial_capital,
			       (char *)0 /* state */,
			       folder_name
			       /* one2m_folder_name_for_processes */,
			       0 /* tab_index */,
			       0 /* set_first_initial_data */,
			       1 /* output_null_option */,
			       1 /* output_not_null_option */,
			       1 /* output_select_option */,
			       (char *)0
			       /* appaserver_user_foreign_login_name */,
			       1 /* prepend_folder_name */,
			       related_folder->omit_lookup_before_drop_down
				) );

		if ( with_append_exclude )
		{
			list_append_list(
				exclude_attribute_name_list,
				related_folder->
					foreign_attribute_name_list );
		}

		continue;

skip_drop_down:

		if ( strcmp( attribute->datatype, "hidden_text" ) == 0 )
		{
			continue;
		}

		element_list =
			attribute_get_attribute_element_list(
				attribute->attribute_name,
				attribute->folder_name
					/* prepend_folder_name */,
				attribute->datatype,
				attribute->post_change_javascript,
				attribute->width,
				attribute->hint_message,
				0 /* not is_primary_attribute */,
				1 /* omit_push_buttons */ );

		if ( element_list )
		{
			list_append_list(
				return_list,
				element_list );

			if ( with_append_exclude )
			{
				list_append_pointer(
					exclude_attribute_name_list,
					attribute->attribute_name );
			}
		}

	} while( list_next( append_isa_attribute_list ) );

	return return_list;

} /* state_three_get_folder_element_list() */

LIST *state_four_get_one2m_mto1_folder_name_list(
			DICTIONARY *post_dictionary )
{
	DICTIONARY *one2m_mto1_related_folder_name_dictionary;

	one2m_mto1_related_folder_name_dictionary =
		dictionary_appaserver_get_prefixed_dictionary(
			post_dictionary,
			ONE2M_MTO1_FOLDER_LIST_PREFIX );

	return dictionary_get_index_list(
			one2m_mto1_related_folder_name_dictionary,
			DROP_DOWN_ELEMENT_NAME );

} /* state_four_get_one2m_mto1_folder_name_list() */

void post_table_state_four(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *database_string,
			char *folder_name,
			char *appaserver_mount_point,
			DICTIONARY *post_dictionary )
{
	LIST *one2m_mto1_related_folder_name_list;
	LIST *one2m_subquery_folder_name_list = {0};
	LIST *mto1_join_folder_name_list = {0};
	QUERY *query;
	char *input_sys_string;
	FOLDER *folder;
	DOCUMENT *document;
	ROLE *role;
	FORM *form;
	OPERATION_LIST_STRUCTURE *operation_list_structure;
	int number_rows_outputted = 0;
	boolean output_submit_reset_buttons_in_trailer = 0;
	RELATED_FOLDER *root_related_folder = {0};

	document = document_new(
			application_get_title_string(
				application_name ),
			application_name );

	document->output_content_type = 1;

	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "cookie" );
	document_set_javascript_module( document, "form" );
	document_set_javascript_module( document, "form_cookie" );
	document_set_javascript_module( document, "keystrokes" );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
				document->application_name,
				document->onload_control_string );

	role = role_new( application_name, role_name );

	folder = folder_with_load_new( 	application_name,
					session,
					folder_name,
					role );

	one2m_mto1_related_folder_name_list =
		state_four_get_one2m_mto1_folder_name_list(
			post_dictionary );

	separate_one2m_mto1_folder_name_list(
		&one2m_subquery_folder_name_list,
		&mto1_join_folder_name_list,
		one2m_mto1_related_folder_name_list,
		folder->mto1_recursive_related_folder_list );

	if ( list_length( folder->mto1_recursive_related_folder_list ) )
	{
		root_related_folder =
			state_four_get_root_related_folder(
				application_name,
				one2m_subquery_folder_name_list,
				folder->mto1_recursive_related_folder_list );
	}

	query = query_new(
			application_name,
			login_name,
			folder_name,
			folder->append_isa_attribute_list,
			post_dictionary /* query_dictionary */,
			(DICTIONARY *)0 /* sort_dictionary */,
			role,
			(LIST *)0 /* where_clause_attribute_name_list */,
			(LIST *)0 /* where_clause_data_list */,
			MAX_ROWS,
			0 /* not_include_root_folder */,
			one2m_subquery_folder_name_list,
			mto1_join_folder_name_list,
			root_related_folder );

	input_sys_string = 
		query_get_sys_string(
			application_name,
			query->query_output->select_clause,
			query->query_output->from_clause,
			query->query_output->where_clause,
			query->query_output->subquery_where_clause,
			query->query_output->order_clause,
			query->max_rows );

	form = form_new( folder_name,
			 application_get_title_string(
				application_name ) );

	form->row_dictionary_list =
		pipe2dictionary_list(	
			input_sys_string, 
	 		attribute_get_lookup_allowed_attribute_name_list(
				folder->append_isa_attribute_list ),
	 		attribute_get_date_attribute_name_list(
				folder->append_isa_attribute_list ),
			FOLDER_DATA_DELIMITER,
			application_name,
			login_name );

	operation_list_structure =
		operation_list_structure_new(
				application_name,
				session,
				folder_name,
				role_name,
				dont_omit_delete );

	form->viewonly_element_list =
		state_four_get_element_list(
			application_name,
			folder_name,
			attribute_get_attribute_name_list(
				folder->append_isa_attribute_list ),
			folder->append_isa_attribute_list,
			operation_list_structure->operation_list );

	form_set_post_process( form, "post_edit_table_form" );
	form_set_current_row( form, 1 );
	form_set_target_frame( form, EDIT_FRAME );

	form_set_folder_parameters(	form,
					"view" /* state_for_heading */,
					login_name,
					application_name,
					database_string,
					session,
					folder->folder_name,
					role_name );

	form_output_title(	form->application_title,
				form->state,
				form->form_title,
				form->folder_name,
				form->target_frame,
				form->subtitle_string,
				0 /* not omit_format_initial_capital */ );

	form->table_border = 1;
	/* form->insert_update_key = insert_update_key; */
	form->target_frame = EDIT_FRAME;
	/* form->html_help_file_anchor = folder->html_help_file_anchor; */
	form->process_id = getpid();

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
		1 /* output_submit_reset_buttons_in_heading */,
		0 /* not with_prelookup_skip_button */,
		form->submit_control_string,
		form->table_border,
		(char *)0 /* caption_string */,
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_get_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(LIST *)0 /* form_button_list */ );

	form_output_table_heading(	form->viewonly_element_list,
					0 /* form_number */ );

	number_rows_outputted += form_output_body(
		&form->current_reference_number,
		form->hidden_name_dictionary,
		form->output_row_zero_only,
		form->row_dictionary_list,
		form->regular_element_list,
		form->viewonly_element_list,
		(char *)0 /* spool_filename */,
		folder->row_level_non_owner_view_only,
		application_name,
		login_name,
		(char *)0 /* attribute_not_null */,
		(char *)0 /* appaserver_user_foreign_login_name */ );

	if ( number_rows_outputted >= ROWS_FOR_SUBMIT_AT_BOTTOM
	&&   list_length( operation_list_structure->operation_list ) )
	{
		output_submit_reset_buttons_in_trailer = 1;
	}

	form_output_trailer(
		output_submit_reset_buttons_in_trailer,
		0 /* output_insert_flag */,
		form->submit_control_string,
		form->html_help_file_anchor,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(char *)0 /* preprompt_button_control_string */,
		application_name,
		1 /* with_back_to_top_button */,
		0 /* form_number */,
		(LIST *)0 /* form_button_list */ );

	document_close();

} /* post_table_state_four() */

void post_statistics_state_four(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *appaserver_mount_point,
			char *database_management_system,
			DICTIONARY *post_dictionary )
{
	LIST *one2m_mto1_related_folder_name_list;
	LIST *one2m_subquery_folder_name_list = {0};
	LIST *mto1_join_folder_name_list = {0};
	QUERY *query;
	ROLE *role;
	char *sys_string;
	FOLDER *folder;
	DOCUMENT *document;
	QUERY_ATTRIBUTE_STATISTICS_LIST *query_attribute_statistics_list;
	char where_clause[ 65536 ];
	RELATED_FOLDER *root_related_folder = {0};

	document = document_new(
			application_get_title_string(
				application_name ),
			application_name );

	document->output_content_type = 1;

	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "cookie" );
	document_set_javascript_module( document, "form" );
	document_set_javascript_module( document, "form_cookie" );
	document_set_javascript_module( document, "keystrokes" );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
				document->application_name,
				document->onload_control_string );

	role = role_new( application_name, role_name );

	folder = folder_with_load_new( 	application_name,
					session,
					folder_name,
					role );

	one2m_mto1_related_folder_name_list =
		state_four_get_one2m_mto1_folder_name_list(
			post_dictionary );

	separate_one2m_mto1_folder_name_list(
		&one2m_subquery_folder_name_list,
		&mto1_join_folder_name_list,
		one2m_mto1_related_folder_name_list,
		folder->mto1_recursive_related_folder_list );

	if ( list_length( folder->mto1_recursive_related_folder_list ) )
	{
		root_related_folder =
			state_four_get_root_related_folder(
				application_name,
				one2m_subquery_folder_name_list,
				folder->mto1_recursive_related_folder_list );
	}

	query = query_new(
			application_name,
			login_name,
			folder_name,
			folder->append_isa_attribute_list,
			post_dictionary /* query_dictionary */,
			(DICTIONARY *)0 /* sort_dictionary */,
			role,
			(LIST *)0 /* where_clause_attribute_name_list */,
			(LIST *)0 /* where_clause_data_list */,
			MAX_ROWS,
			0 /* not_include_root_folder */,
			one2m_subquery_folder_name_list,
			mto1_join_folder_name_list,
			root_related_folder );

	query_attribute_statistics_list =
			query_attribute_statistics_list_new(
					folder->application_name,
					query->query_output->from_clause );

	query_attribute_statistics_list->list =
		query_attribute_statistics_list_get_list(
					folder->attribute_list );

	if ( query->query_output->subquery_where_clause )
	{
		sprintf(where_clause,
			"%s and %s",
			query->query_output->where_clause,
			query->query_output->subquery_where_clause );
	}
	else
	{
		strcpy( where_clause, query->query_output->where_clause );
	}

	sys_string =
	query_attribute_statistics_list_get_build_each_temp_file_sys_string(
			query_attribute_statistics_list->application_name,
			query_attribute_statistics_list->folder_name,
			query_attribute_statistics_list->list,
			where_clause,
			database_management_system );

	if ( !sys_string )
	{
		printf(
"<p>There are no number attributes or dates in this folder to perform statistics on.\n" );
		document_close();
		return;
	}

	system( sys_string );

	query_attribute_statistics_list_populate_list(
			query_attribute_statistics_list->list );

	query_attribute_statistics_list_output_table(
			query_attribute_statistics_list->folder_name,
			where_clause,
			query_attribute_statistics_list->list,
			application_name );

	query_attribute_statistics_remove_temp_file(
					query_attribute_statistics_list->list );

	document_close();

} /* post_statistics_state_four() */

void separate_one2m_mto1_folder_name_list(
			LIST **one2m_subquery_folder_name_list,
			LIST **mto1_join_folder_name_list,
			LIST *one2m_mto1_related_folder_name_list,
			LIST *mto1_recursive_related_folder_list )
{
	char *folder_name;

	if ( !list_rewind( one2m_mto1_related_folder_name_list ) )
		return;

	do {
		folder_name =
			list_get(
				one2m_mto1_related_folder_name_list );

		if ( related_folder_exists_mto1_folder_name(
				mto1_recursive_related_folder_list,
				folder_name ) )
		{
			if ( !*mto1_join_folder_name_list )
			{
				*mto1_join_folder_name_list = list_new();
			}

			list_append_pointer(
				*mto1_join_folder_name_list,
				folder_name );
		}
		else
		{
			if ( !*one2m_subquery_folder_name_list )
			{
				*one2m_subquery_folder_name_list = list_new();
			}

			list_append_pointer(
				*one2m_subquery_folder_name_list,
				folder_name );
		}

	} while( list_next( one2m_mto1_related_folder_name_list ) );

} /* separate_one2m_mto1_folder_name_list() */

LIST *state_four_get_element_list(
			char *application_name,
			char *folder_name,
			LIST *include_attribute_name_list,
			LIST *append_isa_attribute_list,
			LIST *operation_list_list )
{
	LIST *element_list;
	int objects_outputted = {0};
	char *attribute_name;
	ATTRIBUTE *attribute;

	if ( !list_rewind( include_attribute_name_list ) )
		return (LIST *)0;

	element_list = list_new();

	objects_outputted =
		appaserver_library_add_operations(
			element_list,
			objects_outputted,
			operation_list_list,
			folder_name /* delete_isa_only_folder_name */ );

	do {
		attribute_name = 
			list_get_pointer(
				include_attribute_name_list );

		attribute = attribute_seek_attribute( 
				append_isa_attribute_list,
				attribute_name );

		if ( !attribute )
		{
			char msg[ 1024 ];

			sprintf(msg,
		"ERROR in %s/%s(): cannot find attribute = (%s) in list\n",
				__FILE__,
				__FUNCTION__,
				attribute_name );

			appaserver_output_error_message(
				application_name, msg, (char *)0 );

			exit( 1 );
		}

		if ( list_exists_string( attribute->exclude_permission_list,
					 "lookup" ) )
		{
			continue;
		}

		list_append_list(
			element_list,
			appaserver_library_get_update_attribute_element_list(
				&objects_outputted,
				attribute,
				'n' /* update_yn */,
				(LIST *)0 /* primary_attribute_name_list */,
				0 /* not is_primary_attribute */,
				(char *)0 /* folder_post_change_javascript */,
				0 /* not prompt_data_element_only */ ) );

	} while( list_next( include_attribute_name_list ) );

	return element_list;

} /* state_four_get_element_list() */

void post_spreadsheet_state_four(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *appaserver_mount_point,
			DICTIONARY *post_dictionary )
{
	LIST *one2m_mto1_related_folder_name_list;
	LIST *one2m_subquery_folder_name_list = {0};
	LIST *mto1_join_folder_name_list = {0};
	QUERY *query;
	char *input_sys_string;
	FOLDER *folder;
	DOCUMENT *document;
	LIST *row_dictionary_list;
	ROLE *role;
	char output_filename[ 256 ];
	char ftp_filename[ 256 ];
	pid_t process_id = getpid();
	LIST *append_isa_attribute_name_list;
	RELATED_FOLDER *root_related_folder = {0};

	document = document_new(
			application_get_title_string(
				application_name ),
			application_name );

	document->output_content_type = 1;

	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "cookie" );
	document_set_javascript_module( document, "form" );
	document_set_javascript_module( document, "form_cookie" );
	document_set_javascript_module( document, "keystrokes" );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
				document->application_name,
				document->onload_control_string );

	role = role_new( application_name, role_name );

	folder = folder_with_load_new( 	application_name,
					session,
					folder_name,
					role );

	one2m_mto1_related_folder_name_list =
		state_four_get_one2m_mto1_folder_name_list(
			post_dictionary );

	separate_one2m_mto1_folder_name_list(
		&one2m_subquery_folder_name_list,
		&mto1_join_folder_name_list,
		one2m_mto1_related_folder_name_list,
		folder->mto1_recursive_related_folder_list );

	if ( list_length( folder->mto1_recursive_related_folder_list ) )
	{
		root_related_folder =
			state_four_get_root_related_folder(
				application_name,
				one2m_subquery_folder_name_list,
				folder->mto1_recursive_related_folder_list );
	}

	query = query_new(
			application_name,
			login_name,
			folder_name,
			folder->append_isa_attribute_list,
			post_dictionary /* query_dictionary */,
			(DICTIONARY *)0 /* sort_dictionary */,
			role,
			(LIST *)0 /* where_clause_attribute_name_list */,
			(LIST *)0 /* where_clause_data_list */,
			MAX_ROWS,
			0 /* not_include_root_folder */,
			one2m_subquery_folder_name_list,
			mto1_join_folder_name_list,
			root_related_folder );

	input_sys_string = 
		query_get_sys_string(
			application_name,
			query->query_output->select_clause,
			query->query_output->from_clause,
			query->query_output->where_clause,
			query->query_output->subquery_where_clause,
			query->query_output->order_clause,
			query->max_rows );

	sprintf(output_filename,
	 	OUTPUT_FILE_SPREADSHEET,
		appaserver_mount_point,
	 	application_name, 
		folder_name,
		process_id );

	if ( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' )
	{
		sprintf(ftp_filename, 
		 	PREPEND_HTTP_FTP_FILE_SPREADSHEET,
			application_get_http_prefix( application_name ),
		 	appaserver_library_get_server_address(),
		 	application_name,
			folder_name,
		 	process_id );
	}
	else
	{
		sprintf(ftp_filename, 
		 	FTP_FILE_SPREADSHEET,
		 	application_name,
			folder_name,
		 	process_id );
	}

	row_dictionary_list =
		pipe2dictionary_list(	
			input_sys_string, 
	 		attribute_get_lookup_allowed_attribute_name_list(
				folder->append_isa_attribute_list ),
	 		attribute_get_date_attribute_name_list(
				folder->append_isa_attribute_list ),
			FOLDER_DATA_DELIMITER,
			application_name,
			login_name );

	append_isa_attribute_name_list =
		attribute_get_lookup_allowed_attribute_name_list(
			folder->append_isa_attribute_list );

	dictionary_list_output_to_file(
		output_filename,
		row_dictionary_list,
		append_isa_attribute_name_list,
		query_get_display( query->query_output ) );

	printf( "<h1>Spreadsheet Transmission<br></h1>\n" );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );
	
/*
	printf( "<BR><p>Search criteria: %s<hr>\n",
		query_get_display_where_clause(
				query->query_output->where_clause,
				application_name,
				folder_name,
				application_get_is_primary_application(
					application_name ) ) );
*/

	printf( "<p>Selected %d records.</p>\n",
		list_length( row_dictionary_list ) );

	appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );

	document_close();

} /* post_spreadsheet_state_four() */

RELATED_FOLDER *state_four_get_root_related_folder(
			char *application_name,
			LIST *one2m_subquery_folder_name_list,
			LIST *mto1_recursive_related_folder_list )
{
	RELATED_FOLDER *mto1_related_folder;
	RELATED_FOLDER *one2m_related_folder;
	APPLICATION_CONSTANTS *application_constants;
	char *report_writer_root_folder;

	application_constants = application_constants_new();

	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	report_writer_root_folder =
		application_constants_fetch(
			application_constants->dictionary,
			ROOT_FOLDER_APPLICATION_CONSTANT_KEY );

	if ( report_writer_root_folder
	&&   *report_writer_root_folder )
	{
		return related_folder_mto1_seek(
			mto1_recursive_related_folder_list,
			report_writer_root_folder );

	}

	if ( !list_rewind( mto1_recursive_related_folder_list ) )
		return (RELATED_FOLDER *)0;

	do {
		mto1_related_folder =
			list_get_pointer(
				mto1_recursive_related_folder_list );

		if ( !list_rewind( mto1_related_folder->
					folder->
					one2m_related_folder_list ) )
		{
			continue;
		}

		do {
			one2m_related_folder =
				list_get_pointer(
					mto1_related_folder->
						folder->
						one2m_related_folder_list );

			if ( list_exists_string(
				one2m_subquery_folder_name_list,
				one2m_related_folder->
					one2m_related_folder->
					folder_name ) )
			{
				return mto1_related_folder;
			}

		} while( list_next( mto1_related_folder->
					folder->
					one2m_related_folder_list ) );
				
	} while( list_next( mto1_recursive_related_folder_list ) );

	return (RELATED_FOLDER *)0;

} /* state_four_get_root_related_folder() */

