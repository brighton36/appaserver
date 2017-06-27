/* library/form.c							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver form ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "attribute.h"
#include "timlib.h"
#include "form.h"
#include "piece.h"
#include "element.h"
#include "related_folder.h"
#include "dictionary.h"
#include "appaserver_error.h"
#include "document.h"
#include "operation.h"
#include "list.h"
#include "query.h"
#include "application.h"
#include "process_parameter_list.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "pair_one2m.h"

FORM *form_new_form( void )
{
	FORM *form;

	form = (FORM *)calloc( 1, sizeof( FORM ) );
	form->hidden_name_dictionary = dictionary_small_new();
	return form;
}

FORM *form_new( char *form_name, char *application_title )
{
	FORM *form = form_new_form();
	form->form_name = form_name;
	form->application_title = application_title;
	return form;
}

void form_set_folder_parameters(FORM *form,
				char *state,
				char *login_name,
				char *application_name,
				char *database_string,
				char *session,
				char *folder_name,
				char *role_name )
{
	form->state = state;
	form->login_name = login_name;
	form->application_name = application_name;
	form->database_string = database_string;
	form->session = session;
	form->folder_name = folder_name;
	form->role_name = role_name;
	form->drop_down_number_columns = 2;
} /* form_set_folder_parameters() */

void form_set_output_pairs( FORM *form )
{
	form->output_pairs = 1;
}

void form_output_content_type( void )
{
	printf( "Content-type: text/html\n\n" );
}

void form_output_title( char *application_title,
			char *state,
			char *form_title,
			char *folder_name,
			char *target_frame,
			char *subtitle_string,
			boolean omit_format_initial_capital )
{
	char title_buffer[ 512 ], buffer[ 512 ];
	char state_buffer[ 128 ];

/* stub */
/* ---- */
target_frame = "";

	if ( state
	&&   strcmp( state, "query" ) == 0 )
	{
		state = "lookup";
	}

	if ( state && *state )
	{
		strcpy( state_buffer, state );
	}
	else
	{
		*state_buffer = '\0';
	}

	if ( form_title && *form_title )
	{
		strcpy( title_buffer, form_title );
	}
	else
	if ( !folder_name || !*folder_name )
	{
		sprintf(title_buffer,
		 	"%s %s",
		 	state_buffer,
		 	application_title );
	}
	else
	if ( state )
	{
		sprintf(title_buffer,
		 	"%s %s",
		 	state_buffer,
		 	folder_name );
	}
	else
	{
		strcpy( title_buffer, application_title );
	}

	if ( !omit_format_initial_capital )
	{
		printf( "<h1>%s</h1>\n",
			format_initial_capital( buffer, title_buffer ) );
	}
	else
	{
		printf( "<h1>%s</h1>\n", title_buffer );
	}

	if ( subtitle_string && *subtitle_string )
	{
		int position;

		if ( ( position = character_exists( subtitle_string, '^' ) ) )
		{
			if ( position == 1 )
			{
				printf( "<h3>%s</h3>\n", subtitle_string + 1 );
			}
			else
			{
				char sub_subtitle_string[ 256 ];

				piece(	sub_subtitle_string,
					'^',
					subtitle_string,
					1 );

				*( subtitle_string + (position - 1 ) ) = '\0';
				printf( "<h2>%s</h2>\n", subtitle_string );
				printf( "<h3>%s</h3>\n", sub_subtitle_string );
			}
		}
		else
		{
			printf( "<h2>%s</h2>\n", subtitle_string );
		}
	}

	fflush( stdout );

} /* form_output_title() */

void form_output_heading(
			char *login_name,
			char *application_name,
			char *database_string,
			char *session,
			char *form_name,
			char *post_process,
			char *action_string,
			char *folder_name,
			char *role_name,
			char *state,
			char *insert_update_key,
			char *target_frame,
			boolean output_submit_reset_buttons,
			boolean with_prelookup_skip_button,
			char *submit_control_string,
			int table_border,
			char *caption_string,
			char *html_help_file_anchor,
			pid_t process_id,
			char *server_address,
			char *optional_related_attribute_name,
			char *remember_keystrokes_onload_control_string )
{
	char buffer[ 128 ];

	printf( "<form enctype=\"multipart/form-data\" method=post" );

	if ( form_name && *form_name ) printf( " name=\"%s\"", form_name );

	if ( action_string && *action_string )
	{
		printf( " action=\"%s\"",
			action_string );
	}
	else
	{
		if ( !post_process || !*post_process )
		{
			char msg[ 1024 ];
			sprintf( msg,
			"ERROR in %s/%s(): no post process defined",
				 __FILE__,
				 __FUNCTION__ );
				 appaserver_output_error_message(
					application_name, msg, (char *)0 );
			exit( 1 );
		}

		if ( !insert_update_key )
			insert_update_key = FORM_INSERT_UPDATE_KEY_PLACEHOLDER;

		if ( !target_frame || !*target_frame )
			target_frame = EDIT_FRAME;
		if ( !state ) state = "";
		if ( !role_name ) role_name = "";
		if ( !folder_name ) folder_name = "";

		printf( " action=\"%s/%s?%s+%s+%s+%s+%s+%s+%s+%s+%d+%s\"",
			appaserver_library_get_http_prompt(
				appaserver_parameter_file_get_cgi_directory(),
				server_address,
				application_get_ssl_support_yn(
					application_name ),
				application_get_prepend_http_protocol_yn(
					application_name ) ),
			post_process,
			login_name,
			timlib_get_parameter_application_name(
				application_name,
				database_string ),
			session,
			folder_name,
			role_name,
			state,
			insert_update_key,
			target_frame,
			process_id,
			(optional_related_attribute_name)
				? optional_related_attribute_name
				: "null" );
	}

	if ( target_frame && *target_frame )
		printf( " target=\"%s\"", target_frame );

	printf( ">\n" );

	if ( output_submit_reset_buttons )
	{
		form_output_submit_reset_buttons(
			submit_control_string,
			(char *)0 /* button_label */,
			html_help_file_anchor,
			0 /* not with_submit_reset_button_table_tags */,
			remember_keystrokes_onload_control_string,
			application_name,
			0 /* not with_back_to_top_button */,
			with_prelookup_skip_button,
			0 /* form_number */,
			(char *)0 /* post_change_javascript */,
			(LIST *)0 /* pair_one2m_related_folder_name_list */ );
	}

	printf( "<table cellspacing=0 cellpadding=0" );
	if ( table_border ) printf( " border" );
	printf( ">\n" );

	if ( caption_string && *caption_string )
	{
		printf( "<caption align=top><%s>%s</%s></caption>\n",
			FORM_CAPTION_HEADING_TAG,
			format_initial_capital_not_parens(
				buffer, caption_string ),
			FORM_CAPTION_HEADING_TAG );
	}

} /* form_output_heading() */

int form_output_body(	int *form_current_reference_number,
			DICTIONARY *hidden_name_dictionary,
			int output_row_zero_only,
			LIST *row_dictionary_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			char *spool_filename,
			boolean row_level_non_owner_view_only,
			char *application_name,
			char *login_name,
			char *attribute_not_null_string,
			char *appaserver_user_foreign_login_name )
{
	int number_rows_outputted = 0;
	int current_row = 1;
	FILE *spool_file = {0};
	DICTIONARY *data_dictionary = {0};

	if ( spool_filename && *spool_filename )
	{
		spool_file = fopen( spool_filename, "w" );
		if ( !spool_file )
		{
			char msg[ 1024 ];
			sprintf( msg, "ERROR in %s/%s(): cannot open %s",
				 __FILE__,
				 __FUNCTION__,
				 spool_filename );
			appaserver_output_error_message(
				application_name, msg, (char *)0 );
			exit( 1 );
		}
	}

	if( output_row_zero_only )
	{
		if ( list_length( row_dictionary_list ) == 1 )
		{
			data_dictionary =
				list_get_first_pointer(
					row_dictionary_list );
		}

		form_output_row( 	form_current_reference_number,
					hidden_name_dictionary,
					regular_element_list,
					data_dictionary,
					0,
					1 /* push buttons */,
					spool_file,
					row_level_non_owner_view_only,
					application_name,
					login_name,
					(char *)0 /* background_color */,
					appaserver_user_foreign_login_name );

		number_rows_outputted = 1;
	}
	else
	{
		number_rows_outputted += 
			form_output_all_rows(
					form_current_reference_number,
					hidden_name_dictionary,
					row_dictionary_list,
					regular_element_list,
					viewonly_element_list,
					current_row++,
					spool_file,
					row_level_non_owner_view_only,
					application_name,
					login_name,
					attribute_not_null_string,
					appaserver_user_foreign_login_name );
	}

	if ( spool_file ) fclose( spool_file );

	return number_rows_outputted;

} /* form_output_body() */

void form_output_trailer(
			boolean output_submit_reset_buttons,
			boolean output_insert_button,
			char *submit_control_string,
			char *html_help_file_anchor,
			char *remember_keystrokes_onload_control_string,
			char *prelookup_button_control_string,
			char *application_name,
			boolean with_back_to_top_button,
			int form_number )
{
	form_output_trailer_post_change_javascript(
			output_submit_reset_buttons,
			output_insert_button,
			submit_control_string,
			html_help_file_anchor,
			remember_keystrokes_onload_control_string,
			prelookup_button_control_string,
			application_name,
			with_back_to_top_button,
			form_number,
			(char *)0 /* post_change_javascript */,
			(LIST *)0 /* pair_one2m_related_folder_name_list */ );

} /* form_output_trailer() */

void form_output_trailer_post_change_javascript(
			boolean output_submit_reset_buttons,
			boolean output_insert_button,
			char *submit_control_string,
			char *html_help_file_anchor,
			char *remember_keystrokes_onload_control_string,
			char *prelookup_button_control_string,
			char *application_name,
			boolean with_back_to_top_button,
			int form_number,
			char *post_change_javascript,
			LIST *pair_one2m_related_folder_name_list )
{
	printf( "</table>\n" );

	printf( "<table border=1>\n" );

	if ( output_insert_button )
	{
		ELEMENT *element;

		element = element_new( linebreak, "" );
		element_output(	
				(DICTIONARY *)0 /* hidden_name_dictionary */,
				element,
				0 /* row */,
				0 /* with_push_buttons */,
				stdout,
				(char *)0 /* background_color */,
				(char *)0 /* application_name */,
				(char *)0 /* login_name */ );

		element = element_new(
				push_button,
				INSERT_PUSH_BUTTON_NAME );

		element_output(
				(DICTIONARY *)0 /* hidden_name_dictionary */,
				element,
				0 /* row */,
				1 /* with_push_buttons */,
				stdout,
				(char *)0 /* background_color */,
				(char *)0 /* application_name */,
				(char *)0 /* login_name */ );
	}

	if ( output_submit_reset_buttons )
	{
		form_output_submit_reset_buttons(
			submit_control_string,
			(char *)0 /* button_label */,
			html_help_file_anchor,
			1 /* with_submit_reset_button_table_tags */,
			remember_keystrokes_onload_control_string,
			application_name,
			with_back_to_top_button,
			0 /* not with_prelookup_skip_button */,
			form_number,
			post_change_javascript,
			pair_one2m_related_folder_name_list );

		with_back_to_top_button = 0;
	}
	else
	if ( with_back_to_top_button )
	{
		form_output_back_to_top_button();
		with_back_to_top_button = 0;
	}

	if ( prelookup_button_control_string )
	{
		form_output_prelookup_button(
			prelookup_button_control_string,
			with_back_to_top_button );
	}

	printf( "</table>\n" );
	printf( "</form>\n" );

} /* form_output_trailer_post_change_javascript() */

void form_output_row( 		int *form_current_reference_number,
				DICTIONARY *hidden_name_dictionary,
				LIST *element_list,
				DICTIONARY *dictionary,
				int row,
				int with_push_buttons,
				FILE *spool_file,
				boolean row_level_non_owner_view_only,
				char *application_name,
				char *login_name,
				char *background_color,
				char *appaserver_user_foreign_login_name )
{
	ELEMENT *element;
	ELEMENT *non_edit_element;
	char current_value[ 4096 ];
	char *dictionary_login_name = {0};
	boolean set_data_flag;

	if ( !list_reset( element_list ) ) return;

	non_edit_element =
		element_new(	prompt_data_plus_hidden,
				(char *)0 /* name */ );

	if ( dictionary_get_index_data(
			&dictionary_login_name,
			dictionary,
			appaserver_user_foreign_login_name,
			row ) < 0 )
	{
		dictionary_login_name = (char*)0;
	}

	if ( background_color && *background_color )
		printf( "<tr bgcolor=%s>\n", background_color );
	else
		printf( "<tr>\n" );

	do {
		element = list_get_pointer( element_list );
		set_data_flag = 1;

		if ( row_level_non_owner_view_only
		&&   element->element_type == password
		&&   dictionary_login_name
		&&   strcasecmp(	dictionary_login_name,
					login_name ) != 0 )
		{
			non_edit_element->name = element->name;
			element = non_edit_element;
			element_set_data( element, "hidden" );
			set_data_flag = 0;
		}
		else
		if ( row_level_non_owner_view_only
		&&   element->element_type != push_button
		&&   element->element_type != hidden
		&&   dictionary_login_name
		&&   strcasecmp(	dictionary_login_name,
					login_name ) != 0 )
		{
			non_edit_element->name = element->name;
			element = non_edit_element;
		}
		else
		if ( row_level_non_owner_view_only
		&&   element->element_type == push_button
		&&   element->name
		&&   strcmp( element->name, "delete" )  == 0
		&&   dictionary_login_name
		&&   strcasecmp(	dictionary_login_name,
					login_name ) != 0 )
		{
			non_edit_element->name = "";
			element = non_edit_element;
			set_data_flag = 0;
		}
		else
		if ( element->element_type == reference_number )
		{
			element_set_data(	
				element,
				form_get_next_reference_number(
					form_current_reference_number ) );
			set_data_flag = 0;
		}

		if ( set_data_flag )
		{
			*current_value = '\0';

			if ( dictionary
			&&   element->name
			&&   dictionary_get_index_data_multi(
				current_value,
				dictionary,
				element->name,
				row,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) != -1 )
			{
				element_set_data(
					element,
					current_value );
			}
		}

		element_output( hidden_name_dictionary,
				element,
				row,
				with_push_buttons,
				stdout,
				background_color,
				application_name,
				login_name );

		if ( spool_file )
		{
			element_output_as_dictionary(
					spool_file,
					element,
					row );
		}
	} while( list_next( element_list ) );

} /* form_output_row() */

void form_output_sort_buttons(	FILE *output_file,
				LIST *element_list,
				char *form_name )
{
	ELEMENT *element;
	ELEMENT_PUSH_BUTTON *element_push_button;
	char push_button_name[ 512 ];
	LIST *sort_button_type_list = list_new();
	char *sort_button_type;
	char sort_button_type_heading[ 16 ];

	list_append_string( sort_button_type_list, FORM_SORT_LABEL );
	list_append_string( sort_button_type_list, FORM_DESCENDING_LABEL );

	list_rewind( sort_button_type_list );
	do {
		sort_button_type = list_get_pointer( sort_button_type_list );

		if ( element_list
		&&   list_reset( element_list ) )
		{
			printf( "<tr>" );
			do {
				element = list_get_pointer( element_list );
	
				if ( element->element_type == hidden )
					continue;
	
				if ( element->element_type == push_button
				||   element->omit_heading_sort_button )
				{
					fprintf( output_file, "<td></td>" );
					continue;
				}
	
				sprintf(	push_button_name,
						"%s%s%s",
						SORT_BUTTON_PREFIX,
						sort_button_type,
						element->name );
	
				search_replace_string(
					push_button_name,
					"/",
					"," );

				element_push_button =
					element_push_button_new();
	
				/* Trim off the trailing underbar */
				/* ------------------------------ */
				strcpy( sort_button_type_heading,
					sort_button_type );

				sort_button_type_heading[
					strlen(
					sort_button_type_heading ) - 1 ] = '\0';

				/* Replace heading of Descend with Reverse */
				/* --------------------------------------- */
				if ( strcmp(	sort_button_type_heading,
						"descend" ) == 0 )
				{
					strcpy( sort_button_type_heading,
						"reverse" );
				}

				element_push_button_set_heading(
						element_push_button,
						sort_button_type_heading );
	
				element_push_button->onchange_submit_yn = 'y';
				element_push_button->form_name = form_name;

				element_push_button_output(
					output_file,
					push_button_name,
					element_push_button->heading,
					element_push_button->checked,
					0 /* row */,
					element_push_button->onchange_submit_yn,
					element_push_button->form_name,
					(char *)0 /* image_source */,
					(char *)0 /* onclick_keystr...ring */,
					(char *)0 /* onclick_function */ );
	
			} while( next_item( element_list ) );
			fprintf( output_file, "\n" );
		}
	} while( list_next( sort_button_type_list ) );
} /* form_output_sort_buttons() */

void form_output_table_heading(	LIST *element_list,
				int form_number )
{
	ELEMENT *element;
	char buffer[ 1024 ];
	char *heading;
	char *push_button_set_all_control_string;

	if ( !list_rewind( element_list ) ) return;

	printf( "<tr>" );
	do {
		element = list_get_pointer( element_list );

		if ( element->element_type == hidden )
			continue;

		push_button_set_all_control_string = (char *)0;

		heading = element_get_heading(
				&push_button_set_all_control_string,
				element,
				form_number );

		if ( heading && *heading )
		{
			printf( "<th>%s", 
				 format_initial_capital_not_parens(
					buffer, 
					heading ) );

			if ( push_button_set_all_control_string )
			{
				char push_button_name[ 256 ];

				sprintf( push_button_name,
					 "push_button_set_all_%s",
					 element->name );

				printf(
			"<input name=%s type=checkbox onclick=\"%s\">",
					push_button_name,
					push_button_set_all_control_string );
			}
			printf( "</th>" );
		}
		else
		{
			printf( "<th>%s</th>", 
				 format_initial_capital_not_parens(
					buffer, 
					element->name ) );
		}
	} while( next_item( element_list ) );

	printf( "\n" );

} /* form_output_table_heading() */

void form_output_prompt( ELEMENT *element )
{
	char buffer[ 512 ];
	printf( "<td>%s", 
		search_replace( 
			"The ", 
			"", 
			format_initial_capital( 
				buffer, 
				element_get_heading(
					(char **)0,
					element,
					0 /* form_number */ ) ) ) );
} /* form_output_prompt() */

int form_output_insert_rows(	int *form_current_reference_number,
				DICTIONARY *hidden_name_dictionary,
				LIST *element_list,
				int current_row,
				int insert_rows_number,
				DICTIONARY *post_dictionary,
				char *application_name,
				char *login_name )
{
	int number_rows_outputted = 0;

	if ( !insert_rows_number )
		insert_rows_number = FORM_DEFAULT_INSERT_ROWS_NUMBER;

	while ( insert_rows_number-- )
	{
		form_output_row(
			form_current_reference_number,
			hidden_name_dictionary,
			element_list,
			post_dictionary,
			current_row++,
			0 /* without push buttons */,
			(FILE *)0 /* spool_file */,
			0 /* row_level_non_owner_view_only */,
			application_name,
			login_name,
			form_get_background_color( application_name ),
			(char *)0 /* appaserver_user_foreign_login_name */ );

		number_rows_outputted++;

		if ( ! ( ( current_row - 1 ) % FORM_ROWS_BETWEEN_HEADINGS )
		&&   insert_rows_number > 1 )
		{
			form_output_table_heading( element_list, 0 );
		}
	}

	return number_rows_outputted;

} /* form_output_insert_rows() */


LIST *form_get_hydrology_validation_element_list(
				HASH_TABLE *attribute_hash_table,
				LIST *attribute_name_list )
{
	LIST *return_list;
	ATTRIBUTE *attribute;
	char *attribute_name;
	ELEMENT *element = {0};

	return_list = list_new();

	/* For each attribute */
	/* ------------------ */
	if ( list_reset( attribute_name_list ) )
		do {
			attribute_name = list_get( attribute_name_list );

			attribute = (ATTRIBUTE *)hash_table_get( 
							attribute_hash_table,
							attribute_name );

			if ( !attribute )
			{
				fprintf(stderr,
				"%s cannot find attribute = (%s)\n",
					__FUNCTION__,
					attribute_name );
				exit( 1 );
			}

			/* Ignore station and datatype */
			/* --------------------------- */
			if ( strcmp( 	attribute->attribute_name,
					"station" ) == 0 
			||   strcmp(	attribute->attribute_name,
					"datatype" ) == 0 )
			{
				continue;
			}
			else
			/* ----------------------------------------- */
			/* Measurement value get a text item element */
			/* ----------------------------------------- */
			if ( strcmp( 	attribute->attribute_name, 
					"measurement_value" ) == 0 )
			{
				element = element_new( 	text_item,
							attribute->
							attribute_name);

				element_text_item_set_attribute_width(
						element->text_item, 
						attribute->width );

				element_text_item_set_heading(
						element->text_item,
						attribute->attribute_name );
			}
			else
			/* --------------------------------------------- */
			/* Measurement date and time get hidden elements */
			/* --------------------------------------------- */
			if ( strcmp( 	attribute->attribute_name,
					"measurement_date" ) == 0 
			||   strcmp(	attribute->attribute_name,
					"measurement_time" ) == 0 )
			{
				element = element_new(	prompt_data,
							attribute->
							attribute_name);
				element_prompt_data_set_heading(
						element->prompt_data,
						element->name );
				list_append( 	return_list, 
						element, 
						sizeof( ELEMENT ) );

				element = element_new( 	hidden,
							attribute->
							attribute_name);
			}

			list_append( 	return_list, 
					element, 
					sizeof( ELEMENT ) );

		} while( list_next( attribute_name_list ) );
	return return_list;
} /* form_get_hydrology_validation_element_list() */

void form_output_back_to_top_button( void )
{
	printf(
"<td valign=bottom><a onClick=\"%s\"><img src=\"/%s/top.png\"></a>\n",
		"window.scrollTo(0,0)",
		IMAGE_RELATIVE_DIRECTORY );
}

void form_output_remember_keystrokes_button( char *control_string )
{
/*
	printf(
"<td valign=bottom><input type=\"button\" value=\"Recall\" onClick=\"%s\">\n",
		control_string );
*/
	printf(
"<input type=\"button\" value=\"Recall\" onClick=\"%s\">\n",
		control_string );
}

void form_output_prelookup_button(	char *control_string,
					boolean with_back_to_top_button )
{
	printf(
"</form><form enctype=\"multipart/form-data\" name=prelookup_button method=post action=\"%s\" target=%s>\n",
		control_string,
		PROMPT_FRAME );

	if ( with_back_to_top_button )
		form_output_back_to_top_button();

	printf(
"<td><input type=button value=\"Back to Prelookup\" onClick=\"document.forms[1].submit();\"></td>\n" );

} /* form_output_prelookup_button() */

void form_set_remember_keystrokes_button_in_heading( FORM *form )
{
	form->remember_keystrokes_button_in_heading = 1;
}

void form_set_remember_keystrokes_button_in_trailer( FORM *form )
{
	form->remember_keystrokes_button_in_trailer = 1;
}

void form_set_output_content_type( FORM *form )
{
	form->output_content_type = 1;
}

void form_set_output_row_zero_only( FORM *form )
{
	form->output_row_zero_only = 1;
}

void form_set_post_process( FORM *form, char *post_process )
{
	form->post_process = post_process;
}

void form_set_insert_rows_number( FORM *form, int insert_rows_number )
{
	form->insert_rows_number = insert_rows_number;
}

void form_set_output_table_heading( FORM *form )
{
	form->output_table_heading = 1;
}

void form_set_current_row( FORM *form, int current_row )
{
	form->current_row = current_row;
}

int form_output_all_rows(	int *form_current_reference_number,
				DICTIONARY *hidden_name_dictionary,
				LIST *row_dictionary_list,
				LIST *regular_element_list,
				LIST *viewonly_element_list,
				int current_row,
				FILE *spool_file,
				boolean row_level_non_owner_view_only,
				char *application_name,
				char *login_name,
				char *attribute_not_null_string,
				char *appaserver_user_foreign_login_name )
{
	DICTIONARY *row_dictionary;
	int number_rows_outputted = 0;
	char *background_color = (char *)0;
	int row_dictionary_list_length;
	LIST *apply_element_list;
	char *data;

	if ( ! list_rewind( row_dictionary_list ) )
	{
		return 0;
	}

	row_dictionary_list_length = list_length( row_dictionary_list );

	do {
		row_dictionary = list_get_pointer( row_dictionary_list );

		if ( viewonly_element_list
		&&   !regular_element_list )
		{
			apply_element_list = viewonly_element_list;
		}
		else
		{
			apply_element_list = regular_element_list;
		}

		/* If is participating and not logged in as supervisor role. */
		/* --------------------------------------------------------- */
		if ( viewonly_element_list && attribute_not_null_string )
		{
			data = dictionary_get(	row_dictionary,
						attribute_not_null_string );

			/* If not null. */
			/* ------------ */
			if ( data && *data )
			{
				apply_element_list = viewonly_element_list;
			}
		}

		if ( !list_length( apply_element_list ) )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: empty apply_element_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		if ( row_dictionary_list_length > 1 )
		{
			background_color =
				form_get_background_color( application_name );
		}

		form_output_row(
			form_current_reference_number,
			hidden_name_dictionary,
			apply_element_list,
			row_dictionary,
			current_row++,
			1 /* with push buttons */,
			spool_file,
			row_level_non_owner_view_only,
			application_name,
			login_name,
			background_color,
			appaserver_user_foreign_login_name );

		number_rows_outputted++;

		if ( ! ( ( current_row - 1 ) % FORM_ROWS_BETWEEN_HEADINGS )
		&&   !   list_at_end( row_dictionary_list ) )
		{
			form_output_table_heading(
				regular_element_list,
				0 /* form_number */ );
		}

	} while( list_next( row_dictionary_list ) );
	return number_rows_outputted;
} /* form_output_all_rows() */

void form_set_row_dictionary_list( FORM *form, LIST *row_dictionary_list )
{
	form->row_dictionary_list = row_dictionary_list;
}

void form_set_target_frame( FORM *form, char *target_frame )
{
	form->target_frame = target_frame;
}

void form_set_insert_update_key( FORM *form, char *insert_update_key )
{
	form->insert_update_key = insert_update_key;
}

void form_set_operation_list( FORM *form, LIST *operation_list )
{
	form->operation_list = operation_list;
}

void form_set_drop_down_number_columns(	FORM *form,
					int drop_down_number_columns )
{
	form->drop_down_number_columns = drop_down_number_columns;
}

/*
void form_set_output_tri( FORM *form )
{
	form->output_tri = 1;
}
*/

void form_set_post_dictionary_string(	FORM *form,
					char *post_dictionary_string )
{
	form->post_dictionary_string = post_dictionary_string;
}

void form_set_output_multi_selects_first( FORM *form )
{
	form->output_multi_selects_first = 1;
}

void form_set_output_dynamic_drop_down(	
				FORM *form,
				char *element_name_string_array_string,
				char *option_value_string_array_string,
				char *option_display_string_array_string )
{
	form->element_name_string_array_string = 
		strdup( element_name_string_array_string );
	form->option_value_string_array_string = 
		strdup( option_value_string_array_string );
	form->option_display_string_array_string = 
		strdup( option_display_string_array_string );
	form->output_dynamic_drop_down = 1;
}


void form_set_omit_output_body( FORM *form )
{
	form->omit_output_body = 1;
}

void form_set_omit_output_html( FORM *form )
{
	form->omit_output_html = 1;
}

void form_dont_set_data_as_current_value( FORM *form )
{
	form->dont_set_data_as_current_value = 1;
}

void form_set_operation_button_in_trailer( 
				FORM *form, 
				char *operation_button_in_trailer_sys_string,
				char *operation_button_in_trailer_prompt )
{
	form->operation_button_in_trailer_sys_string =
		strdup( operation_button_in_trailer_sys_string );
	form->operation_button_in_trailer_prompt =
		strdup( operation_button_in_trailer_prompt );
}

void form_dont_output_document_heading( FORM *form )
{
	form->dont_output_document_heading = 1;
}

LIST *form_dictionary2hidden_element_list( DICTIONARY *dictionary )
{
	LIST *element_list;
	LIST *key_list;
	char *key, *data;
	ELEMENT *element;

	element_list = list_new();
	key_list = dictionary_get_key_list( dictionary );

	if ( key_list && list_rewind( key_list ) )
	{
		do {
			key = list_get( key_list );
			element = element_new( hidden, key );
			data = dictionary_get( dictionary, key );
			element_set_data( element, data );
			list_append( 	element_list,
					element,
					sizeof( ELEMENT ) );
		} while( list_next( key_list ) );
	}
	return element_list;
} /* form_dictionary2hidden_element_list() */


void form_set_action( FORM *form, char *action_string )
{
	form->action_string = strdup( action_string );
}

void form_set_reference_number( FORM *form,
				char *reference_number )
{
	form->current_reference_number = atoi( reference_number );
}

char *form_get_next_reference_number( int *form_current_reference_number )
{
	int current_reference_number;
	static char return_reference_number[ 128 ];

	current_reference_number = *form_current_reference_number;

	if ( !current_reference_number ) current_reference_number = 1;
	*form_current_reference_number = current_reference_number + 1;
	sprintf( return_reference_number, 
		 "%d", 
		 current_reference_number );
	return return_reference_number;
} /* form_get_next_reference_number() */


void form_set_dont_output_operations( FORM *form )
{
	form->dont_output_operations = 1;
}

char *form_get_remember_keystrokes_onload_control_string(
					char *form_name,
					LIST *non_multi_element_name_list,
					LIST *multi_element_name_list,
					char *post_change_javascript,
					char *cookie_key_prefix,
					char *cookie_key )
{
	char buffer[ 4096 ];
	char *local_form_name;
	char cookie_key_buffer[ 128 ];

	form_build_cookie_key_buffer(	cookie_key_buffer,
					cookie_key_prefix,
					cookie_key );

	local_form_name =
		(form_name && *form_name) 
			? form_name : "unknown";

	sprintf( buffer,
		 "keystrokes_onload(document.%s,'<%s>','%s','%c','%c');",
		 local_form_name,
		 cookie_key_buffer,
		 list_display_delimited(
				non_multi_element_name_list,
				FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER ),
		 FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER,
		 ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER );

	if ( multi_element_name_list && list_length( multi_element_name_list ) )
	{
		sprintf( buffer + strlen( buffer ),
"keystrokes_multi_onload(document.%s,'<multi_%s>','%s','%c','%c','%c');",
		 	local_form_name,
		 	cookie_key_buffer,
		 	list_display_delimited(
				multi_element_name_list,
				FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER ),
		 	FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER,
			ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER,
		 	ELEMENT_MULTI_SELECT_REMEMBER_DELIMITER );
	}

	if ( post_change_javascript
	&&   *post_change_javascript )
	{
		sprintf( buffer + strlen( buffer ),
			 "%s;",
			 post_change_javascript );
	}

	/* form->onload_control_string = strdup( buffer ); */
	return strdup( buffer );

} /* form_get_remember_keystrokes_onload_control_string() */

/* Appends to form->submit_control_string */
/* -------------------------------------- */
void form_append_remember_keystrokes_submit_control_string(
				char **onclick_keystrokes_save_string,
				FORM *form,
				LIST *non_multi_element_name_list,
				LIST *multi_element_name_list,
				char *cookie_key_prefix,
				char *cookie_key )
{
	char buffer[ 4096 ];
	char *form_name;
	char cookie_key_buffer[ 128 ];

	form_build_cookie_key_buffer(	cookie_key_buffer,
					cookie_key_prefix,
					cookie_key );

	form_name = (form->form_name && *form->form_name) 
				? form->form_name : "unknown";

	sprintf( buffer,
		 "keystrokes_save(document.%s,'<%s>','%s','%c','%c') && ",
		 form_name,
		 cookie_key_buffer,
		 list_display_delimited(
				non_multi_element_name_list,
				FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER ),
		 FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER,
		 ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER );

	if ( multi_element_name_list && list_length( multi_element_name_list ) )
	{
		sprintf( buffer + strlen( buffer ),
"keystrokes_multi_save(document.%s,'<multi_%s>','%s','%c','%c','%c') && ",
		 	form_name,
		 	cookie_key_buffer,
		 	list_display_delimited(
				multi_element_name_list,
				FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER ),
		 	FORM_KEYSTROKES_ELEMENT_NAME_DELIMITER,
		 	ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER,
		 	ELEMENT_MULTI_SELECT_REMEMBER_DELIMITER );
	}

	*onclick_keystrokes_save_string = strdup( buffer );

	form_append_submit_control_string(
		form->submit_control_string, buffer );

} /* form_append_remember_keystrokes_submit_control_string() */

void form_append_submit_control_string( char *form_submit_control_string,
					char *control_string )
{
	if ( form_submit_control_string && control_string )
	{
		int str_len = strlen( control_string );

		if ( *(control_string + str_len - 1) == ';' )
			*(control_string + str_len - 1) = '\0';

		if (	strlen(	form_submit_control_string ) +
			strlen( control_string ) >=
				FORM_SUBMIT_CONTROL_STRING_SIZE )
		{
			fprintf(stderr,
		"Error in %s/%s()/%d: submit control string is too big.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	
		strcat( form_submit_control_string, control_string );

		str_len = strlen( form_submit_control_string );
		if ( *(form_submit_control_string + str_len - 1 ) != '&'
		&&   *(form_submit_control_string + str_len - 2 ) != '&'
		&&   *(form_submit_control_string + str_len - 3 ) != '&' )
		{
			strcat( form_submit_control_string, "&&" );
		}
	}

} /* form_append_submit_control_string() */

char *form_get_onload_control_string( FORM *form )
{
	return form->onload_control_string;
}

char *form_get_remember_keystrokes_button_control_string( FORM *form )
{
	return form->remember_keystrokes_button_control_string;
}

char *form_get_submit_control_string( FORM *form )
{
	return form->submit_control_string;
}

void form_set_table_border( FORM *form )
{
	form->table_border = 1;
}

char *form_get_background_color( char *application_name )
{
	static int cycle_count = 0;
	static char **background_color_array = {0};
	static int background_color_array_length = 0;
	char *background_color;

	if ( !background_color_array )
	{
		background_color_array =
			form_get_background_color_array(
				&background_color_array_length,
				application_name );
	}

	if ( !background_color_array_length )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty background_color_array.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	background_color = background_color_array[ cycle_count ];
	if ( ++cycle_count == background_color_array_length )
		cycle_count = 0;

	return background_color;
} /* form_get_background_color() */

void form_build_cookie_key_buffer(	char *cookie_key_buffer,
					char *cookie_key_prefix,
					char *cookie_key )
{
	if ( cookie_key_prefix && cookie_key )
	{
		sprintf(	cookie_key_buffer,
				"%s_%s",
				cookie_key_prefix,
				cookie_key );
	}
	else
	if ( cookie_key )
	{
		strcpy( cookie_key_buffer, cookie_key );
	}
	else
	if ( cookie_key_prefix )
	{
		strcpy( cookie_key_buffer, cookie_key_prefix );
	}
	else
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty parameters.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
} /* form_build_cookie_key_buffer() */

void form_set_new_button_onclick_keystrokes_save_string(
				LIST *element_list,
				char *onclick_keystrokes_save_string )
{
	ELEMENT *element;

	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get_pointer( element_list );

		if ( element->element_type == push_button
		&&   timlib_strncmp(element->name,
				    VERTICAL_NEW_PUSH_BUTTON_PREFIX ) == 0 )
		{
			element->push_button->onclick_keystrokes_save_string =
				onclick_keystrokes_save_string;
		}
	} while( list_next( element_list ) );
} /* form_set_new_button_onclick_keystrokes_save_string() */

void form_execute_output_prompt_edit_form(
			char *application_name,
			char *database_string,
			char *login_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *state,
			char *error_file,
			DICTIONARY_APPASERVER *dictionary_appaserver,
			char *target_frame )
{
	char sys_string[ 65536 ];

	if ( !target_frame || !*target_frame ) target_frame = EDIT_FRAME;

	sprintf(sys_string,
		"output_prompt_edit_form %s %s %s %s %s %s %s \"%s\" 2>>%s",
			login_name,
			timlib_get_parameter_application_name(
				application_name,
				database_string ),
			session,
			folder_name,
			role_name,
			state,
			target_frame,
			dictionary_appaserver_get_escaped_dictionary_string(
				dictionary_appaserver,
				0 /* not with_non_prefixed_dictionary */ ),
			error_file );

	system( sys_string );
	/* ------------------------------------------------------------- */
	/* Place exit( 0 ); after calling this function for readability. */
	/* ------------------------------------------------------------- */
} /* form_execute_output_prompt_edit_form() */

void form_execute_output_prompt_insert_form(
			char *application_name,
			char *database_string,
			char *login_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *state,
			char *error_file,
			DICTIONARY_APPASERVER *dictionary_appaserver )
{
	char sys_string[ 65536 ];

	sprintf(sys_string,
		"output_prompt_insert_form %s %s %s %s %s %s n \"%s\" 2>>%s",
			login_name,
			timlib_get_parameter_application_name(
				application_name,
				database_string ),
			session,
			folder_name,
			role_name,
			state,
			dictionary_appaserver_get_escaped_dictionary_string(
				dictionary_appaserver,
				0 /* not with_non_prefixed_dictionary */ ),
			error_file );

	system( sys_string );
	/* ----------------------------------------------------- */
	/* Place exit( 0 ); in the calling code for readability. */
	/* ----------------------------------------------------- */
} /* form_execute_output_prompt_insert_form() */

void form_execute_output_prompt_process_form(
			char *application_name,
			char *database_string,
			char *login_name,
			char *session,
			char *process_name,
			char *role_name,
			char *error_file,
			DICTIONARY *post_dictionary )
{
	char sys_string[ 65536 ];

	if ( post_dictionary && dictionary_length( post_dictionary ) )
	{
		char escaped_dictionary_string[ 65536 ];

		escape_character(	escaped_dictionary_string,
					dictionary_display_delimited(
						post_dictionary, '&' ),
					'$' );

		sprintf(sys_string,
		"output_prompt_process_form %s %s %s '%s' '%s' y \"%s\" 2>>%s",
			login_name,
			timlib_get_parameter_application_name(
				application_name,
				database_string ),
			session,
			process_name,
			role_name,
			escaped_dictionary_string,
			error_file );
	}
	else
	{
		sprintf(sys_string,
		"output_prompt_process_form %s %s %s '%s' '%s' y 2>>%s",
			login_name,
			timlib_get_parameter_application_name(
				application_name,
				database_string ),
			session,
			process_name,
			role_name,
			error_file );
	}

	system( sys_string );
	/* ---------------------------------------------------- */
	/* Place exit( 0 ) in the calling code for readability. */
	/* ---------------------------------------------------- */
} /* form_execute_output_prompt_process_form() */

char **form_get_background_color_array(
			int *background_color_array_length,
			char *application_name )
{
	char **background_color_array;
	char *color_from_application_constants;
	APPLICATION_CONSTANTS *application_constants;

	background_color_array =
		(char **)calloc(FORM_MAX_BACKGROUND_COLOR_ARRAY,
				sizeof( char * ) );

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	if ( ( color_from_application_constants =
		application_constants_fetch(
			application_constants->dictionary,
			"color1" ) ) )
	{
		background_color_array[ 0 ] = color_from_application_constants;
		*background_color_array_length = 1;

		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color2" ) ) )
		{
			background_color_array[ 1 ] =
				color_from_application_constants;
			*background_color_array_length = 2;
		}
		else
		{
			background_color_array[ 1 ] = "white";
		}

		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color3" ) ) )
		{
			background_color_array[ 2 ] =
				color_from_application_constants;
			*background_color_array_length = 3;
		}
		else
		{
			background_color_array[ 2 ] = "white";
		}


		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color4" ) ) )
		{
			background_color_array[ 3 ] =
				color_from_application_constants;
			*background_color_array_length = 4;
		}
		else
		{
			background_color_array[ 3 ] = "white";
		}


		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color5" ) ) )
		{
			background_color_array[ 4 ] =
				color_from_application_constants;
			*background_color_array_length = 5;
		}
		else
		{
			background_color_array[ 4 ] = "white";
		}


		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color6" ) ) )
		{
			background_color_array[ 5 ] =
				color_from_application_constants;
			*background_color_array_length = 6;
		}
		else
		{
			background_color_array[ 5 ] = "white";
		}


		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color7" ) ) )
		{
			background_color_array[ 6 ] =
				color_from_application_constants;
			*background_color_array_length = 7;
		}
		else
		{
			background_color_array[ 6 ] = "white";
		}


		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color8" ) ) )
		{
			background_color_array[ 7 ] =
				color_from_application_constants;
			*background_color_array_length = 8;
		}
		else
		{
			background_color_array[ 7 ] = "white";
		}


		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color9" ) ) )
		{
			background_color_array[ 8 ] =
				color_from_application_constants;
			*background_color_array_length = 9;
		}
		else
		{
			background_color_array[ 8 ] = "white";
		}


		if ( ( color_from_application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color10" ) ) )
		{
			background_color_array[ 9 ] =
				color_from_application_constants;
			*background_color_array_length = 10;
		}
		else
		{
			background_color_array[ 9 ] = "white";
		}
	}
	else
	{
		background_color_array[ 0 ] = FORM_COLOR1;
		background_color_array[ 1 ] = FORM_COLOR2;
		background_color_array[ 2 ] = FORM_COLOR3;
		background_color_array[ 3 ] = FORM_COLOR4;
		background_color_array[ 4 ] = FORM_COLOR5;
		*background_color_array_length = 5;
	}
	return background_color_array;

} /* form_get_background_color_array() */

void form_output_reset_button(	char *post_change_javascript,
				int form_number )
{
/*
	printf(
"<td valign=bottom><input type=\"button\" value=\"Reset\" onClick=\"form_reset(document.forms[%d], '%c')",
		form_number,
		ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER );
*/

	printf(
"<input type=\"button\" value=\"Reset\" onClick=\"form_reset(document.forms[%d], '%c')",
		form_number,
		ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER );

	if ( post_change_javascript )
	{
		printf( ";%s;", post_change_javascript );
	}

	printf( "\">\n" );

} /* form_output_reset_button() */

void form_output_prelookup_skip_button(	int form_number )
{
	printf(
"<input type=\"button\" value=\"Skip\" onClick=\"form_reset(document.forms[%d], '%c'); document.forms[%d].submit()",
		form_number,
		ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER,
		form_number );

	printf( "\">\n" );

} /* form_output_prelookup_skip_button() */

void form_output_html_help_file_anchor(
			char *application_name,
			char *html_help_file_anchor )
{
	if ( html_help_file_anchor
	&&   *html_help_file_anchor )
	{
		char full_pathname[ 512 ];

		if ( *html_help_file_anchor == '/' )
		{
			strcpy( full_pathname, html_help_file_anchor );
		}
		else
		{
			sprintf(	full_pathname,
					"/appaserver/src_%s/%s",
					application_name,
					html_help_file_anchor );
		}

		printf(
"<input type=button value=Help onclick='window.open(\"%s\",\"help_window\",\"width=600,height=350,resizable=yes,scrollbars=yes\")'>\n",
			full_pathname );
	}

} /* form_output_html_help_file_anchor() */

void form_output_submit_reset_buttons(
			char *submit_control_string,
			char *button_label,
			char *html_help_file_anchor,
			boolean with_table_tags,
			char *remember_keystrokes_onload_control_string,
			char *application_name,
			boolean with_back_to_top_button,
			boolean with_prelookup_skip_button,
			int form_number,
			char *post_change_javascript,
			LIST *pair_one2m_related_folder_name_list )
{
	if ( with_table_tags ) printf( "<tr><td>\n" );

	if ( with_prelookup_skip_button )
		form_output_prelookup_skip_button( form_number );

	form_output_submit_button(
			submit_control_string,
			button_label,
			form_number,
			pair_one2m_related_folder_name_list );

	form_output_reset_button(	post_change_javascript,
					form_number );

	if ( html_help_file_anchor && *html_help_file_anchor )
	{
		form_output_html_help_file_anchor(
			application_name,
			html_help_file_anchor );
	}

	if ( remember_keystrokes_onload_control_string )
	{
		form_output_remember_keystrokes_button(
			remember_keystrokes_onload_control_string );
	}

	if ( with_back_to_top_button )
	{
		form_output_back_to_top_button();
	}

	if ( with_table_tags ) printf( "</td>\n" );

} /* form_output_submit_reset_buttons() */

void form_output_submit_button(	char *submit_control_string,
				char *button_label,
				int form_number,
				LIST *pair_one2m_related_folder_name_list )
{
	if ( list_length( pair_one2m_related_folder_name_list ) )
	{
		form_output_insert_pair_one2m_submit_buttons(
			submit_control_string,
			pair_one2m_related_folder_name_list );
		return;
	}

	if ( !button_label || !*button_label )
		button_label = SUBMIT_BUTTON_LABEL;

	if ( submit_control_string && *submit_control_string )
	{
		/* -------------------------------------------- */
		/* The submit_control_string is assumed to	*/
		/* have "&&" appended to it.			*/
		/* -------------------------------------------- */
		printf(
"	<input type=\"button\" value=\"%s\" 			\n"
"	onClick=\"%s document.forms[%d].submit();\">		\n",
			button_label,
			submit_control_string,
			form_number );
	}
	else
	{
		printf(
"	<input type=\"button\" value=\"%s\" 			\n"
"	onClick=\"document.forms[%d].submit();\">		\n",
			button_label,
		     	form_number );
	}

} /* form_output_submit_button() */

void form_output_insert_pair_one2m_submit_buttons(
				char *submit_control_string,
				LIST *pair_one2m_related_folder_name_list )
{
	char *related_folder_name;
	char buffer[ 128 ];
	char pair_one2m_submit_folder_javascript[ 128 ];

	if ( !submit_control_string )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: empty submit_control_string.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );	
	}

	if ( !list_rewind( pair_one2m_related_folder_name_list ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: empty pair_one2m_related_folder_name_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );	
	}

	sprintf( pair_one2m_submit_folder_javascript,
		 "timlib_element_value_set( '%s%s_0', '%s' ); ",
		 PAIR_ONE2M_PREFIX,
		 PAIR_ONE2M_SUBMIT_FOLDER,
		 PAIR_ONE2M_OMIT );

	/* Output the generic <Submit> button. */
	/* ----------------------------------- */
	printf(
"<td valign=bottom><input type=\"button\" value=\"%s\"	  "
"onClick=\"%s %s document.forms[0].submit();\">		\n",
		SUBMIT_BUTTON_LABEL,
		submit_control_string,
		pair_one2m_submit_folder_javascript );

	do {
		related_folder_name =
			list_get( 
				pair_one2m_related_folder_name_list );

		sprintf( pair_one2m_submit_folder_javascript,
			 "timlib_element_value_set( '%s%s_0', '%s' ); ",
			 PAIR_ONE2M_PREFIX,
			 PAIR_ONE2M_SUBMIT_FOLDER,
			 related_folder_name );

		/* -------------------------------------------- */
		/* The submit_control_string is assumed to	*/
		/* have "&&" appended to it.			*/
		/* -------------------------------------------- */
/*
		printf(
"	<input type=\"button\" value=\"%s\" 			\n"
"	onClick=\"%s %s document.forms[0].submit();\">		\n",
			format_initial_capital_cr(
				buffer,
				related_folder_name ) ,
			submit_control_string,
			pair_one2m_submit_folder_javascript );
*/
		printf(
"<td valign=bottom><button onClick=\"%s %s document.forms[0].submit();\"> "
"%s</button>							        \n",
			submit_control_string,
			pair_one2m_submit_folder_javascript,
			format_initial_capital_cr(
				buffer,
				related_folder_name ) );

	} while( list_next( pair_one2m_related_folder_name_list ) );

} /* form_output_insert_pair_one2m_submit_buttons() */

