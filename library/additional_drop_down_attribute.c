/* library/additional_drop_down_attribute.c				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver role ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "piece.h"
#include "timlib.h"
#include "appaserver_library.h"
#include "additional_drop_down_attribute.h"


ADDITIONAL_DROP_DOWN_ATTRIBUTE *additional_drop_down_attribute_new(
		char *application_name,
		char *login_name,
		char *drop_down_folder_name )
{
	ADDITIONAL_DROP_DOWN_ATTRIBUTE *
		additional_drop_down_attribute;

	if ( ! ( additional_drop_down_attribute =
		calloc( 1, sizeof( ADDITIONAL_DROP_DOWN_ATTRIBUTE ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: memory allocation.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	additional_drop_down_attribute->application_name = application_name;
	additional_drop_down_attribute->login_name = login_name;
	additional_drop_down_attribute->drop_down_folder_name =
		drop_down_folder_name;

	additional_drop_down_attribute_populate_folder_attribute(
			&additional_drop_down_attribute->folder_name,
			&additional_drop_down_attribute->attribute_name,
			additional_drop_down_attribute->application_name,
			additional_drop_down_attribute->drop_down_folder_name );

	additional_drop_down_attribute->
		additional_user_drop_down_attribute_list =
			additional_drop_down_attribute_get_user_list(
				additional_drop_down_attribute->
					application_name,
				additional_drop_down_attribute->
					drop_down_folder_name,
				additional_drop_down_attribute->
					login_name );
	return additional_drop_down_attribute;

} /* additional_drop_down_attribute_new() */

ADDITIONAL_USER_DROP_DOWN_ATTRIBUTE *additional_user_drop_down_attribute_new(
			char *folder_name,
			char *attribute_name )
{
	ADDITIONAL_USER_DROP_DOWN_ATTRIBUTE *
		additional_user_drop_down_attribute;

	if ( ! ( additional_user_drop_down_attribute =
		calloc( 1, sizeof( ADDITIONAL_USER_DROP_DOWN_ATTRIBUTE ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: memory allocation.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	additional_user_drop_down_attribute->folder_name = folder_name;
	additional_user_drop_down_attribute->attribute_name = attribute_name;
	return additional_user_drop_down_attribute;

} /* additional_user_drop_down_attribute_new() */

void additional_drop_down_attribute_populate_folder_attribute(
			char **folder_name,
			char **attribute_name,
			char *application_name,
			char *drop_down_folder_name )
{
	char sys_string[ 1024 ];
	char *results_string;
	char return_folder_name[ 128 ];
	char return_attribute_name[ 128 ];
	char where_clause[ 128 ];
	char *from_folder_name = "additional_drop_down_attribute";

	sprintf( where_clause,
		 "drop_down_folder = '%s'",
		 drop_down_folder_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=folder,attribute	"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 from_folder_name,
		 where_clause );

	if ( ( results_string = pipe2string( sys_string ) ) )
	{
		piece(	return_folder_name,
			FOLDER_DATA_DELIMITER,
			results_string,
			0 );
		piece(	return_attribute_name,
			FOLDER_DATA_DELIMITER,
			results_string,
			1 );
		*folder_name = strdup( return_folder_name );
		*attribute_name = strdup( return_attribute_name );
	}

} /* additional_drop_down_attribute_populate_folder_attribute() */

LIST *additional_drop_down_attribute_get_user_list(
			char *application_name,
			char *drop_down_folder_name,
			char *login_name )
{
	char sys_string[ 1024 ];
	char input_buffer[ 512 ];
	char folder_name[ 128 ];
	char attribute_name[ 128 ];
	char piece_buffer[ 128 ];
	char where_clause[ 128 ];
	char *from_folder_name = "additional_user_drop_down_attribute";
	char *select = "folder,attribute,prepend_yn";
	LIST *additional_user_drop_down_attribute_list = list_new();
	ADDITIONAL_USER_DROP_DOWN_ATTRIBUTE *
		additional_user_drop_down_attribute;
	FILE *input_pipe;

	sprintf( where_clause,
		 "drop_down_folder = '%s' and login_name = '%s'",
		 drop_down_folder_name,
		 login_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 from_folder_name,
		 where_clause );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	folder_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	attribute_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		additional_user_drop_down_attribute =
			additional_user_drop_down_attribute_new(
				strdup( folder_name ),
				strdup( attribute_name ) );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		additional_user_drop_down_attribute->prepend =
			( *piece_buffer == 'y' );

		list_append_pointer(
			additional_user_drop_down_attribute_list,
			additional_user_drop_down_attribute );
	}

	pclose( input_pipe );
	return additional_user_drop_down_attribute_list;

} /* additional_drop_down_attribute_get_user_list() */

boolean additional_drop_down_exists_attribute(
			LIST *user_list,
			char *attribute_name )
{
	ADDITIONAL_USER_DROP_DOWN_ATTRIBUTE *
		additional_user_drop_down_attribute;

	if ( !list_rewind( user_list ) ) return 0;
	if ( !attribute_name ) return 0;

	do {
		additional_user_drop_down_attribute =
			list_get_pointer( user_list );

		if ( strcasecmp(	additional_user_drop_down_attribute->
						attribute_name,
					attribute_name ) == 0 )
		{
			return 1;
		}
	} while( list_next( user_list ) );
	return 0;


} /* additional_drop_down_exists_attribute() */

void additional_drop_down_attribute_get_select_clause(
			char *select_clause,
			char **first_attribute_name,
			char *application_name,
			char *constant_select,
			char *select_folder_name,
			ADDITIONAL_DROP_DOWN_ATTRIBUTE *
				additional_drop_down_attribute )
{
	LIST *select_list;
	char *select_attribute_name;
	ADDITIONAL_USER_DROP_DOWN_ATTRIBUTE *
		additional_user_drop_down_attribute;
	LIST *user_list;
	char *table_name;
	boolean first_time = 1;

	select_list = list_delimiter_string_to_list( constant_select, ',' );

	if ( !list_length( select_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty constant_select = (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			constant_select );
		exit( 1 );
	}

	/* Select list for the data list */
	/* ----------------------------- */
	table_name =
		get_table_name(
			application_name,
			select_folder_name );

	list_rewind( select_list );
	do {
		select_attribute_name = list_get_pointer( select_list );

		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			select_clause +=
				sprintf(select_clause,
					"," );
		}

		select_clause +=
			sprintf(select_clause,
				"%s.%s",
				table_name,
				select_attribute_name );

	} while( list_next( select_list ) );

	/* The delimiter between the data list and the label list. */
	/* ------------------------------------------------------- */
	select_clause +=
		sprintf(select_clause,
			",'%c'",
			MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER );

	/* The label list */
	/* -------------- */
	user_list = additional_drop_down_attribute->
			additional_user_drop_down_attribute_list;

	/* Prepend addtional_user_drop_down_attribute_list */
	/* ----------------------------------------------- */
	if ( list_rewind( user_list ) )
	{
		do {
			additional_user_drop_down_attribute =
				list_get_pointer(
					user_list );

			if ( !additional_user_drop_down_attribute->prepend )
				continue;

			if ( first_time )
			{
				first_time = 0;
			}
			else
			{
				select_clause +=
					sprintf(select_clause,
						"," );
			}

			if ( first_attribute_name && !(**first_attribute_name) )
			{
				*first_attribute_name =
					additional_user_drop_down_attribute->
						attribute_name;
			}

			table_name =
				get_table_name(
					application_name,
					additional_user_drop_down_attribute->
						folder_name );

			select_clause +=
				sprintf(select_clause,
					"%s.%s",
					table_name,
					additional_user_drop_down_attribute->
						attribute_name );
		} while( list_next( user_list ) );
	} /* if ( list_rewind( user_list ) ) */

	table_name =
		get_table_name(
			application_name,
			select_folder_name );

	/* Select list */
	/* ----------- */
	if ( list_rewind( select_list ) )
	{
		do {
			select_attribute_name = list_get_pointer( select_list );
	
			if ( first_time )
			{
				first_time = 0;
			}
			else
			{
				select_clause +=
					sprintf(select_clause,
						"," );
			}

			if ( first_attribute_name && !(**first_attribute_name) )
			{
				*first_attribute_name = select_attribute_name;
			}

			select_clause +=
				sprintf(select_clause,
					"%s.%s",
					table_name,
					select_attribute_name );
	
		} while( list_next( select_list ) );
	}

	/* Append addtional_user_drop_down_attribute_list */
	/* ---------------------------------------------- */
	if ( list_rewind( user_list ) )
	{
		do {
			additional_user_drop_down_attribute =
				list_get_pointer(
					user_list );

			if ( additional_user_drop_down_attribute->prepend )
				continue;

			if ( first_time )
			{
				first_time = 0;
			}
			else
			{
				select_clause +=
					sprintf(select_clause,
						"," );
			}

			if ( first_attribute_name && !(**first_attribute_name) )
			{
				*first_attribute_name =
					additional_user_drop_down_attribute->
						attribute_name;
			}

			table_name =
				get_table_name(
					application_name,
					additional_user_drop_down_attribute->
						folder_name );

			select_clause +=
				sprintf(select_clause,
					"%s.%s",
					table_name,
					additional_user_drop_down_attribute->
						attribute_name );
		} while( list_next( user_list ) );
	} /* if ( list_rewind( user_list ) ) */

	/* Append default folder/attribute */
	/* ------------------------------- */
	if ( additional_drop_down_attribute->folder_name
	&&   *additional_drop_down_attribute->folder_name
	&&   additional_drop_down_attribute->attribute_name
	&&   *additional_drop_down_attribute->attribute_name
	&&   !additional_drop_down_exists_attribute(
			user_list,
			additional_drop_down_attribute->attribute_name ) )
	{
		if ( first_time )
		{
			first_time = 0;
			*first_attribute_name =
				additional_drop_down_attribute->
					attribute_name;
		}
		else
		{
			select_clause +=
				sprintf(select_clause,
					"," );
		}

		table_name =
			get_table_name(
				application_name,
				additional_drop_down_attribute->
					folder_name );

		select_clause +=
			sprintf(select_clause,
				"%s.%s",
				table_name,
				additional_drop_down_attribute->
					attribute_name );
	}

#ifdef NOT_DEFINED
	/* Append the delimiter */
	/* -------------------- */
	if ( first_time )
	{
		first_time = 0;
	}
	else
	{
		select_clause +=
			sprintf(select_clause,
				"," );
	}
#endif

} /* additional_drop_down_attribute_get_select_clause() */

