/* ---------------------------------------------	*/
/* get_folder_data.c					*/
/* ---------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "folder.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "name_arg.h"
#include "appaserver.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void update_folder_row_access_count(
				char *application_name,
				char *folder_name,
				long row_access_count );

void setup_arg(		NAME_ARG *arg, int argc, char **argv );

void fetch_parameters(	char **application_name,
			char **folder_name,
			char **select,
			char **where_clause,
			char **order_clause,
			char **group_clause,
			char **quick_yes_no,
			char **dbms,
			char **maxrows,
			NAME_ARG *arg );

int main( int argc, char **argv )
{
	char *application_name;
	char *folder_name;
	char *select;
	char *order_clause;
	char *group_clause;
	char *quick_yes_no;
	char *dbms;
	char *maxrows = {0};
	long maxrows_long = 0L;
	char *where_clause;
	char sys_string[ 65536 ];
	char order_by_clause[ 4096 ];
	char group_by_clause[ 4096 ];
	char where_clause_escaped[ 65536 ];
	LIST *attribute_name_list, *primary_name_list;
	char *table_name;
	FOLDER *folder;
	char sql_executable[ 128 ];
        NAME_ARG *arg = init_arg( argv[ 0 ] );
	long row_access_count = 0L;
	char input_buffer[ 65536 ];
	FILE *input_pipe;

/*
output_starting_argv_stderr( argc, argv );
*/

	setup_arg( arg, argc, argv );

	fetch_parameters(	&application_name,
				&folder_name,
				&select,
				&where_clause,
				&order_clause,
				&group_clause,
				&quick_yes_no,
				&dbms,
				&maxrows,
				arg );

	if ( !folder_name || !*folder_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty folder_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( maxrows && *maxrows ) maxrows_long = atol( maxrows );

	if ( !*dbms ) dbms = appaserver_parameter_file_get_dbms();

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	if( strcmp( select, "primary" ) == 0 )
	{
		folder = folder_new_folder(
					application_name,
					BOGUS_SESSION,
					folder_name );

		folder->attribute_list =
		attribute_get_attribute_list(
		application_name,
		folder_name,
		(char *)0 /* attribute_name */,
		(LIST *)0 /* mto1_isa_related_folder_list */,
		(char *)0 /* role_name */ );

		primary_name_list = 
			folder_get_primary_attribute_name_list(
				folder->attribute_list );

		if ( !list_length( primary_name_list ) )
		{
			char msg[ 1024 ];
			sprintf( msg, 
"ERROR %s/%s(): got blank primary attribute name list for folder = (%s)",
				 __FILE__,
				 __FUNCTION__,
				 folder->folder_name );
			appaserver_output_error_message(
				application_name, msg, (char *)0 );
			exit( 1 );
		}

		select = 
			list2comma_string( primary_name_list );
	}
	else
	if( strcmp( select, "all" ) == 0 )
	{
		folder = folder_new_folder(
					application_name,
					BOGUS_SESSION,
					folder_name );

		folder->attribute_list =
			attribute_get_attribute_list(
				application_name,
				folder_name,
				(char *)0 /* attribute_name */,
				(LIST *)0 /* mto1_isa_related_folder_list */,
				(char *)0 /* role_name */ );

		attribute_name_list = 
			folder_get_attribute_name_list(
					folder->attribute_list );

		if ( !list_length( attribute_name_list ) )
		{
			char msg[ 1024 ];
			sprintf( msg, 
	"ERROR %s/%s(): got blank attribute name list for folder = (%s)",
				 __FILE__,
				 __FUNCTION__,
				 folder->folder_name );
			appaserver_output_error_message(
				application_name, msg, (char *)0 );
			exit( 1 );
		}

		select =
			list2comma_string( attribute_name_list );
	}
	else
	if( strcmp( select, "count" ) == 0 )
	{
		select = "count(*)";
		order_clause = "none";
	}

	/* Set final where clause */
	/* ---------------------- */
	strcpy( where_clause_escaped, where_clause );
	escape_dollar_sign( where_clause_escaped );

	if ( !*where_clause_escaped )
		strcpy( where_clause_escaped, "1 = 1" );

	/* Set group by clause */
	/* ------------------- */
	if ( *group_clause )
	{
		sprintf(group_by_clause, 
		 	"group by %s",
		 	group_clause );
	}
	else
	{
		*group_by_clause = '\0';
	}

	/* Set order by clause */
	/* ------------------- */
	if ( strcmp( order_clause, "select" ) == 0 )
	{
		sprintf(order_by_clause, 
		 	"order by %s",
		 	select );
	}
	else
	if ( *order_clause && strcmp( order_clause, "none" ) != 0 )
	{
		sprintf(order_by_clause, 
		 	"order by %s",
		 	order_clause );
	}
	else
	{
		*order_by_clause = '\0';
	}

	if ( strcmp( quick_yes_no, "yes" ) == 0 )
	{
		sprintf(	sql_executable,
				"sql_quick.e '%c' %s",
				FOLDER_DATA_DELIMITER,
				dbms );
	}
	else
	{
		sprintf(	sql_executable,
				"sql.e '%c' %s",
				FOLDER_DATA_DELIMITER,
				dbms );
	}

	table_name = get_multiple_table_names( application_name, folder_name );

	if ( !*table_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty table_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !select )
	{
		char msg[ 1024 ];
		sprintf( msg, 
			 "ERROR: %s/%s(%s,%s) has no select clause\n",
			 __FILE__,
			 __FUNCTION__,
			 application_name,
			 folder_name );
		appaserver_output_error_message(
			application_name, msg, (char *)0 );
		exit( 1 );
	}

	sprintf(sys_string,
	 	"echo \"select %s				 "
		" 	from %s					 "
		" 	where %s				 "
		" 	%s					 "
		"	%s;\"					|"
	 	"%s 						|"
	 	"cat 						 ",
	 	select, 
		table_name,
		where_clause_escaped,
		group_by_clause,
		order_by_clause,
		sql_executable );

/*
fprintf( stderr, "%s\n", sys_string );
*/

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		row_access_count++;

		if ( maxrows_long
		&&   row_access_count > maxrows_long )
		{
			char msg[ 65536 ];

			printf(
"Warning: Output truncated. Max rows of %ld exceeded\n", maxrows_long );

			sprintf( msg,
"Warning in %s/%s()/%d: Output truncated. Max rows of %ld exceeded in (from %s where %s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 maxrows_long,
				 table_name,
				 where_clause_escaped );
			m2( application_name, msg );

			pclose( input_pipe );
			exit( 0 );
		}

		printf( "%s\n", input_buffer );
	}

	pclose( input_pipe );

	update_folder_row_access_count(	application_name,
					folder_name,
					row_access_count );

	exit( 0 );
} /* main() */

void fetch_parameters(	char **application_name,
			char **folder_name,
			char **select,
			char **where_clause,
			char **order_clause,
			char **group_clause,
			char **quick_yes_no,
			char **dbms,
			char **maxrows,
			NAME_ARG *arg )
{
	*application_name = fetch_arg( arg, "application" );
	*folder_name = fetch_arg( arg, "folder" );
	*select = fetch_arg( arg, "select" );
	*where_clause = fetch_arg( arg, "where_clause" );
	*order_clause = fetch_arg( arg, "order_clause" );
	*group_clause = fetch_arg( arg, "group_clause" );
	*quick_yes_no = fetch_arg( arg, "quick" );
	*dbms = fetch_arg( arg, "dbms" );
	*maxrows = fetch_arg( arg, "maxrows" );

} /* fetch_parameters() */

void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

	set_comment( arg, "Valid options for select are" );
	set_comment( arg, "\"primary\",\"all\",\"count\",column_comma_list" );

	set_comment( arg, "" );

	set_comment( arg, "Valid options for order_clause are" );
	set_comment( arg,
		"\"select\",\"none\",column_comma_list [descending]" );

        ticket = add_valid_option( arg, "application" );

        ticket = add_valid_option( arg, "folder" );

        ticket = add_valid_option( arg, "select" );
        set_default_value( arg, ticket, "" );

        ticket = add_valid_option( arg, "where_clause" );
        set_default_value( arg, ticket, "1=1" );

        ticket = add_valid_option( arg, "quick" );
	add_valid_value( arg, ticket, "yes" );
	add_valid_value( arg, ticket, "no" );
        set_default_value( arg, ticket, "no" );

        ticket = add_valid_option( arg, "order_clause" );
        set_default_value( arg, ticket, "select" );

        ticket = add_valid_option( arg, "group_clause" );
        set_default_value( arg, ticket, "" );

        ticket = add_valid_option( arg, "dbms" );
        set_default_value( arg, ticket, "" );

        ticket = add_valid_option( arg, "maxrows" );
        set_default_value( arg, ticket, "" );

        ins_all( arg, argc, argv );

} /* setup_arg() */

void update_folder_row_access_count(
				char *application_name,
				char *folder_name,
				long row_access_count )
{
	char sys_string[ 1024 ];
	char *table_name;
	char first_folder_name[ 128 ];

	table_name = get_table_name( application_name, "folder" );

	piece( first_folder_name, ',', folder_name, 0 );

	sprintf( sys_string,
"echo \"update %s set row_access_count = row_access_count + %ld where folder = '%s';\" | sql.e &",
		 table_name,
		 row_access_count,
		 first_folder_name );

	system( sys_string );

} /* update_folder_row_access_count() */

