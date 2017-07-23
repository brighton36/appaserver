/* library/appaserver.c 						*/
/* -------------------------------------------------------------------- */
/* This maintains a set of APPASERVER ADTs.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver.h"
#include "application.h"
#include "appaserver_user.h"
#include "folder.h"
#include "list.h"
#include "related_folder.h"
#include "piece.h"
#include "timlib.h"
#include "process_parameter_list.h"
#include "environ.h"
#include "appaserver_parameter_file.h"
#include "hashtbl.h"

APPASERVER *appaserver_new_appaserver(
		char *application_name,
		char *session,
		char *folder_name )
{
	APPASERVER *appaserver;

	appaserver = (APPASERVER *)
				calloc( 1, sizeof( APPASERVER ) ) ;
	appaserver->application_name = application_name;
	appaserver->session = session;

	appaserver->folder =
		folder_new_folder(
			application_name,
			session,
			folder_name );

	return appaserver;

} /* appaserver_new_appaserver() */

char *appaserver_get_delete_display_string(
					char *folder_name,
					LIST *attribute_list,
					LIST *related_folder_list,
					char *primary_data_list_string )
{
	char buffer[ 65536 ];
	char *buffer_pointer = buffer;
	RELATED_FOLDER *related_folder;

	buffer_pointer += sprintf(
				buffer_pointer,
				"folder: %s (%s=%s)",
				folder_name,
				list_display(
				folder_get_primary_attribute_name_list(
					attribute_list ) ),
				primary_data_list_string );

	if ( !list_rewind( related_folder_list ) ) return strdup( buffer );
	do {
		related_folder = (RELATED_FOLDER *)
					list_get( related_folder_list );
		buffer_pointer += sprintf(
			buffer_pointer,
			"\n\t%s",
			related_folder->folder->folder_name );

		buffer_pointer += sprintf(
			buffer_pointer,
			" (%s=%s)",
			list_display( 
			    related_folder_get_foreign_attribute_name_list(
				folder_get_primary_attribute_name_list(
					related_folder->
						one2m_related_folder->
						attribute_list ),
				related_folder->related_attribute_name ) ),
			list_display(
				related_folder->
					primary_data_list ) );

	} while( list_next( related_folder_list ) );
	return strdup( buffer );
} /* appaserver_get_delete_display_string() */

LIST *appaserver_remove_attribute_name_list_from_related_folder_list(
					LIST *related_folder_list,
					LIST *exclude_attribute_name_list )
{
	LIST *foreign_attribute_name_list;
	LIST *primary_attribute_name_list;
	RELATED_FOLDER *related_folder;
	LIST *return_list;

	return_list = list_new();

	if ( list_reset( related_folder_list ) )
	{
		do {
			related_folder = (RELATED_FOLDER *)
					 list_get( related_folder_list );

			primary_attribute_name_list =
				folder_get_primary_attribute_name_list(
					related_folder->
						folder->
						attribute_list );

			foreign_attribute_name_list =
			    related_folder_get_foreign_attribute_name_list(
					primary_attribute_name_list,
				     	related_folder->
					related_attribute_name );

			if ( list_length( 
				list_subtract( 
				 foreign_attribute_name_list,
				 exclude_attribute_name_list ) ) )
			{
				list_append_pointer( 	return_list, 
							related_folder );
			}

		} while( list_next( related_folder_list ) );
	}
	return return_list;
} /* appaserver_remove_attribute_name_list_from_related_folder_list() */

LIST *appaserver_include_attribute_name_list_in_related_folder_list(
						LIST *related_folder_list,
						LIST *attribute_name_list )
{
	LIST *foreign_attribute_name_list;
	LIST *primary_attribute_name_list;
	RELATED_FOLDER *related_folder;
	LIST *return_list;

	return_list = list_new();

	if ( list_reset( related_folder_list ) )
	{
		do {
			related_folder = (RELATED_FOLDER *)
					 list_get( related_folder_list );

			primary_attribute_name_list =
				folder_get_primary_attribute_name_list(
					related_folder->
						folder->
						attribute_list );

			foreign_attribute_name_list =
			    related_folder_get_foreign_attribute_name_list(
					primary_attribute_name_list,
				     	related_folder->
					related_attribute_name );

			if ( list_length( 
				list_subtract( 
					foreign_attribute_name_list,
					attribute_name_list ) ) ==
			     list_length( attribute_name_list ) )
			{
				list_append_pointer( 	return_list, 
							related_folder );
			}

		} while( list_next( related_folder_list ) );
	}
	return return_list;
} /* appaserver_include_attribute_name_list_in_related_folder_list() */

LIST *appaserver_get_exclude_permission_record_list(
					char *application_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=attribute,role,permission"
		 "			folder=attribute_exclude	",
		 application_name );

	return pipe2list( sys_string );
} /* appaserver_get_exclude_permission_record_list() */

static LIST *global_exclude_permission_record_list = {0};

LIST *appaserver_get_exclude_permission_list(
					char *application_name,
					char *attribute_name,
					char *role_name )
{
	char fetched_attribute_name[ 128 ];
	char fetched_role_name[ 128 ];
	char permission[ 128 ];
	LIST *exclude_permission_list;
	char *exclude_permission_record;

	if ( !attribute_name || !*attribute_name ) return (LIST *)0;
	if ( !role_name || !*role_name ) return (LIST *)0;
 
	if ( !global_exclude_permission_record_list )
	{
		global_exclude_permission_record_list =
			appaserver_get_exclude_permission_record_list(
					application_name );
	}

	exclude_permission_list = list_new_list();

	if ( !list_rewind( global_exclude_permission_record_list ) )
		return exclude_permission_list;

	do {
		exclude_permission_record =
			list_get_pointer(
				global_exclude_permission_record_list );

		piece(	fetched_attribute_name,
			FOLDER_DATA_DELIMITER,
			exclude_permission_record,
			0 );

		if ( strcmp( attribute_name, fetched_attribute_name ) != 0 )
			continue;

		piece(	fetched_role_name,
			FOLDER_DATA_DELIMITER,
			exclude_permission_record,
			1 );

		if ( strcmp( role_name, fetched_role_name ) != 0 )
			continue;

		piece(	permission,
			FOLDER_DATA_DELIMITER,
			exclude_permission_record,
			2 );

		list_append_pointer(	exclude_permission_list,
					strdup( permission ) );

	} while( list_next( global_exclude_permission_record_list ) );

	return exclude_permission_list;

} /* appaserver_get_exclude_permission_list() */

boolean appaserver_exclude_permission(	LIST *exclude_permission_list,
					char *permission )
{
	char *compare_permission;

	if ( !exclude_permission_list
	||   !list_rewind( exclude_permission_list ) )
		return 0;
	do {
		compare_permission =
			list_get_pointer( exclude_permission_list );
		if ( strcmp( compare_permission, permission ) == 0 )
			return 1;
	} while( list_next( exclude_permission_list ) );
	return 0;
} /* appaserver_exclude_permission() */

LIST *appaserver_get_update_lookup_attribute_element_list(
				char update_yn,
				LIST *primary_attribute_name_list,
				LIST *exclude_permission_list,
				char *attribute_name,
				char *datatype,
				int width,
				char *post_change_javascript,
				char *on_focus_javascript_function,
				boolean is_primary_attribute )
{
	LIST *return_list;
	ELEMENT *element;

	return_list = list_new();

	if ( strcmp(	datatype,
			element_get_type_string( http_filename ) ) == 0 )
	{
		element = element_new(
				http_filename, 
				attribute_name );

		if ( update_yn == 'y' )
		{
			ELEMENT *temp_element;

			temp_element =
				element_get_text_item_variant_element(
					attribute_name,
					element_get_type_string( text_item ),
					width,
					post_change_javascript,
					on_focus_javascript_function );

			element_text_item_set_onchange_null2slash(
					temp_element->text_item );

			element->http_filename->update_text_item =
				temp_element->text_item;
		}

		list_append_pointer( return_list, element );
		return return_list;
	}

	if ( update_yn != 'y' 
	||   appaserver_exclude_permission(
			exclude_permission_list,
			"update" ) )
	{
		if ( appaserver_exclude_permission(
			exclude_permission_list,
			"lookup" ) )
		{
			return (LIST *)0;
		}

		if ( strcmp( 	datatype, 
				"password" ) == 0 )
		{
			return (LIST *)0;
		}

		element = element_new( 
				prompt_data_plus_hidden,
				attribute_name );

		element_prompt_data_set_heading(
				element->prompt_data,
				element->name );

		list_append_pointer(
				return_list, 
				element );

		if ( list_exists_string(
			primary_attribute_name_list,
			attribute_name ) )
		{
			element = element_new(
				hidden, 
				attribute_name );

			list_append_pointer(
				return_list, 
				element );
		}
		return return_list;
	} /* if view only */

	if ( strcmp( datatype, "notepad" ) == 0 )
	{
		element = element_new( 	notepad,
					attribute_name);

		element_notepad_set_attribute_width(
				element->notepad,
				width );

		element_notepad_set_heading(
				element->notepad, 
				element->name );

		element_notepad_set_onchange_null2slash(
				element->notepad );

		element->notepad->state = "update";
	}
	else
	if ( strcmp( datatype, "password" ) == 0 )
	{
		element = element_new( 	password,
					attribute_name);

		element_password_set_attribute_width(
				element->password,
				width );

		element_password_set_heading(
				element->password, 
				element->name );

		element->password->state = "update";
	}
	else
	if ( strcmp( datatype, "hidden_text" ) == 0 )
	{
		element = element_new(
				hidden, 
				attribute_name );
	}
	else
	if ( strcmp( datatype, "timestamp" ) == 0 )
	{
		element = element_new( 
				prompt_data,
				attribute_name );

		element_prompt_data_set_heading(
				element->prompt_data,
				element->name );
	}
	else
	if ( process_parameter_list_element_name_boolean( attribute_name ) )
	{
		element =
			element_get_yes_no_drop_down(
				attribute_name,
				(char *)0 /* prepend_folder_name */,
				post_change_javascript,
				1 /* with_is_null */,
				0 /* not with_not_null */ );

		element->drop_down->state = "update";
	}
	else
	if ( strcmp( datatype, "reference_number" ) == 0 )
	{
		element = element_new(
				hidden, 
				attribute_name );

		list_append_pointer( return_list, element );

		element = element_new( 
				prompt_data,
				attribute_name );

		if ( is_primary_attribute )
		{
			char heading[ 128 ];
			sprintf( heading, "*%s", element->name );
			element_prompt_data_set_heading(
					element->prompt_data,
					strdup( heading ) );
		}
		else
		{
			element_prompt_data_set_heading(
					element->prompt_data,
					element->name );
		}
	}
	else
	if ( strcmp( datatype, "date" ) == 0
	||   strcmp( datatype, "current_date" ) == 0
	||   strcmp( datatype, "date_time" ) == 0 )
	{
		element = element_new( 	element_date,
					attribute_name);

		element_text_item_set_attribute_width(
				element->text_item, 
				width );

		if ( is_primary_attribute )
		{
			char heading[ 128 ];
			sprintf( heading, "*%s", element->name );
			element_text_item_set_heading(
					element->text_item,
					strdup( heading ) );
		}
		else
		{
			element_text_item_set_heading(
					element->text_item,
					element->name );
		}

		element_text_item_set_onchange_null2slash(
				element->text_item );

		element->text_item->post_change_javascript =
			post_change_javascript;

		element->text_item->on_focus_javascript_function =
			on_focus_javascript_function;

		element->text_item->state = "update";
	}
	else
	{
		element = element_new( 	text_item,
					attribute_name);

		element_text_item_set_attribute_width(
				element->text_item, 
				width );

		if ( is_primary_attribute )
		{
			char heading[ 128 ];
			sprintf( heading, "*%s", element->name );
			element_text_item_set_heading(
					element->text_item,
					strdup( heading ) );
		}
		else
		{
			element_text_item_set_heading(
					element->text_item,
					element->name );
		}

		element_text_item_set_onchange_null2slash(
				element->text_item );

		element->text_item->post_change_javascript =
			post_change_javascript;

		element->text_item->on_focus_javascript_function =
			on_focus_javascript_function;

		element->text_item->state = "update";
	}

	list_append_pointer( return_list, element );

	return return_list;

} /* appaserver_get_update_lookup_attribute_element_list() */

void appaserver_append_isa_related_attribute_list(
					LIST *attribute_list,
					LIST *mto1_isa_related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( mto1_isa_related_folder_list
	&&   list_rewind( mto1_isa_related_folder_list ) )
	{
		do {
			related_folder =
				(RELATED_FOLDER *)
					list_get_pointer(
						mto1_isa_related_folder_list );
			list_append_list(
				attribute_list,
				related_folder->folder->attribute_list );
		} while( list_next( mto1_isa_related_folder_list ) );
	}

} /* appaserver_append_isa_related_attribute_list() */

LIST *appaserver_get_exclude_attribute_name_list(
			LIST *attribute_list,
			char *permission )
{
	char *compare_permission;
	ATTRIBUTE *attribute;
	LIST *exclude_attribute_name_list = list_new();

	if ( !list_rewind( attribute_list ) )
		return exclude_attribute_name_list;

	do {
		attribute = list_get_pointer( attribute_list );

		if ( !attribute->exclude_permission_list
		||   !list_rewind( attribute->exclude_permission_list ) )
		{
			continue;
		}

		do {
			compare_permission =
				list_get_pointer(
					attribute->
					exclude_permission_list );

			if ( strcmp( permission, compare_permission ) == 0 )
			{
				list_append_pointer(
						exclude_attribute_name_list,
						attribute->attribute_name );
			}
		} while( list_next( attribute->exclude_permission_list ) );

	} while( list_next( attribute_list ) );

	return exclude_attribute_name_list;

} /* appaserver_get_exclude_attribute_name_list() */

int appaserver_get_frameset_menu_horizontal(	char *application_name,
						char *login_name )
{
	char application_frameset_menu_horizontal_yn;
	char appaserver_user_frameset_menu_horizontal_yn;
	int frameset_menu_horizontal = 1;

	application_frameset_menu_horizontal_yn =
		application_get_frameset_menu_horizontal_yn(
					application_name );

	appaserver_user_frameset_menu_horizontal_yn =
		appaserver_user_get_frameset_menu_horizontal_yn(
					application_name,
					login_name );

	if ( application_frameset_menu_horizontal_yn != 'y'
	||   appaserver_user_frameset_menu_horizontal_yn != 'y' )
	{
		frameset_menu_horizontal = 0;
	}

	return frameset_menu_horizontal;

} /* appaserver_get_frameset_menu_horizontal() */

LIST *appaserver_get_mto1_isa_foreign_attribute_name_list(
					LIST *mto1_isa_related_folder_list )
{
	RELATED_FOLDER *first_related_folder;

	if ( !list_length( mto1_isa_related_folder_list ) )
		return (LIST *)0;

	first_related_folder =
		(RELATED_FOLDER *)
			list_get_first_pointer(
				mto1_isa_related_folder_list );

	return related_folder_get_foreign_attribute_name_list(
		folder_get_primary_attribute_name_list(
			first_related_folder->
				folder->attribute_list ),
			first_related_folder->related_attribute_name );
	
} /* appaserver_get_mto1_isa_foreign_attribute_name_list() */

LIST *appaserver_get_isa_folder_list( char *application_name )
{
	static LIST *isa_folder_list = {0};
	ISA_FOLDER *isa_folder;
	char sys_string[ 1024 ];
	char *where = "relation_type_isa_yn = 'y'";
	char input_buffer[ 1024 ];
	char folder_name[ 128 ];
	char related_folder_name[ 128 ];
	char related_attribute_name[ 128 ];
	FILE *pipe;
	char *select = "folder,related_folder,related_attribute";

	if ( isa_folder_list ) return isa_folder_list;

	isa_folder_list = list_new_list();

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			folder=relation			"
		"			select=%s			"
		"			where=\"%s\"			",
		application_name,
		select,
		where );

	pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, pipe ) )
	{
		piece( folder_name, '^', input_buffer, 0 );
		piece( related_folder_name, '^', input_buffer, 1 );
		piece( related_attribute_name, '^', input_buffer, 2 );

		isa_folder = appaserver_new_isa_folder(
					strdup( folder_name ),
					strdup( related_folder_name ),
					strdup( related_attribute_name ) );

		list_append_pointer(	isa_folder_list,
					isa_folder );
	}
	pclose( pipe );
	return isa_folder_list;
} /* appaserver_get_isa_folder_list() */

char *appaserver_isa_folder_list_display(
				LIST *isa_folder_list )
{
	ISA_FOLDER *isa_folder;
	char buffer[ 4096 ];
	char *buffer_ptr = buffer;
	boolean first_time = 1;

	*buffer_ptr = '\0';

	if ( list_rewind( isa_folder_list ) )
	{
		do {
			isa_folder =
				list_get_pointer(
					isa_folder_list );
			if ( first_time )
			{
				first_time = 0;
			}
			else
			{
				buffer_ptr += sprintf( buffer_ptr, "," );
			}

			buffer_ptr +=
			sprintf(
				buffer_ptr,
			"folder = %s, related_folder = %s, accounted_for = %d",
				isa_folder->folder_name,
				isa_folder->related_folder_name,
				isa_folder->accounted_for );

		} while( list_next( isa_folder_list ) );
	}
	return strdup( buffer );
} /* appaserver_isa_folder_list_display() */

ISA_FOLDER *appaserver_new_isa_folder(	char *folder_name,
					char *related_folder_name,
					char *related_attribute_name )
{
	ISA_FOLDER *a = (ISA_FOLDER *)calloc( 1, sizeof( ISA_FOLDER ) );

	if ( !related_attribute_name
	||   !*related_attribute_name )
	{
		related_attribute_name = "null";
	}

	a->folder_name = folder_name;
	a->related_folder_name = related_folder_name;
	a->related_attribute_name = related_attribute_name;
	return a;
} /* appaserver_new_isa_folder() */

int appaserver_isa_folder_accounted_for(
				LIST *isa_folder_list,
				char *related_folder_name,
				char *related_attribute_name )
{
	ISA_FOLDER *isa_folder;

	if ( !list_rewind( isa_folder_list ) ) return 0;

	if ( !related_attribute_name ) related_attribute_name = "null";

	do {
		isa_folder = list_get_pointer( isa_folder_list );

		if (
			( strcmp(	isa_folder->folder_name,
					related_folder_name ) == 0
		&&        strcmp(	isa_folder->related_attribute_name,
					related_attribute_name ) == 0 )
/*
		||
		   	( strcmp(	isa_folder->related_folder_name,
					related_folder_name ) == 0
		&&	  strcmp(	isa_folder->related_attribute_name,
					related_attribute_name ) == 0 )
*/
				)
		{
			if ( isa_folder->accounted_for ) return 1;
			isa_folder->accounted_for = 1;
			return 0;
		}
	} while( list_next( isa_folder_list ) );

	return 0;

} /* appaserver_isa_folder_accounted_for() */

enum aggregate_statistic appaserver_based_on_datatype_get_aggregate_statistic(
				char *application_name,
				char *appaserver_mount_point,
				char *datatype )
{
	enum aggregate_statistic aggregate_statistic;

	if ( appaserver_get_aggregation_sum_yn(
				application_name,
				appaserver_mount_point,
				datatype ) == 'y' )
	{
		aggregate_statistic =
			aggregate_statistic_get_aggregate_statistic(
				"sum",
				daily );
	}
	else
	{
		aggregate_statistic =
			aggregate_statistic_get_aggregate_statistic(
				"average",
				daily );
	}

	return aggregate_statistic;

} /* appaserver_based_on_datatype_get_aggregate_statistic() */

char appaserver_get_aggregation_sum_yn(
				char *application,
				char *appaserver_mount_point,
				char *datatype )
{
	char sys_string[ 1024 ];
	char *results;

	sprintf( sys_string,
		 "%s/%s/get_aggregation_sum_yn '%s' '%s'",
		 appaserver_mount_point,
		 application_get_relative_source_directory( application ),
		 application,
		 datatype );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		return 'n';
	}
	else
	{
		return *results;
	}
} /* appaserver_get_aggregation_sum_yn() */

