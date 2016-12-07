/* families_list.c					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "additional_drop_down_attribute.h"
#include "appaserver_library.h"
#include "dictionary.h"
#include "folder.h"
#include "timlib.h"
#include "query.h"
#include "attribute.h"
#include "appaserver_parameter_file.h"

/* Prototypes */
/* ---------- */
LIST *get_fish_families_record_list(
			char *application_name,
			char *login_name,
			char *census_date,
			char *interview_location,
			char *interview_number );

boolean get_dictionary_variables(
			char **login_name,
			char **census_date,
			char **interview_location,
			char **interview_number,
			DICTIONARY *post_dictionary );

void get_catches_family_in_clause(
			char *catches_family_in_clause,
			char *application_name,
			DICTIONARY *post_dictionary );

char *get_species_sys_string(
			char *application_name,
			char *login_name,
			DICTIONARY *post_dictionary );

char *get_species_sys_string(
			char *application_name,
			char *login_name,
			DICTIONARY *post_dictionary );

char *get_fish_families_sys_string(
			char *application_name,
			char *login_name,
			DICTIONARY *post_dictionary );

char *get_families_sys_string(
			char *application_name,
			char *login_name,
			DICTIONARY *post_dictionary );

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *sys_string;
	char *folder_name;
	DICTIONARY *post_dictionary = {0};
	int results;

	output_starting_argv_stderr( argc, argv );

	if ( argc < 4 )
	{
		fprintf( stderr,
		"Usage: %s application login_name folder_name [dictionary]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	login_name = argv[ 2 ];
	folder_name = argv[ 3 ];

	if ( argc == 5 && strcmp( argv[ 4 ], "$dictionary" ) != 0 )
	{
		post_dictionary = 
			dictionary_string2dictionary( argv[ 4 ] );
	}

	if ( strcmp( folder_name, "fish_families" ) == 0 )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: fish_families is not longer in use.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
/*
		sys_string = get_fish_families_sys_string(
					application_name,
					login_name,
					post_dictionary );
*/
	}
	else
	if ( strcmp( folder_name, "families" ) == 0 )
	{
		sys_string = get_families_sys_string(
					application_name,
					login_name,
					post_dictionary );
	}
	else
	{
		fprintf(	stderr,
				"Error %s/%s()/%d: invalid folder_name= (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				folder_name );
		exit( 1 );
	}

	results = system( sys_string );

	return 0;
} /* main() */

char *get_fish_families_sys_string(	char *application_name,
					char *login_name,
					DICTIONARY *post_dictionary )
{
	char sys_string[ 1024 ];
	char *fish_families_table_name;
	char *families_table_name;
	char where_clause[ 1024 ];
	ADDITIONAL_DROP_DOWN_ATTRIBUTE *additional_drop_down_attribute;
	char select_clause[ 1024 ];
	char *constant_select = "family";
	char *first_attribute_name = "";
	char sort_clause[ 16 ];
	char *one2m_folder_name = {0};
	char catches_family_in_clause[ 1024 ];

	if ( post_dictionary )
	{
		one2m_folder_name =
			dictionary_get(
				post_dictionary,
				ONE2M_FOLDER_NAME_FOR_PROCESS );
	}

	if ( one2m_folder_name
	&&   strcmp(	one2m_folder_name,
			"catch_measurements_family" ) == 0 )
	{
		get_catches_family_in_clause(
			catches_family_in_clause,
			application_name,
			post_dictionary );
	}
	else
	{
		strcpy( catches_family_in_clause, "1 = 1" );
	}

	additional_drop_down_attribute =
		additional_drop_down_attribute_new(
			application_name,
			login_name,
			"fish_families" );

	additional_drop_down_attribute_get_select_clause(
			select_clause,
			&first_attribute_name,
			additional_drop_down_attribute->
				application_name,
			constant_select,
			additional_drop_down_attribute->
				drop_down_folder_name,
			additional_drop_down_attribute );

	if ( strcmp( first_attribute_name, "florida_state_code" ) == 0 )
		strcpy( sort_clause, "sort -n" );
	else
		strcpy( sort_clause, "sort" );

	families_table_name =
		get_table_name(	application_name,
				"families" );;

	fish_families_table_name =
		get_table_name(	application_name,
				"fish_families" );;

	sprintf( where_clause,
		 "%s.family = %s.family and	"
		 "%s				",
		 families_table_name,
		 fish_families_table_name,
		 catches_family_in_clause );

	sprintf( sys_string,
"echo \"select %s from %s,%s where %s;\" | sql.e | %s | sed 's/\\^|\\^/|/'",
		 select_clause,
		 families_table_name,
		 fish_families_table_name,
		 where_clause,
		 sort_clause );

	return strdup( sys_string );

} /* get_fish_families_sys_string() */

char *get_families_sys_string(		char *application_name,
					char *login_name,
					DICTIONARY *post_dictionary )
{
	char sys_string[ 1024 ];
	char *families_table_name;
	ADDITIONAL_DROP_DOWN_ATTRIBUTE *additional_drop_down_attribute;
	char select_clause[ 1024 ];
	char *constant_select = "family";
	char *first_attribute_name = "";
	char sort_clause[ 16 ];
	char *where_clause = "1 = 1";

	if ( post_dictionary )
	{
		FOLDER *folder;
		QUERY *query;

		folder = folder_new_folder(
					application_name,
					BOGUS_SESSION,
					"families" );

		folder->attribute_list =
			attribute_get_attribute_list(
				folder->application_name,
				folder->folder_name,
				(char *)0 /* attribute_name */,
				(LIST *)0 /* mto1_isa_related_folder_list */,
				(char *)0 /* role_name */ );

		query =	query_new(
				application_name,
				login_name,
				folder->folder_name,
				(LIST *)0 /* attribute_list */,
				post_dictionary,
				(DICTIONARY *)0 /* sort_dictionary */,
				(ROLE *)0 /* role */,
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

		where_clause =
			query->query_output->where_clause;
	}

	additional_drop_down_attribute =
		additional_drop_down_attribute_new(
			application_name,
			login_name,
			"families" );

	additional_drop_down_attribute_get_select_clause(
			select_clause,
			&first_attribute_name,
			additional_drop_down_attribute->
				application_name,
			constant_select,
			additional_drop_down_attribute->
				drop_down_folder_name,
			additional_drop_down_attribute );

	strcpy( sort_clause, "sort" );

	families_table_name =
		get_table_name(	application_name,
				"families" );;

	sprintf( sys_string,
"echo \"select %s from %s where %s;\" | sql.e | %s | sed 's/\\^|\\^/|/'",
		 select_clause,
		 families_table_name,
		 where_clause,
		 sort_clause );

	return strdup( sys_string );

} /* get_families_sys_string() */

boolean get_dictionary_variables(
			char **login_name,
			char **census_date,
			char **interview_location,
			char **interview_number,
			DICTIONARY *post_dictionary )
{
	if ( ! ( *login_name =
			dictionary_get(	post_dictionary,
					"login_name_0" ) ) )
	{
		return 0;
	}
	if ( ! ( *census_date =
			dictionary_get(	post_dictionary,
					"census_date_0" ) ) )
	{
		return 0;
	}
	if ( ! ( *interview_location =
			dictionary_get(	post_dictionary,
					"interview_location_0" ) ) )
	{
		return 0;
	}
	if ( ! ( *interview_number =
			dictionary_get(	post_dictionary,
					"interview_number_0" ) ) )
	{
		return 0;
	}

	return 1;
} /* get_dictionary_variables() */

LIST *get_fish_families_record_list(
			char *application_name,
			char *login_name,
			char *census_date,
			char *interview_location,
			char *interview_number )
{
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];
	char *folder_name = "catches_family";
	char *select_clause = "family";

	sprintf( where_clause,
		 "login_name = '%s' and				"
		 "census_date = '%s' and			"
		 "interview_location = '%s' and			"
		 "interview_number = %s				",
		 login_name,
		 census_date,
		 interview_location,
		 interview_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select_clause,
		 folder_name,
		 where_clause );

	return pipe2list( sys_string );

} /* get_fish_families_record_list() */

void get_catches_family_in_clause(
			char *catches_family_in_clause,
			char *application_name,
			DICTIONARY *post_dictionary )
{
	char *login_name;
	char *census_date;
	char *interview_location;
	char *interview_number;
	LIST *fish_families_record_list;
	char *record;
	char *table_name;
	int first_time;

	strcpy( catches_family_in_clause, "1 = 1" );

	if ( !get_dictionary_variables(
			&login_name,
			&census_date,
			&interview_location,
			&interview_number,
			post_dictionary ) )
	{
		return;
	}

	fish_families_record_list =
		get_fish_families_record_list(
			application_name,
			login_name,
			census_date,
			interview_location,
			interview_number );

	if ( !fish_families_record_list
	||   !list_length( fish_families_record_list ) )
	{
		strcpy( catches_family_in_clause, "1 = 2" );
		return;
	}

	table_name = get_table_name( application_name, "fish_families" );

	catches_family_in_clause +=
		sprintf( catches_family_in_clause,
			 "%s.family in (",
			 table_name );

	first_time = 1;
	list_rewind( fish_families_record_list );

	do {
		record = list_get_pointer( fish_families_record_list );

		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			catches_family_in_clause +=
				sprintf( catches_family_in_clause,
				 	 "," );
		}

		catches_family_in_clause +=
			sprintf( catches_family_in_clause,
			 	 "'%s'",
				 record );

	} while( list_next( fish_families_record_list ) );

	catches_family_in_clause +=
		sprintf( catches_family_in_clause,
			 ")" );

} /* get_catches_family_in_clause() */

