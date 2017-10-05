/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/appaserver_library.c			*/
/* --------------------------------------------------------------------	*/
/*									*/
/* These are the generic Appaserver functions.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "related_folder.h"
#include "folder.h"
#include "application.h"
#include "attribute.h"
#include "query.h"
#include "appaserver_library.h"
#include "element.h"
#include "appaserver.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "operation.h"
#include "date.h"
#include "environ.h"
#include "date_convert.h"
#include "appaserver_error.h"
#include "basename.h"
#include "process_parameter_list.h"

static char *system_folder_list[] = {
					"additional_user_drop_down_attribute",
					"additional_drop_down_attribute",
					"appaserver_sessions",
					"appaserver_user",
					"application",
					"application_constants",
					"attribute",
					"attribute_datatype",
					"attribute_exclude",
					"date_formats",
					"drop_down_prompt",
					"drop_down_prompt_data",
					"folder",
					"folder_attribute",
					"folder_row_level_restrictions",
					"foreign_attribute",
					"form",
					"grace_output",
					"javascript_files",
					"javascript_folders",
					"javascript_processes",
					"javascript_process_sets",
					"login_default_role",
					"operation",
					"permissions",
					"process",
					"process_generic_datatype_folder",
					"process_generic_value_folder",
					"process_generic_output",
					"process_generic_units",
					"process_groups",
					"process_set",
					"process_set_parameter",
					"process_parameter",
					"prompt",
					"relation",
					"role",
					"role_folder",
					"role_operation",
					"role_appaserver_user",
					"role_process",
					"role_process_set_member",
					"row_level_restrictions",
					"row_security_role_update",
					"select_statement",
					"subschemas",
					"upgrade_scripts",
					(char *)0 };

void output_update_pipe_string( 	FILE *destination,
					char *attribute_name,
					char *data,
					LIST *primary_key_data_list )
{
	fprintf( destination,
		 "%s|",
		 list_display( primary_key_data_list ) );
	fprintf( destination,
		 "%s=%s",
		 attribute_name,
		 data );
	fprintf( destination, "\n" );
} /* output_update_pipe_string() */

char *get_full_attribute_name( 	char *application_name,
				char *folder_name,
				char *attribute_name )
{
	char full_attribute_name[ 512 ];

	if ( folder_name && *folder_name )
	{
		char tmp[ 512 ];

		/* A join might be in place. */
		/* ------------------------- */
		piece( tmp, ',', folder_name, 0 );

		sprintf( full_attribute_name,
			 "%s.%s",
			 get_table_name(	application_name,
						tmp ),
			 attribute_name );
	}
	else
	{
		strcpy( full_attribute_name, attribute_name );
	}

	return strdup( full_attribute_name );
} /* get_full_attribute_name() */

char *get_multiple_table_names(
			char *application_name,
			char *folder_name_list_comma_string )
{
	return get_multi_table_name(	application_name,
				folder_name_list_comma_string );
}

char *get_multi_table_name(	char *application_name,
				char *multi_folder_name_list_string )
{
	char multi_table_name[ 1024 ];
	char *ptr = multi_table_name;
	char folder_name[ 128 ];
	char *table_name;
	int i;

	for(	i = 0;
		piece(	folder_name,
			',',
			multi_folder_name_list_string,
			i );
		i++ )
	{
		if ( i ) ptr += sprintf( ptr, "," );
		table_name = get_table_name( application_name, folder_name );
		ptr += sprintf( ptr, "%s", table_name );
		free( table_name );
	}

	return strdup( multi_table_name );

} /* get_multi_table_name() */

char *get_folder_name(		char *application_name,
				char *table_name )
{
	char folder_name[ 512 ];
	char application_table_name[ 512 ];

	if ( !application_name || !table_name ) return "";

	sprintf( application_table_name, "%s_application", application_name );

	if ( strcmp( table_name, application_table_name ) == 0 )
	{
		strcpy( folder_name, table_name );
	}
	else
	if ( application_is_primary_application( application_name ) )
	{
		strcpy( folder_name, table_name );
	}
	else
	{
		char search_string[ 512 ];
		int str_len;

		sprintf( search_string, "%s_", application_name );
		str_len = strlen( search_string );

		if ( strncasecmp( table_name, search_string, str_len ) != 0 )
		{
			strcpy( folder_name, table_name );
		}
		else
		{
			strcpy(	folder_name,
				table_name + strlen( application_name ) + 1 );
		}
	}
	return strdup( folder_name );
} /* get_folder_name() */

char *get_table_name( 	char *application_name,
			char *folder_name )
{
	char table_name[ 512 ];

	if ( !application_name ) return folder_name;

	if ( strcmp( folder_name, "application" ) == 0 )
	{
		sprintf( table_name, "%s_%s", application_name, folder_name );
	}
	else
	if ( application_is_primary_application( application_name ) )
	{
		sprintf( table_name, "%s", folder_name );
	}
	else
	{
		sprintf( table_name, "%s_%s", application_name, folder_name );
	}
	return strdup( table_name );
} /* get_table_name() */

/* Sample: dictionary_string = "first_name=tim&last_name=riley" */
/* ------------------------------------------------------------ */
void output_dictionary_string_as_hidden( char *dictionary_string )
{
	char pair_string[ 4096 ];
	char attribute[ 2048 ];
	char data[ 2048 ];
	int i;

	for( i = 0; piece( pair_string, '&', dictionary_string, i ); i++ )
	{
		piece( attribute, '=', pair_string, 0 );
		if ( piece( data, '=', pair_string, 1 ) )
		{
			printf( 
			"<input type=\"hidden\"	name=\"%s\" value=\"%s\">\n",
			attribute, data );
		}
	}
} /* output_dictionary_string_as_hidden() */

void appaserver_library_output_dictionary_as_hidden( DICTIONARY *dictionary )
{
	output_dictionary_as_hidden( dictionary );
}

void output_dictionary_as_hidden( DICTIONARY *dictionary )
{
	LIST *key_list = dictionary_get_key_list( dictionary );
	char *key;
	char *data;

	if ( list_reset( key_list ) )
	{
		do {
			key = list_get_string( key_list );

			if ( ( data =
				(char *)dictionary_get( dictionary, key ) ) )
			{
				printf( 
			"<input name=\"%s\" type=\"hidden\" value=\"%s\">\n",
					key,
					data );
			}
		} while( list_next( key_list ) );
	}
	list_free_container( key_list );
} /* output_dictionary_as_hidden() */

LIST *get_relation_operator_list( char *datatype )
{
	LIST *list;

	list = list_new();

	if ( strcmp( datatype, "date" ) == 0
	||   strcmp( datatype, "time" ) == 0 )
	{
		list_append_string( list, BEGINS_OPERATOR );
		list_append_string( list, EQUAL_OPERATOR );
		list_append_string( list, BETWEEN_OPERATOR );
		list_append_string( list, OR_OPERATOR );
		list_append_string( list, GREATER_THAN_OPERATOR );
		list_append_string( list, GREATER_THAN_EQUAL_TO_OPERATOR );
		list_append_string( list, LESS_THAN_OPERATOR );
		list_append_string( list, LESS_THAN_EQUAL_TO_OPERATOR );
		list_append_string( list, NOT_EQUAL_OPERATOR );
		list_append_string( list, NOT_EQUAL_OR_NULL_OPERATOR );
		list_append_string( list, NULL_OPERATOR );
		list_append_string( list, NOT_NULL_OPERATOR );
	}
	else
	if ( strcmp( datatype, "notepad" ) == 0 )
	{
		list_append_string( list, CONTAINS_OPERATOR );
		list_append_string( list, NOT_CONTAINS_OPERATOR );
		list_append_string( list, NULL_OPERATOR );
		list_append_string( list, NOT_NULL_OPERATOR );
	}
	else
	if ( strcmp( datatype, "text" ) == 0
	||   strcmp( datatype, "http_filename" ) == 0 )
	{
		list_append_string( list, BEGINS_OPERATOR );
		list_append_string( list, EQUAL_OPERATOR );
		list_append_string( list, CONTAINS_OPERATOR );
		list_append_string( list, OR_OPERATOR );
		list_append_string( list, NOT_CONTAINS_OPERATOR );
		list_append_string( list, NOT_EQUAL_OPERATOR );
		list_append_string( list, NOT_EQUAL_OR_NULL_OPERATOR );
		list_append_string( list, NULL_OPERATOR );
		list_append_string( list, NOT_NULL_OPERATOR );
	}
	else
	{
		list_append_string( list, EQUAL_OPERATOR );
		list_append_string( list, BETWEEN_OPERATOR );
		list_append_string( list, BEGINS_OPERATOR );
		list_append_string( list, CONTAINS_OPERATOR );
		list_append_string( list, NOT_CONTAINS_OPERATOR );
		list_append_string( list, OR_OPERATOR );
		list_append_string( list, NOT_EQUAL_OPERATOR );
		list_append_string( list, NOT_EQUAL_OR_NULL_OPERATOR );
		list_append_string( list, GREATER_THAN_OPERATOR );
		list_append_string( list, GREATER_THAN_EQUAL_TO_OPERATOR );
		list_append_string( list, LESS_THAN_OPERATOR );
		list_append_string( list, LESS_THAN_EQUAL_TO_OPERATOR );
		list_append_string( list, NULL_OPERATOR );
		list_append_string( list, NOT_NULL_OPERATOR );
	}

	return list;
} /* get_relation_operator_list() */

LIST *get_relation_operator_equal_list()
{
	static LIST *list = {0};

	if ( list ) return list;

	list = list_new();
	list_append_string( list, EQUAL_OPERATOR );
	return list;
} /* get_relation_operator_equal_list() */

char *get_operator_character( char *operator_string )
{
	if ( strcmp( operator_string, EQUAL_OPERATOR ) == 0 )
		return "=";
	else
	if ( strcmp( operator_string, NOT_EQUAL_OPERATOR ) == 0 )
		return "<>";
	else
	if ( strcmp( operator_string, GREATER_THAN_OPERATOR ) == 0 )
		return ">";
	else
	if ( strcmp( operator_string, GREATER_THAN_EQUAL_TO_OPERATOR ) == 0 )
		return ">=";
	else
	if ( strcmp( operator_string, LESS_THAN_OPERATOR ) == 0 )
		return "<";
	else
	if ( strcmp( operator_string, LESS_THAN_EQUAL_TO_OPERATOR ) == 0 )
		return "<=";
	else
	if ( strcmp( operator_string, BEGINS_OPERATOR ) == 0 )
		return BEGINS_OPERATOR;
	else
	if ( strcmp( operator_string, CONTAINS_OPERATOR ) == 0 )
		return CONTAINS_OPERATOR;
	else
	if ( strcmp( operator_string, OR_OPERATOR ) == 0 )
		return OR_OPERATOR;
	else
	if ( strcmp( operator_string, NOT_CONTAINS_OPERATOR ) == 0 )
		return NOT_CONTAINS_OPERATOR;
	else
	if ( strcmp( operator_string, NOT_EQUAL_OR_NULL_OPERATOR ) == 0 )
		return NOT_EQUAL_OR_NULL_OPERATOR;
	else
	if ( strcmp( operator_string, BETWEEN_OPERATOR ) == 0 )
		return BETWEEN_OPERATOR;
	else
	if ( strcmp( operator_string, NULL_OPERATOR ) == 0 )
		return NULL_OPERATOR;
	else
	if ( strcmp( operator_string, NOT_NULL_OPERATOR ) == 0 )
		return NOT_NULL_OPERATOR;
	else
		return "unknown operator";
} /* get_operator_character() */

boolean appaserver_library_is_system_folder( char *folder_name )
{
	return is_system_folder( folder_name );
}

boolean is_system_folder( char *folder_name )
{
	char **f;

	for ( f = system_folder_list; *f; f++ )
		if ( strcmp( folder_name, *f ) == 0 )
			return 1;
	return 0;
} /* is_system_folder() */

char **get_system_folder_list( void )
{
	return system_folder_list;
}

char **appaserver_library_get_system_folder_list( void )
{
	return system_folder_list;
}

LIST *get_role_list(	char *application_name,
			char *login_name,
			char *error_file )
{
	char sys_string[ 256 ];

	sprintf( sys_string, 
		 "roles4appaserver_user.sh %s %s 2>>%s", 
		 application_name,
		 login_name, error_file );
	return pipe2list( sys_string );
} /* get_role_list() */

void appaserver_library_set_no_display_pressed(
					DICTIONARY *ignore_dictionary, 
					LIST *key_list )
{
	char *key;
	char no_display_key[ 1024 ];

	if ( list_reset( key_list ) )
	{
		do {
			key = list_get_pointer( key_list );

			sprintf( no_display_key,
				 "%s_0",
				 key );

			dictionary_set_pointer(
				ignore_dictionary,
				strdup( no_display_key ),
				"yes" );
		} while( list_next( key_list ) );
	}

} /* appaserver_library_set_no_display_pressed() */

LIST *appaserver_library_get_no_display_pressed_attribute_name_list( 	
					DICTIONARY *ignore_dictionary,
					LIST *attribute_name_list )
{
	LIST *return_list = list_new_list();
	char *attribute_name;
	char key_to_search[ 128 ];

	if ( !list_reset( attribute_name_list ) ) return (LIST *)0;

	if ( !ignore_dictionary ) return return_list;

	do {
		attribute_name =
			list_get_pointer( attribute_name_list );

		sprintf( key_to_search,
			 "%s_0",
			 attribute_name );

		if ( dictionary_key_exists(
				ignore_dictionary,
				key_to_search ) )
		{
			list_append_unique_string(
						return_list, 
						attribute_name );
		}

	} while( list_next( attribute_name_list ) );
	return return_list;
} /* appaserver_library_get_no_display_pressed_attribute_name_list() */

LIST *appaserver_library_get_ignore_pressed_attribute_name_list( 	
					DICTIONARY *ignore_dictionary,
					LIST *attribute_name_list,
					DICTIONARY *query_dictionary )
{
	LIST *return_list = create_list();
	char *attribute_name;

	if ( !dictionary_length( ignore_dictionary ) ) return return_list;
	if ( !list_reset( attribute_name_list ) ) return return_list;

	do {
		attribute_name = list_get( attribute_name_list );

		if ( dictionary_key_exists_index_zero(
			ignore_dictionary,
			attribute_name ) )
		{
			if ( !query_dictionary )
			{
				list_append_string( 	return_list, 
							attribute_name );
			}
			else
			if ( !dictionary_key_exists_index_zero(
				query_dictionary,
				attribute_name ) )
			{
				list_append_string( 	return_list, 
							attribute_name );
			}
			continue;
		}

	} while( list_next( attribute_name_list ) );

	return return_list;

} /* appaserver_library_get_ignore_pressed_attribute_name_list() */

int get_attribute_width(	char *application,
				char *attribute_name )
{
	char sys_string[ 1024 ];
	char *results;

	sprintf( sys_string,
		 "width4attribute.sh %s %s 2>>%s",
		 application,
		 attribute_name,
		 appaserver_get_error_filename( application ) );

	results = pipe2string( sys_string );
	if ( !results )
		return 0;
	else
		return atoi( results );
} /* get_attribute_width() */


LIST *get_date_slot_list( 	char *begin_date,
				char *end_date )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "get_date_slot_list %s %s",
		 begin_date,
		 end_date );
	return pipe2list( sys_string );
} /* get_date_slot_list() */

LIST *get_attribute4table_list( char *table_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string, "attributes4table.sh %s", table_name );
	return pipe2list( sys_string );
} /* get_attribute4table_list() */


void populate_no_display_button_for_ignore( DICTIONARY *dictionary )
{
	LIST *attribute_name_list;
	char *attribute_name;
	char *data;
	char trimmed_attribute_name[ 256 ];
	char ignored_attribute_name[ 256 ];
	int str_len = strlen( IGNORE_PUSH_BUTTON_PREFIX );

	attribute_name_list =
		dictionary_get_key_list( dictionary );

	if ( list_rewind( attribute_name_list ) )
	{
		do {
			attribute_name = 
				list_get_pointer( attribute_name_list );

			if ( strncmp(
				IGNORE_PUSH_BUTTON_PREFIX,
				attribute_name,
				str_len ) == 0 )
			{
				data = dictionary_get_data(
							dictionary,
							attribute_name );

				strcpy(	trimmed_attribute_name,
					attribute_name + str_len );

				sprintf(ignored_attribute_name,
		 			"%s%s", 
		 			NO_DISPLAY_PUSH_BUTTON_PREFIX, 
		 			trimmed_attribute_name );

				dictionary_set_pointer(
					dictionary,
					strdup( ignored_attribute_name ),
					data );
			}
		} while( list_next( attribute_name_list ) );
	}
} /* populate_no_display_button_for_ignore() */

void populate_ignore_button_for_no_display_pressed( DICTIONARY *dictionary )
{
	LIST *attribute_name_list;
	char *attribute_name;
	char *data;
	char trimmed_attribute_name[ 256 ];
	char ignored_attribute_name[ 256 ];
	int str_len = strlen( NO_DISPLAY_PUSH_BUTTON_PREFIX );

	attribute_name_list =
		dictionary_get_key_list( dictionary );

	if ( list_rewind( attribute_name_list ) )
	{
		do {
			attribute_name = 
				list_get_pointer( attribute_name_list );

			if ( strncmp(
				NO_DISPLAY_PUSH_BUTTON_PREFIX,
				attribute_name,
				str_len ) == 0 )
			{
				data = dictionary_get_data(
							dictionary,
							attribute_name );

				strcpy(	trimmed_attribute_name,
					attribute_name + str_len );

				sprintf(ignored_attribute_name,
		 			"%s%s", 
		 			IGNORE_PUSH_BUTTON_PREFIX, 
		 			trimmed_attribute_name );

				dictionary_set_pointer(
					dictionary,
					strdup( ignored_attribute_name ),
					data );
			}
		} while( list_next( attribute_name_list ) );
	}
} /* populate_ignore_button_for_no_display_pressed() */

LIST *appaserver_library_get_datatype_name_list( char *application )
{
	char sys_string[ 1024 ];
	sprintf( sys_string, "datatype_name_list.sh %s", application );
	return pipe2list( sys_string );
}

char *get_displayable_primary_attribute_list_string(
				LIST *primary_attribute_name_list )
{
	char buffer[ 512 ];

	return strdup( 
		format_initial_capital( buffer, 
					list_display_delimited(
						primary_attribute_name_list,
						'/' ) ) );
} /* get_displayable_primary_attribute_list_string() */

LIST *appaserver_library_get_attribute_name_list(
					char *application, 
					char *folder_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string, 
		 "attributes4folder.sh %s %s", 
		 application, folder_name );

	return pipe2list( sys_string );
} /* appaserver_library_get_attribute_name_list() */

/*
int is_primary_application( char *application_name )
{
	return application_get_is_primary_application( application_name );
}
*/

char *build_multi_attribute_key( LIST *key_list, char delimiter )
{
	char multi_attribute_key[ 1024 ];
	char *ptr = multi_attribute_key;
	char *key_string;
	int first_time = 1;

	if ( !list_rewind( key_list ) ) return (char *)0;

	do {
		key_string = list_get_pointer( key_list );

		if ( first_time )
		{
			ptr += sprintf( ptr, "%s", key_string );
			first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, "%c%s", delimiter, key_string );
		}
	} while( list_next( key_list ) );

	return strdup( multi_attribute_key );
} /* build_multi_attribute_key() */

LIST *appaserver_library_get_folder_name_list(
					char *application_name,
					char *attribute_name )
{
	char sys_string[ 1024 ];
	char *table_name;

	table_name = get_table_name( application_name, "folder_attribute" );

	sprintf(sys_string,
"echo \"select distinct folder from %s where attribute = '%s';\" | sql",
		table_name,
		attribute_name );
	return pipe2list( sys_string );
} /* appaserver_library_get_folder_name_list() */

LIST *appaserver_library_get_insert_table_element_list(
			char *login_name,
			char *application_name,
			char *session,
			char *role_name,
			LIST *attribute_list,
			LIST *include_attribute_name_list,
			LIST *mto1_related_folder_list,
			DICTIONARY *query_dictionary,
			DICTIONARY *preprompt_dictionary,
			int row_dictionary_list_length,
			LIST *no_display_pressed_attribute_name_list,
			LIST *posted_attribute_name_list,
			boolean row_level_non_owner_forbid,
			boolean override_row_restrictions,
			char *folder_post_change_javascript,
			int max_query_rows_for_drop_downs,
			char *one2m_folder_name_for_processes )
{
	LIST *return_list;
	LIST *element_list;
	LIST *ignore_attribute_name_list;
	char *attribute_name;
	RELATED_FOLDER *related_folder;
	ELEMENT *element;
	LIST *foreign_attribute_name_list;
	int objects_outputted = 0;
	LIST *primary_attribute_name_list;
	LIST *isa_folder_list;
	int max_drop_down_size = INT_MAX;
	boolean is_primary_attribute;
	DICTIONARY *parameter_dictionary;
	DICTIONARY *where_clause_dictionary;

	if ( !list_reset( include_attribute_name_list ) )
		return list_new_list();

/*
Inspiration: benthic SPECIES_MEASUREMENT has BENTHIC_SPECIES
drop-down needing SWEEP.sweep_number in the where clause.
	parameter_dictionary = dictionary_copy( preprompt_dictionary );
*/

	parameter_dictionary = dictionary_copy( query_dictionary );

	dictionary_append_dictionary(
		parameter_dictionary,
		preprompt_dictionary );

	where_clause_dictionary = dictionary_copy( preprompt_dictionary );

	isa_folder_list =
		appaserver_get_isa_folder_list(
			application_name );

	primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			attribute_list );

	return_list = list_new_list();
	ignore_attribute_name_list = list_new();

	if ( row_level_non_owner_forbid )
	{
		list_append_pointer(	ignore_attribute_name_list,
					"login_name" );
	}

	/* For each attribute */
	/* ------------------ */
	do {
		attribute_name = 
			list_get_pointer( include_attribute_name_list );

		/* If the attribute is accounted for already */
		/* ----------------------------------------- */
		if ( list_exists_string( ignore_attribute_name_list,
					 attribute_name ) )
		{
			continue;
		}

		is_primary_attribute =
			list_exists_string(	
					primary_attribute_name_list,
					attribute_name );

		if ( ( related_folder =
		       related_folder_attribute_consumes_related_folder(
			       ignore_attribute_name_list,
			       (LIST *)0 /* omit_update_attribute_name_list */,
			       mto1_related_folder_list,
			       attribute_name,
			       (LIST *)0 /* include_attribute_name_list */ ) ) )
		{
			ATTRIBUTE *attribute;

			attribute = attribute_seek_attribute( 
						attribute_list,
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
					application_name,
					msg,
					(char *)0 );
				exit( 1 );
			}

			if ( related_folder->ignore_output ) continue;

			if ( isa_folder_list
			&&   list_length( isa_folder_list )
			&&   appaserver_isa_folder_accounted_for(
				isa_folder_list,
				related_folder->folder->folder_name,
				related_folder->
					related_attribute_name ) )
			{
				continue;
			}

			foreign_attribute_name_list =
				related_folder_get_foreign_attribute_name_list(
			   	   folder_get_primary_attribute_name_list(
					related_folder->folder->
						attribute_list ),
			   	   related_folder->related_attribute_name );

			list_append_list(
				return_list,
				related_folder_get_insert_element_list(
					 application_name,
					 session,
					 login_name,
					 related_folder,
					 foreign_attribute_name_list,
					 row_dictionary_list_length,
					 parameter_dictionary,
					 where_clause_dictionary,
					 0 /* prompt_data_element_only */,
					 folder_post_change_javascript,
					 max_drop_down_size,
					 row_level_non_owner_forbid,
					 override_row_restrictions,
					 is_primary_attribute,
					 role_name,
					 max_query_rows_for_drop_downs,
					 0 /* drop_down_multi_select */,
					 related_folder->
							folder->
							no_initial_capital,
					 one2m_folder_name_for_processes
					) );

/*
			list_append_string_list( 
					ignore_attribute_name_list,
					foreign_attribute_name_list );
*/

			related_folder->ignore_output = 1;

			objects_outputted++;
		}

		if ( list_exists_string(	ignore_attribute_name_list,
						attribute_name ) )
		{
			continue;
		}

		element_list =
		   	appaserver_library_get_insert_attribute_element_list(
				&objects_outputted,
				attribute_list,
				attribute_name,
				posted_attribute_name_list,
				is_primary_attribute,
				folder_post_change_javascript,
				application_name );

		if ( element_list )
		{
			list_append_list(
				return_list,
				element_list );

			list_append_pointer(
				ignore_attribute_name_list,
				attribute_name );
		}

	} while( list_next( include_attribute_name_list ) );

	if ( no_display_pressed_attribute_name_list 
	&&   list_rewind( no_display_pressed_attribute_name_list ) )
	{
		do {
			attribute_name = list_get_string(
				no_display_pressed_attribute_name_list );

			element = element_new( hidden, attribute_name );

			list_append_pointer(
					return_list, 
					element );
		} while( list_next( no_display_pressed_attribute_name_list ) );
	}

	return return_list;

} /* appaserver_library_get_insert_table_element_list() */

LIST *appaserver_library_get_update_attribute_element_list(
					int *objects_outputted,
					ATTRIBUTE *attribute,
					char update_yn,
					LIST *primary_attribute_name_list,
					boolean is_primary_attribute,
					char *folder_post_change_javascript,
					boolean prompt_data_element_only )
{
	LIST *element_list = {0};
	LIST *return_list;
	char *post_change_javascript;

	return_list = list_new_list();

	if ( attribute->post_change_javascript
	&&   *attribute->post_change_javascript )
	{
		post_change_javascript = attribute->post_change_javascript;
	}
	else
	{
		post_change_javascript = folder_post_change_javascript;
	}

	if ( prompt_data_element_only || attribute->omit_update )
	{
		update_yn = 'n';
	}

	element_list =
		appaserver_library_get_update_lookup_attribute_element_list(
				update_yn,
				primary_attribute_name_list,
				attribute->exclude_permission_list,
				attribute->attribute_name,
				attribute->datatype,
				attribute->width,
				post_change_javascript,
				attribute->on_focus_javascript_function,
				is_primary_attribute );

	if ( element_list && list_length( element_list ) )
	{
		list_append_list( return_list, element_list );
		(*objects_outputted)++;
	}

	return return_list;

} /* appaserver_library_get_update_attribute_element_list() */

LIST *appaserver_library_get_insert_attribute_element_list(
					int *objects_outputted,
					LIST *attribute_list,
					char *attribute_name,
					LIST *posted_attribute_name_list,
					boolean is_primary_attribute,
					char *folder_post_change_javascript,
					char *application_name )
{
	ATTRIBUTE *attribute;
	LIST *element_list = {0};
	LIST *return_list;
	char *post_change_javascript;

	attribute = attribute_seek_attribute( 
					attribute_list,
					attribute_name );
	
	if ( !attribute )
	{
		char msg[ 1024 ];

		sprintf(msg,
		"%s.%s() cannot find attribute = (%s)\n",
			__FILE__,
			__FUNCTION__,
			attribute_name );
		appaserver_output_error_message(
			application_name, msg, (char *)0 );
		exit( 1 );
	}

	if ( appaserver_exclude_permission(
		attribute->exclude_permission_list,
		"insert" ) )
	{
		return (LIST *)0;
	}

	if ( strcmp( attribute->datatype, "timestamp" ) == 0 )
	{
		return (LIST *)0;
	}

	return_list = list_new_list();

	if ( attribute->post_change_javascript
	&&   *attribute->post_change_javascript )
	{
		post_change_javascript = attribute->post_change_javascript;
	}
	else
	{
		post_change_javascript = folder_post_change_javascript;
	}

	element_list =
	appaserver_library_with_attribute_get_insert_attribute_element_list(
			attribute->attribute_name,
			attribute->datatype,
			attribute->width,
			post_change_javascript,
			attribute->on_focus_javascript_function,
			posted_attribute_name_list,
			is_primary_attribute,
			attribute->omit_update );

	if ( element_list && list_length( element_list ) )
	{
		list_append_list( return_list, element_list );
		(*objects_outputted)++;
	}

	return return_list;
} /* appaserver_library_get_insert_attribute_element_list() */

LIST *appaserver_library_with_attribute_get_insert_attribute_element_list(
				char *attribute_name,
				char *datatype,
				int width,
				char *post_change_javascript,
				char *on_focus_javascript_function,
				LIST *posted_attribute_name_list,
				boolean is_primary_attribute,
				boolean omit_update )
{
	LIST *return_list;
	ELEMENT *element;

	if ( !datatype ) return (LIST *)0;

	return_list = list_new();

	if ( posted_attribute_name_list
	&&   list_exists_string( posted_attribute_name_list,
				 attribute_name ) )
	{
		element = element_new(
				hidden, 
				attribute_name );
	}
	else
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
	}
	else
	if ( strcmp( datatype, "hidden_text" ) == 0 )
	{
		element = element_new(
				hidden,
				attribute_name );
	}
	else
	if ( process_parameter_list_element_name_boolean( attribute_name ) )
	{
		element =
			element_get_yes_no_drop_down(
				attribute_name,
				(char *)0 /* prepend_folder_name */,
				post_change_javascript,
				0 /* not with_is_null */,
				0 /* not with_not_null */ );
	}
	else
	if ( strcmp( datatype, "reference_number" ) == 0 )
	{
		element = element_new( 	reference_number,
					attribute_name);

		element_reference_number_set_attribute_width(
				element->reference_number,
				width );

		if ( is_primary_attribute )
		{
			char heading[ 128 ];
			sprintf( heading, "*%s", element->name );
			element_reference_number_set_heading(
					element->reference_number,
					strdup( heading ) );
		}
		else
		{
			element_reference_number_set_heading(
					element->reference_number,
					element->name );
		}
		element->reference_number->omit_update = omit_update;
	}
	else
	{
		if ( strcmp( datatype, "date" ) == 0
		||   strcmp( datatype, "current_date" ) == 0 )
		{
			element = element_new( 	element_date,
						attribute_name);
		}
		else
		if ( strcmp( datatype, "current_date_time" ) == 0 )
		{
			element = element_new( 	element_current_date_time,
						attribute_name);
		}
		else
		{
			element = element_new( 	text_item,
						attribute_name);
		}

		element_text_item_set_attribute_width(
				element->text_item, 
				width );

		element->text_item->state = "insert";

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

		/* Note: must include validate_date.js */
		/* ----------------------------------- */
		if ( strcmp( datatype, "time" ) == 0
		||   strcmp( datatype, "current_time" ) == 0 )
		{
			post_change_javascript =
				attribute_append_post_change_javascript(
					post_change_javascript,
					"validate_time(this)",
					1 /* place_first */ );
		}
		else
		if ( strcmp( datatype, "date" ) == 0
		||   strcmp( datatype, "current_date" ) == 0 )
		{
			post_change_javascript =
				attribute_append_post_change_javascript(
					post_change_javascript,
					"validate_date(this)",
					1 /* place_first */ );
		}

		if ( post_change_javascript
		&&   *post_change_javascript )
		{
			element->text_item->post_change_javascript =
				post_change_javascript;
		}
		if ( on_focus_javascript_function
		&&   *on_focus_javascript_function )
		{
			element->text_item->on_focus_javascript_function =
				on_focus_javascript_function;
		}
	}

	list_append_pointer( return_list, element );
	return return_list;

} /* appaserver_library_with_attribute_get_insert_attribute_element_list() */

boolean appaserver_library_validate_begin_end_date(
					char **begin_date,
					char **end_date,
					char *application_name,
					char *database_management_system,
					PROCESS_GENERIC_OUTPUT *
						process_generic_output,
					DICTIONARY *post_dictionary )
{
	char buffer[ 128 ];
	int i;
	static char new_begin_date[ 16 ];
	static char new_end_date[ 16 ];

	if ( character_exists( *begin_date, '/' ) ) return 1;
	if ( character_exists( *end_date, '/' ) ) return 1;

	if ( !*begin_date && dictionary_length( post_dictionary ) )
	{
		dictionary_get_index_data(	begin_date,
						post_dictionary,
						"begin_date",
						0 );
	}

	if ( !*end_date && dictionary_length( post_dictionary ) )
	{
		dictionary_get_index_data(	end_date,
						post_dictionary,
						"end_date",
						0 );
	}

	if ( process_generic_output )
	{
		if ( ( !*begin_date || !**begin_date )
		&&   ( !*end_date || !**end_date ) )
		{
			char *where_clause;

			where_clause =
			process_generic_output_get_drop_down_where_clause(
				application_name,
				process_generic_output,
				post_dictionary );

			if ( !*where_clause ) return 0;
	
			process_generic_output_get_period_of_record_date(
					begin_date,
					application_name,
					process_generic_output->
						value_folder->
							value_folder_name,
					"min",
					process_generic_output->
						value_folder->
							date_attribute_name,
					where_clause );
			if ( !**begin_date ) return 0;
	
			process_generic_output_get_period_of_record_date(
					end_date,
					application_name,
					process_generic_output->
						value_folder->value_folder_name,
					"max",
					process_generic_output->
						value_folder->
							date_attribute_name,
					where_clause );
			return 1;
		}
	
		if ( ( !*begin_date || !**begin_date ) )
		{
			char *where_clause;

			where_clause =
			process_generic_output_get_drop_down_where_clause(
				application_name,
				process_generic_output,
				post_dictionary );

			process_generic_output_get_period_of_record_date(
					begin_date,
					application_name,
					process_generic_output->
						value_folder->value_folder_name,
					"min",
					process_generic_output->
						value_folder->
							date_attribute_name,
					where_clause );
			if ( !**begin_date ) return 0;
		}
	} /* if process_generic_output */

	if ( !*begin_date || strcmp( *begin_date, "begin_date" ) == 0 )
	{
		*begin_date = "1901-01-01";
	}

	if ( !*end_date || strcmp( *end_date, "end_date" ) == 0 )
	{
		if ( strcmp( *begin_date, "1901-01-01" ) == 0 )
		{
			char sys_string[ 128 ];

			strcpy( sys_string, "date.e 0 | piece.e ':' 0" );
			*end_date = pipe2string( sys_string );
		}
		else
		{
			*end_date = *begin_date;
		}
	}

	strcpy( new_begin_date, *begin_date );
	strcpy( new_end_date, *end_date );

	if ( database_management_system
	&&   strcmp( database_management_system, "oracle" ) == 0 )
	{
		*begin_date = new_begin_date;
		*end_date = new_end_date;
		return 1;
	}

	piece( buffer, '-', new_begin_date, 0 );
	if ( strlen( buffer ) != 4 ) return 0;

	/* Don't allow for 0000-00-00 */
	/* -------------------------- */
	if ( !atoi( buffer ) ) return 0;

	if ( !piece( buffer, '-', new_begin_date, 1 ) )
	{
		strcat( new_begin_date, "-01-01" );
	}
	else
	{
		i = atoi( buffer );
		if ( i < 1 || i > 12 ) return 0;
	}

	if ( !piece( buffer, '-', new_begin_date, 2 ) )
	{
		strcat( new_begin_date, "-01" );
	}
	else
	{
		i = atoi( buffer );
		if ( i < 1 || i > 31 ) return 0;
	}

	if ( !*new_end_date || strcmp( new_end_date, "end_date" ) == 0 )
	{
		strcpy( new_end_date, new_begin_date );
	}

	piece( buffer, '-', new_end_date, 0 );
	if ( strlen( buffer ) != 4 ) return 0;
	if ( !atoi( buffer ) ) return 0;

	if ( !piece( buffer, '-', new_end_date, 1 ) )
	{
		strcat( new_end_date, "-12-31" );
	}
	else
	{
		i = atoi( buffer );
		if ( i < 1 || i > 12 ) return 0;
	}

	if ( !piece( buffer, '-', new_end_date, 2 ) )
	{
		strcat( new_end_date, "-31" );
	}
	else
	{
		i = atoi( buffer );
		if ( i < 1 || i > 31 ) return 0;
	}

	*begin_date = new_begin_date;
	*end_date = new_end_date;

	if ( strcmp( new_begin_date, new_end_date ) > 0 )
		return 0;
	else
		return 1;

} /* appaserver_library_validate_begin_end_date() */

LIST *appaserver_library_get_primary_data_list(
					DICTIONARY *post_dictionary,
					LIST *attribute_list )
{
	LIST *primary_attribute_name_list;
	LIST *primary_data_list;
	char *data;

	primary_data_list = list_new_list();

	if ( !attribute_list
	||   !list_length( attribute_list ) )
	{
		return primary_data_list;
	}

	primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			attribute_list );

	if ( !list_rewind( primary_attribute_name_list ) )
		return primary_data_list;

	do {
		if ( !( data =
			dictionary_get_index_zero(
				post_dictionary,
				list_get_pointer(
					primary_attribute_name_list ) ) ) )
		{
			return list_new_list();
		}

		list_append_pointer( primary_data_list, data );
	} while( list_next( primary_attribute_name_list ) );
	return primary_data_list;
} /* appaserver_library_get_primary_data_list() */

char *appaserver_library_get_server_address( void )
{
	char server_address[ 128 ] = {0};

	if ( !environ_name_to_value( server_address, "HTTP_HOST" ) )
	{
		if ( !environ_name_to_value(
					server_address,
					"SERVER_NAME" ) )
		{
			if ( !environ_name_to_value(
					server_address,
					"SERVER_ADDR" ) )
			{
				environ_name_to_value(
						server_address,
						"HOSTNAME" );
			}
		}
	}

	return strdup( server_address );

} /* appaserver_library_get_server_address() */

void appaserver_library_output_ftp_prompt(
				char *output_filename,
				char *prompt,
				char *target,
				char *mime_type )
{
	char mime_type_prompt[ 128 ];

	if ( mime_type && *mime_type )
	{
		sprintf( mime_type_prompt,
			 "type=\"%s\"",
			 mime_type );
	}
	else
	{
		*mime_type_prompt = '\0';
	}

	if ( target && *target )
	{
		printf(
		"<br><a %s href=\"%s\" target=\"%s\">%s</a>\n",
			mime_type_prompt,
			output_filename,
			target,
			prompt );
	}
	else
	{
		printf(
		"<br><a %s href=\"%s\">%s</a>\n",
			mime_type_prompt,
			output_filename,
			prompt );
	}
	
} /* appaserver_library_output_ftp_prompt() */

char *appaserver_library_get_http_prompt(
				 	char *cgi_directory,
				 	char *server_address,
					char ssl_support_yn,
					char prepend_http_protocol_yn )
{
	static char http_prompt[ 512 ];

	if ( prepend_http_protocol_yn == 'y' )
	{
		if ( ssl_support_yn == 'y' )
		{
/*
			sprintf( http_prompt,
				 "https://%s%s/suid/~%s",
			 	 server_address,
			 	 cgi_directory,
				 appaserver_library_get_whoami() );
*/
			sprintf( http_prompt,
			 	"https://%s%s",
			 	server_address,
			 	cgi_directory );
		}
		else
		{
			sprintf( http_prompt,
			 	"http://%s%s",
			 	server_address,
			 	cgi_directory );
		}
	}
	else
	{
		sprintf( http_prompt, "%s", cgi_directory );
	}

	return http_prompt;

} /* appaserver_library_get_http_prompt() */

char *appaserver_library_get_whoami( void )
{
	char sys_string[ 64 ];

	strcpy( sys_string, "whoami" );
	return pipe2string( sys_string );
}

boolean appaserver_library_get_from_preprompt( DICTIONARY *dictionary )
{
	char key[ 128 ];
	char *data;

	if ( !dictionary ) return 0;

	sprintf( key, "%s", FROM_PREPROMPT );
	if ( ! ( data = dictionary_get_pointer( dictionary, key ) ) )
		return 0;

	return ( strcmp( data, "yes" ) == 0 );
} /* appaserver_library_get_from_preprompt() */

LIST *appaserver_library_get_omit_insert_prompt_attribute_name_list(
						LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	LIST *omit_insert_prompt_attribute_name_list = list_new();

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = (ATTRIBUTE *)
				list_get_pointer( attribute_list );

			if ( list_exists_string(
				attribute->exclude_permission_list,
				"insert" )
			||   attribute->omit_insert_prompt
			||   attribute->omit_insert )
			{
				list_append_pointer(
					omit_insert_prompt_attribute_name_list,
					attribute->attribute_name );
			}

		} while( list_next( attribute_list ) );
	}
	return omit_insert_prompt_attribute_name_list;
} /* appaserver_library_get_omit_insert_prompt_attribute_name_list() */

LIST *appaserver_library_get_omit_insert_attribute_name_list(
						LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	LIST *omit_insert_attribute_name_list = list_new();

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = (ATTRIBUTE *)
				list_get_pointer( attribute_list );

			if ( attribute->omit_insert )
			{
				list_append_pointer(
					omit_insert_attribute_name_list,
					attribute->attribute_name );
			}

		} while( list_next( attribute_list ) );
	}
	return omit_insert_attribute_name_list;
} /* appaserver_library_get_omit_insert_attribute_name_list() */

LIST *appaserver_library_get_prompt_data_element_list(
				char *attribute_name,
				boolean is_primary_attribute )
{
	ELEMENT *element;
	char prompt_data_heading[ 128 ];
	LIST *element_list = list_new_list();

	element = element_new(
		prompt_data,
		attribute_name );

	if ( is_primary_attribute )
	{
		sprintf( prompt_data_heading, "*%s", attribute_name );
		element->prompt_data->heading =
			strdup( prompt_data_heading );
	}
	else
	{
		*prompt_data_heading = '\0';
	}

	list_append_pointer(
			element_list, 
			element );

	/* ------------------------- */
	/* Create a hidden element   */
	/* so delete will work.      */
	/* ------------------------- */
	element = element_new( 
		hidden,
		attribute_name );

	list_append_pointer(
			element_list, 
			element );
	return element_list;
} /* appaserver_library_get_prompt_data_element_list() */

void appaserver_library_output_style_sheet(
					FILE *output_file, 
					char *application_name )
{
	char buffer[ 1024 ];
	FILE *input_file;

	fprintf( output_file, "<style>\n" );
	fflush( output_file );

	sprintf( buffer,
		 "%s/appaserver/%s/style.css",
		 appaserver_parameter_file_get_document_root(),
		 application_name );

	input_file = fopen( buffer, "r" );
	if ( !input_file )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 buffer );
		exit( 1 );
	}

	while( get_line( buffer, input_file ) )
		fprintf( output_file, "%s\n", buffer );

	fclose( input_file );
	fflush( stdout );
	fprintf( output_file,
		 "</style>\n" );
} /* appaserver_library_output_style_sheet() */

LIST *appaserver_library_trim_carrot_number( LIST *data_list )
{
	char *data;
	char *ptr;

	if ( !list_rewind( data_list ) )
		return data_list;

	do {
		data = list_get_pointer( data_list );
		ptr = data + strlen( data );
		while( ptr > data )
		{
			if ( *ptr == '^' )
			{
				*ptr = '\0';
				break;
			}
			ptr--;
		}
				
	} while( list_next( data_list ) );

	return data_list;
} /* appaserver_library_trim_carrot_number() */

void appaserver_library_set_vertical_new_button_folder_name(
				DICTIONARY *non_prefixed_dictionary,
				char *vertical_new_button_folder_name,
				char *prefix )
{
	char key[ 128 ];

	sprintf( key,
		 "%s%s_0",
		 prefix,
		 vertical_new_button_folder_name );

	dictionary_set_pointer(
			non_prefixed_dictionary,
			strdup( key ),
			"yes" );

} /* appaserver_library_set_vertical_new_button_folder_name() */

char *appaserver_library_get_vertical_new_button_folder_name(
				DICTIONARY *non_prefixed_dictionary,
				char *prefix )
{
	LIST *key_list;
	char *key;
	char *data;
	char return_key[ 128 ];

	if ( !non_prefixed_dictionary ) return (char *)0;

	key_list = dictionary_extract_prefixed_key_list(
					non_prefixed_dictionary,
					prefix );

	if ( !list_rewind( key_list ) ) return (char *)0;

	do {
		key = list_get_pointer( key_list );
		data = dictionary_get_data( non_prefixed_dictionary, key );
		if ( strcmp( data, "yes" ) == 0 )
		{
			return strdup(
				trim_index(
				return_key,
				key +
				strlen( prefix ) ) );
		}
	} while( list_next( key_list ) );
	return (char *)0;
} /* appaserver_library_get_vertical_new_button_folder_name() */

char *appaserver_library_get_verify_attribute_widths_submit_control_string(
					LIST *element_list,
					char *database_management_system,
					char *source_form )
{
	ELEMENT *element;
	char buffer[ 128 ];
	char local_element_heading[ 128 ];
	static char submit_control_string[ FORM_SUBMIT_CONTROL_STRING_SIZE ];
	char *element_name;
	int element_attribute_width;
	char *element_heading;
	char element_name_list_string[
		FORM_SUBMIT_CONTROL_STRING_SIZE ];
	char *name_ptr = element_name_list_string;
	char element_heading_list_string[
		FORM_SUBMIT_CONTROL_STRING_SIZE ];
	char *heading_ptr = element_heading_list_string;
	char element_attribute_width_list_string[
		FORM_SUBMIT_CONTROL_STRING_SIZE ];
	char *width_ptr = element_attribute_width_list_string;
	int first_time = 1;

	if ( !list_rewind( element_list ) ) return (char *)0;

	do {
		element = list_get_pointer( element_list );
		element_name = element->name;
		element_attribute_width =
			element_get_attribute_width( element );
		element_heading =
			element_get_heading( (char **)0, element, 0 );

		if ( element_attribute_width
		&&   strncmp( element_name, "to_", 3 ) != 0 )
		{
			if ( first_time )
			{
				name_ptr += sprintf( name_ptr, "'" );
				heading_ptr += sprintf( heading_ptr, "'" );
				width_ptr += sprintf( width_ptr, "'" );
				first_time = 0;
			}
			else
			{
				name_ptr += sprintf( name_ptr, "," );
				heading_ptr += sprintf( heading_ptr, "," );
				width_ptr += sprintf( width_ptr, "," );
			}

			name_ptr += sprintf( name_ptr, "%s_0", element_name );

			if ( element_heading
			&&   *element_heading
			&&    strncmp( element_heading, "from_", 5 ) == 0 )
			{
				strcpy( local_element_heading,
					element_heading + 5 );
			}
			else
			{
				if ( element_heading && *element_heading )
				{
					strcpy( local_element_heading,
						element_heading );
				}
				else
				{
					strcpy( local_element_heading, "" );
				}
			}

			heading_ptr +=
				sprintf( heading_ptr,
					"%s",
					format_initial_capital(
						buffer,
						local_element_heading ) );

			width_ptr += sprintf( 	width_ptr,
						"%d",
						element_attribute_width );
		}

	} while( list_next( element_list ) );

	name_ptr += sprintf( name_ptr, "'" );
	heading_ptr += sprintf( heading_ptr, "'" );
	width_ptr += sprintf( width_ptr, "'" );

	if ( strcmp( element_name_list_string, "'" ) == 0 )
		return (char *)0;

	if (	strlen( element_name_list_string ) +
		strlen( element_heading_list_string ) +
		strlen( element_attribute_width_list_string ) +
		strlen( database_management_system ) +
		strlen( source_form ) +
		40 > FORM_SUBMIT_CONTROL_STRING_SIZE )
	{
		fprintf( stderr,
		"ERROR in %s/%s(): data too big for string size.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	sprintf(submit_control_string,
		"verify_attribute_widths(%s,%s,%s,'%s','%s') && ",
		element_name_list_string,
		element_heading_list_string,
		element_attribute_width_list_string,
		database_management_system,
		source_form );

	return submit_control_string;
} /* appaserver_library_get_verify_attribute_widths_submit_control_string() */

void appaserver_library_dictionary_convert_date(
						DICTIONARY *dictionary,
						char *application_name,
						char *date_string,
						char *key )
{
	DATE_CONVERT *date_convert;
	char destination[ 1024 ];
	char date_piece_string[ 64 ];
	char *destination_ptr = destination;
	int i;

	if ( !date_string || !*date_string ) return;

	*destination = '\0';

	for( i = 0; piece( date_piece_string, ',', date_string, i ); i++ )
	{
		date_convert =
			date_convert_new_database_format_date_convert(
				application_name,
				date_piece_string );

		if ( !date_convert )
		{
			fprintf( stderr,
			 "Error in %s/%s(): cannot fetch from database\n",
		 		__FILE__,
		 		__FUNCTION__ );
			exit( 1 );
		}

		if ( i ) destination_ptr += sprintf( destination_ptr, "," );

		destination_ptr += sprintf(	destination_ptr,
						"%s",
						date_convert->return_date );

		date_convert_free( date_convert );
	} /* for each comma delimited date */

	dictionary_set_pointer(
		dictionary,
		key,
		strdup( destination ) );

} /* appaserver_library_dictionary_convert_date() */

void appaserver_library_dictionary_database_convert_begin_end_dates(
					DICTIONARY *dictionary,
					char *application_name )
{
	char *date_string;
	char *key;

	key = "begin_date_0";


	if ( ( date_string = dictionary_fetch( dictionary, key ) ) )
	{
		appaserver_library_dictionary_convert_date( 
				dictionary,
				application_name,
				date_string,
				key );
	}

	key = "end_date_0";

	if ( ( date_string = dictionary_fetch( dictionary, key ) ) )
	{
		appaserver_library_dictionary_convert_date( 
				dictionary,
				application_name,
				date_string,
				key );
	}

	key = "query_from_begin_date_0";

	if ( ( date_string = dictionary_fetch( dictionary, key ) ) )
	{
		appaserver_library_dictionary_convert_date( 
				dictionary,
				application_name,
				date_string,
				key );
	}

	key = "query_from_end_date_0";

	if ( ( date_string = dictionary_fetch( dictionary, key ) ) )
	{
		appaserver_library_dictionary_convert_date( 
				dictionary,
				application_name,
				date_string,
				key );
	}

	key = "from_begin_date_0";

	if ( ( date_string = dictionary_fetch( dictionary, key ) ) )
	{
		appaserver_library_dictionary_convert_date( 
				dictionary,
				application_name,
				date_string,
				key );
	}

	key = "from_end_date_0";

	if ( ( date_string = dictionary_fetch( dictionary, key ) ) )
	{
		appaserver_library_dictionary_convert_date( 
				dictionary,
				application_name,
				date_string,
				key );
	}

} /* appaserver_library_dictionary_database_convert_begin_end_dates() */

void appaserver_library_post_dictionary_database_convert_dates(
					DICTIONARY *post_dictionary,
					char *application_name,
					LIST *attribute_list )
{
	LIST *date_attribute_name_list;
	char *attribute_name;
	char key[ 128 ];
	char *date_string;
	int index;
	int highest_index;

	date_attribute_name_list =
		attribute_get_date_attribute_name_list(
			attribute_list );

	if ( !list_length( date_attribute_name_list ) ) return;

	highest_index = dictionary_get_highest_index( post_dictionary );

	for( index = 0; index <= highest_index; index++ )
	{
		list_rewind( date_attribute_name_list );
		do {
			attribute_name =
				list_get_pointer( date_attribute_name_list );

			if ( index == 0 )
			{
				/* Do without prefix and suffix */
				/* ---------------------------- */
				strcpy(	key, attribute_name );

				date_string = dictionary_fetch(
						post_dictionary,
						key );

				appaserver_library_dictionary_convert_date( 
					post_dictionary,
					application_name,
					date_string,
					key );
			}

			/* Do without prefix */
			/* ----------------- */
			sprintf(	key,
					"%s_%d",
					attribute_name,
					index );

			date_string = dictionary_fetch(
					post_dictionary,
					key );

			appaserver_library_dictionary_convert_date( 
				post_dictionary,
				application_name,
				date_string,
				key );

			/* Do from date */
			/* ------------ */
			sprintf(	key,
					"%s%s_%d",
					QUERY_FROM_STARTING_LABEL,
					attribute_name,
					index );

			date_string = dictionary_fetch(
					post_dictionary,
					key );

			appaserver_library_dictionary_convert_date( 
				post_dictionary,
				application_name,
				date_string,
				key );

			/* Do to date */
			/* ---------- */
			sprintf(	key,
					"%s%s_%d",
					QUERY_TO_STARTING_LABEL,
					attribute_name,
					index );
	
			date_string = dictionary_fetch(
					post_dictionary,
					key );

			appaserver_library_dictionary_convert_date( 
				post_dictionary,
				application_name,
				date_string,
				key );

		} while( list_next( date_attribute_name_list ) );
	} /* for each index */
} /* appaserver_library_post_dictionary_database_convert_dates() */

void appaserver_library_dictionary_convert_dates(
					DICTIONARY *dictionary,
					char *application_name )
{
	LIST *key_list = dictionary_get_key_list( dictionary );
	char *key;
	char *date_string;
	enum date_convert_format date_convert_format;
	enum date_convert_format database_date_convert_format;
	char *database_string = {0};

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	database_date_convert_format =
		date_convert_get_database_date_format(
				application_name );

	if ( !list_reset( key_list ) ) return;

	do {
		key = list_get_string( key_list );

		if ( ( date_string =
			(char *)dictionary_get( dictionary, key ) ) )
		{
			date_convert_format =
				date_convert_date_get_format(
					date_string );

			if ( date_convert_format != date_convert_unknown
			&& date_convert_format != database_date_convert_format )
			{
				appaserver_library_dictionary_convert_date( 
					dictionary,
					application_name,
					date_string,
					key );
			}
		}
	} while( list_next( key_list ) );
	list_free_container( key_list );
} /* appaserver_library_dictionary_convert_dates() */

void appaserver_library_list_database_convert_dates(
					LIST *data_list,
					char *application_name,
					LIST *attribute_list )
{
	LIST *date_attribute_name_list;
	LIST *primary_attribute_name_list;
	char *attribute_name;
	char *data;
	DATE_CONVERT *date_convert = {0};

	date_attribute_name_list =
		attribute_get_date_attribute_name_list(
			attribute_list );

	primary_attribute_name_list =
		attribute_get_primary_attribute_name_list(
			attribute_list );

	if ( !list_rewind( primary_attribute_name_list ) ) return;
	if ( !list_rewind( data_list ) ) return;

	do {
		attribute_name =
			list_get_pointer( primary_attribute_name_list );

		if ( list_exists_string(	date_attribute_name_list,
						attribute_name ) )
		{
			data = list_get_pointer( data_list );

			if ( !date_convert )
			{
				date_convert =
				date_convert_new_database_format_date_convert(
						application_name,
						data );
	
				if ( !date_convert )
				{
					fprintf( stderr,
			 "Error in %s/%s(): cannot fetch from database\n",
				 		__FILE__,
				 		__FUNCTION__ );
					exit( 1 );
				}
			}
			else
			{
				date_convert_populate_return_date(
					date_convert->return_date,
					date_convert->source_format,
					date_convert->destination_format,
					data );
			}
			list_set_current(
				data_list,
				strdup( date_convert->return_date ) );
		}
		if ( !list_next( data_list ) ) break;
	} while( list_next( primary_attribute_name_list ) );
	if ( date_convert ) date_convert_free( date_convert );
} /* appaserver_library_list_database_convert_dates() */

void appaserver_library_output_calendar_javascript( void )
{
		fflush( stdout );
		printf(
"<link rel=stylesheet type=text/css href=/%s/src/css/jscal2.css />\n"
"<link rel=stylesheet type=text/css href=/%s/src/css/border-radius.css />\n"
"<link rel=stylesheet type=text/css href=/%s/src/css/gold/gold.css />\n"
"<script type=text/javascript src=/%s/src/js/jscal2.js></script>\n"
"<script type=text/javascript src=/%s/src/js/lang/en.js></script>\n",
			CALENDAR_RELATIVE_DIRECTORY,
			CALENDAR_RELATIVE_DIRECTORY,
			CALENDAR_RELATIVE_DIRECTORY,
			CALENDAR_RELATIVE_DIRECTORY,
			CALENDAR_RELATIVE_DIRECTORY );
		fflush( stdout );
} /* appaserver_library_output_calendar_javascript() */

int appaserver_library_get_reference_number(
				char *application_name,
				int insert_rows_number )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "reference_number.sh \"%s\" %d 2>>%s",
		 application_name,
		 insert_rows_number,
		 appaserver_get_error_filename( application_name ) );
	return atoi( pipe2string( sys_string ) );
} /* appaserver_library_get_reference_number() */

char *appaserver_library_get_prelookup_button_control_string(
				char *application_name,
				char *cgi_directory,
				char *server_address,
				char *login_name,
				char *database_string,
				char *session,
				char *folder_name,
				char *lookup_before_drop_down_base_folder_name,
				char *role_name,
				char *state )
{
	static char control_string[ 1024 ];
	char *use_folder_name;

	if ( lookup_before_drop_down_base_folder_name )
		use_folder_name = lookup_before_drop_down_base_folder_name;
	else
		use_folder_name = folder_name;

	sprintf(control_string,
		"%s/post_choose_folder?%s+%s+%s+%s+%s+%s",
		appaserver_library_get_http_prompt(
			cgi_directory,
		 	server_address,
			application_get_ssl_support_yn(
				application_name ),
		       application_get_prepend_http_protocol_yn(
				application_name ) ),
		 login_name,
		 timlib_get_parameter_application_name(
			application_name,
			database_string ),
		 session,
		 use_folder_name,
		 role_name,
		 state );
	return control_string;
} /* appaserver_library_get_prelookup_button_control_string() */

void appaserver_library_populate_last_foreign_attribute_key(
			DICTIONARY *post_dictionary,
			LIST *mto1_related_folder_list,
			LIST *primary_attribute_name_list )
{
	RELATED_FOLDER *related_folder;
	char *last_primary_attribute_name;
	char *last_foreign_attribute_name;
	char *data;
	char key[ 128 ];

	if ( !post_dictionary ) return;

	if ( !list_rewind( mto1_related_folder_list ) ) return;

	do {
		related_folder = list_get_pointer( mto1_related_folder_list );

		/* Populate the dictionary with the last related primary key. */
		/* ---------------------------------------------------------- */
		last_foreign_attribute_name =
			list_get_last_pointer(
				related_folder->
					folder->
					primary_attribute_name_list );

		sprintf( key, "%s_0", last_foreign_attribute_name );

		if ( ( data =
			dictionary_get_pointer(
				post_dictionary, key ) ) );
		{
			last_primary_attribute_name =
				list_get_last_pointer(
					primary_attribute_name_list );

			sprintf( key, "%s_0", last_primary_attribute_name );

			dictionary_set_pointer(
				post_dictionary,
				strdup( key ),
				data );
		}


		sprintf(	key,
				"%s%s_0",
				QUERY_RELATION_OPERATOR_STARTING_LABEL,
				last_foreign_attribute_name );

		if ( ( data =
			dictionary_get_pointer(
				post_dictionary, key ) ) );
		{
			last_primary_attribute_name =
				list_get_last_pointer(
					primary_attribute_name_list );
			sprintf(key,
				"%s%s_0",
				QUERY_RELATION_OPERATOR_STARTING_LABEL,
				last_primary_attribute_name );

			dictionary_set_pointer(
				post_dictionary,
				strdup( key ),
				data );
		}

		/* Populate the dictionary with the last */
		/* parameter primary key.		 */
		/* ------------------------------------- */
		last_foreign_attribute_name =
			list_get_last_pointer(
					primary_attribute_name_list );

		sprintf( key, "%s_0", last_foreign_attribute_name );

		if ( ( data =
			dictionary_get_pointer(
				post_dictionary, key ) ) );
		{
			last_primary_attribute_name =
				list_get_last_pointer(
					related_folder->
						folder->
						primary_attribute_name_list );

			sprintf( key, "%s_0", last_primary_attribute_name );

			dictionary_set_pointer(
				post_dictionary,
				strdup( key ),
				data );
		}

		sprintf(	key,
				"%s%s_0",
				QUERY_RELATION_OPERATOR_STARTING_LABEL,
				last_foreign_attribute_name );

		if ( ( data =
			dictionary_get_pointer(
				post_dictionary, key ) ) );
		{
			last_primary_attribute_name =
				list_get_last_pointer(
					related_folder->
						folder->
						primary_attribute_name_list );
			sprintf(key,
				"%s%s_0",
				QUERY_RELATION_OPERATOR_STARTING_LABEL,
				last_primary_attribute_name );

			dictionary_set_pointer(
				post_dictionary,
				strdup( key ),
				data );
		}

	} while( list_next( mto1_related_folder_list ) );

} /* appaserver_library_populate_last_foreign_attribute_key() */

boolean appaserver_library_get_from_php( DICTIONARY *post_dictionary )
{
	char *filename;
	char *extension;

	if ( !( filename = dictionary_fetch(
				post_dictionary,
				"filename" ) ) )
	{
		return 0;
	}

	extension = basename_get_extension( filename );
	return ( strcmp( extension, "php" ) == 0 );
} /* appaserver_library_get_from_php() */

boolean appaserver_library_application_exists(
				char *application,
				char *appaserver_error_directory )
{
	char sys_string[ 1024 ];
	char *results;

	if ( timlib_strcmp( application, "mysql" ) == 0
	||   timlib_strcmp( application, "appaserver" ) == 0
	||   timlib_strcmp( application, "root" ) == 0
	||   timlib_strcmp( application, "test" ) == 0
	||   timlib_strcmp( application, "system" ) == 0 )
	{
		return 1;
	}

	sprintf( sys_string,
		 "stat.e %s/appaserver_%s.err 2>&1 | grep '^\\.filename'",
		 appaserver_error_directory,
		 application );

	results = pipe2string( sys_string );

	return (boolean)results;
/*
	sprintf( sys_string,
	 	"stat.e %s/%s 2>&1 | grep '^\\.filename'",
	 	document_root_directory,
	 	application );

	results = pipe2string( sys_string );

	return (boolean)results;
	if ( (boolean)results ) return 1;

	sprintf( sys_string,
		 "cat /etc/passwd | grep '^%s:'",
		 application );

	results = pipe2string( sys_string );
	return (boolean)results;
*/
} /* appaserver_library_application_exists() */

char *appaserver_library_get_default_role_name(
				char *application_name,
				char *login_name )
{
	char sys_string[ 1024 ];
	char *results;

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=role			"
		 "			folder=login_default_role	"
		 "			where=\"login_name = '%s'\"	",
		 application_name,
		 login_name );

	results = pipe2string( sys_string );

	if ( !results )
		return "";
	else
		return results;

} /* appaserver_library_get_default_role_name() */

int appaserver_library_add_operations(
			LIST *element_list,
			int objects_outputted,
			LIST *operation_list,
			char *delete_isa_only_folder_name )
{
	ELEMENT *element;
	OPERATION *operation;
	char heading[ 128 ];
	char buffer[ 128 ];

	if ( !operation_list || !list_reset( operation_list ) )
		return objects_outputted;

	do {
		objects_outputted++;
		operation = list_get_pointer( operation_list );

		if ( operation->empty_placeholder_instead )
		{
			element = element_new( empty_column, (char *)0 );

			list_append(	element_list, 
					element, 
					sizeof( ELEMENT ) );

			continue;
		}

		element = element_new(
				push_button,
				operation->
					process->
					process_name);
	
		if ( timlib_begins_string(
			operation->process->process_name,
			"delete" ) )
		{
			if ( delete_isa_only_folder_name
			&&   *delete_isa_only_folder_name
			&&   timlib_strcmp(
				operation->process->process_name,
				"delete_isa_only" ) == 0 )
				
			{
				sprintf( heading,
					 "delete %s only",
					 delete_isa_only_folder_name );

				format_initial_capital(
					heading,
					heading );

				element->push_button->heading =
					strdup( heading );
			}

/*
			element->push_button->image_source =
				"/zimages/trashcan.gif";
*/
			sprintf( buffer,
				 "/%s/trashcan.gif",
				 IMAGE_RELATIVE_DIRECTORY );

			element->push_button->image_source =
				strdup( buffer );

			element->push_button->onclick_function =
				"timlib_delete_button_warning()";
		}
		else
		if ( strcmp( operation->process->process_name,
			     "detail" ) == 0 )
		{
			sprintf( buffer,
				 "/%s/magnify_glass.gif",
				 IMAGE_RELATIVE_DIRECTORY );

			element->push_button->image_source =
				strdup( buffer );
/*
			element->push_button->image_source =
				"/zimages/magnify_glass.gif";
*/
		}

		list_append(	element_list, 
				element, 
				sizeof( ELEMENT ) );

	} while( list_next( operation_list ) );

	return objects_outputted;

} /* appaserver_library_add_operations() */

boolean appaserver_library_exists_javascript_folder(
				char *application_name,
				char *folder_name )
{
	char where[ 128 ];
	char sys_string[ 256 ];

	sprintf( where, "folder = '%s'", folder_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=count			"
		 "			folder=javascript_folders	"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return atoi( pipe2string( sys_string ) );
} /* appaserver_library_exists_javascript_folder() */

void appaserver_library_purge_temporary_files( char *application_name )
{
	char sys_string[ 128 ];

	sprintf( sys_string,
		 "appaserver_purge_temporary_files.sh %s &",
		 application_name );

	system( sys_string );
}

void appaserver_library_automatically_set_login_name(
				DICTIONARY *query_dictionary,
				char *login_name,
				LIST *mto1_related_folder_list,
				LIST *attribute_list,
				LIST *role_attribute_exclude_list )
{
	char *appaserver_user_foreign_login_name;
	boolean set_login_name = 0;

	if ( ( appaserver_user_foreign_login_name =
		related_folder_get_appaserver_user_foreign_login_name(
			mto1_related_folder_list ) ) )
	{
		if ( role_exists_attribute_exclude_insert(
					role_attribute_exclude_list,
					appaserver_user_foreign_login_name ) )
		{
			set_login_name = 1;
		}

		if ( !set_login_name )
		{
			ATTRIBUTE *attribute;

			attribute = attribute_seek_attribute(
					attribute_list,
					appaserver_user_foreign_login_name );

			if ( attribute )
			{
				if ( attribute->omit_insert )
					set_login_name = 1;
				else
				if ( attribute->omit_insert_prompt )
					set_login_name = 1;
			}
		}

		if ( set_login_name )
		{
			dictionary_set_pointer(
				query_dictionary,
				appaserver_user_foreign_login_name,
				login_name );
		}
	}

} /* appaserver_library_automatically_set_login_name() */

char *appaserver_library_preupdate_change_state_display(
			enum preupdate_change_state preupdate_change_state )
{
	if ( preupdate_change_state == from_null_to_something )
		return "from_null_to_something";
	else
	if ( preupdate_change_state == from_something_to_null )
		return "from_something_to_null";
	else
	if ( preupdate_change_state == from_something_to_something_else )
		return "from_something_to_something_else";
	else
		return "no_change";

} /* appaserver_library_preupdate_change_state_display() */

enum preupdate_change_state appaserver_library_get_preupdate_change_state(
				char *preupdate_data,
				char *postupdate_data,
				char *preupdate_placeholder_name )
{
	/* ------------------------------------------ */
	/* If a numeric attribute is being checked.   */
	/* Note: can't return from_something_to_null. */
	/* ------------------------------------------ */
	if ( !postupdate_data )
	{
		if ( !preupdate_data ||	!preupdate_placeholder_name )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: null parameters.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
		else
		if ( !*preupdate_data
		||   strcmp( preupdate_data, "select" ) == 0 )
		{
			return from_null_to_something;
		}
		else
		if ( strcmp( preupdate_data, preupdate_placeholder_name ) != 0 )
		{
			return from_something_to_something_else;
		}
		else
		{
			return no_change;
		}
	}

	if (	!preupdate_data
	||	!postupdate_data
	||	!preupdate_placeholder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: null parameters.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( strcmp( preupdate_data, preupdate_placeholder_name ) == 0 )
	{
		return no_change;
	}
	else
	if ( !*preupdate_data && !*postupdate_data )
	{
		return no_change;
	}
	else
	if ( !*preupdate_data
	||   strcmp( preupdate_data, "select" ) == 0 )
	{
		return from_null_to_something;
	}
	else
	if ( !*postupdate_data
	||   strcmp( postupdate_data, "null" ) == 0
	||   strcmp( postupdate_data, "/" ) == 0 )
	{
		return from_something_to_null;
	}
	else
	{
		return from_something_to_something_else;
	}

} /* appaserver_library_get_preupdate_change_state() */

char *appaserver_library_get_sort_attribute_name( LIST *attribute_list )
{
	if ( attribute_list_exists(
				attribute_list,
				SORT_ORDER_ATTRIBUTE_NAME ) )
	{
		return SORT_ORDER_ATTRIBUTE_NAME;
	}
	else
	if ( attribute_list_exists(
				attribute_list,
				DISPLAY_ORDER_ATTRIBUTE_NAME ) )
	{
		return DISPLAY_ORDER_ATTRIBUTE_NAME;
	}
	else
	if ( attribute_list_exists(
				attribute_list,
				SEQUENCE_NUMBER_ATTRIBUTE_NAME ) )
	{
		return SEQUENCE_NUMBER_ATTRIBUTE_NAME;
	}

	return (char *)0;

} /* appaserver_library_get_sort_attribute_name() */

LIST *appaserver_library_get_update_lookup_attribute_element_list(
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
	ELEMENT *element = {0};

	return_list = list_new();

	if ( timlib_strcmp(
			datatype,
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

		if ( timlib_strcmp(
				datatype, 
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

	if ( timlib_strcmp( datatype, "notepad" ) == 0 )
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
	if ( timlib_strcmp( datatype, "password" ) == 0 )
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
	if ( timlib_strcmp( datatype, "hidden_text" ) == 0 )
	{
		element = element_new(
				hidden, 
				attribute_name );
	}
	else
	if ( timlib_strcmp( datatype, "timestamp" ) == 0 )
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
	if ( timlib_strcmp( datatype, "reference_number" ) == 0 )
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
	if ( timlib_strcmp( datatype, "date" ) == 0
	||   timlib_strcmp( datatype, "date_time" ) == 0 )
	{
		element = element_new( 	element_date,
					attribute_name );

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
	if ( timlib_strcmp( datatype, "current_date" ) == 0 )
	{
		element = element_new( 	element_current_date,
					attribute_name );

		element_text_item_set_attribute_width(
				element->text_item, 
				width );

		element->text_item->dont_create_current_date = 1;

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
	if ( timlib_strcmp( datatype, "current_date_time" ) == 0 )
	{
		element = element_new( 	element_current_date_time,
					attribute_name );

		element_text_item_set_attribute_width(
				element->text_item, 
				width );

		element->text_item->dont_create_current_date = 1;

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
	if ( timlib_strcmp( datatype, "text" ) == 0
	||   timlib_strcmp( datatype, "integer" ) == 0
	||   timlib_strcmp( datatype, "time" ) == 0
	||   timlib_strcmp( datatype, "float" ) == 0 )
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

	if ( element )
	{
		list_append_pointer( return_list, element );
	}
	else
	if ( datatype )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: could not assign an element to datatype = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 datatype );
	}

	return return_list;

} /* appaserver_library_get_update_lookup_attribute_element_list() */

