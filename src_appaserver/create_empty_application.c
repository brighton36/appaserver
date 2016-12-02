/* --------------------------------------------------- 	*/
/* src_appaserver/create_empty_application.c	       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "application.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "folder.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "session.h"

/* Constants */
/* --------- */
#define DELETE_OK			1
#define SYSTEM_ADMINISTRATION_ROLE	"system"
#define SUPERVISOR_ROLE			"supervisor"
#define DATAENTRY_ROLE			"dataentry"
#define PROFILE_FILENAME		"/etc/profile"
#define RESETTED_PASSWORD		"changeit"

/* Prototypes */
/* ---------- */
char *get_error_filename(		char *appaserver_error_directory,
					char *destination_application );

char *get_post_login_sed_executable(
					char *destination_application,
					char create_database_yn );

void drop_application_tables(		char *destination_application,
					char really_yn );
boolean delete_existing_application(
					char *destination_application,
					char create_database_yn,
					char *appaserver_error_directory,
					char *document_root_directory,
					char really_yn );

boolean create_empty_application(	char *current_application,
					char *destination_application,
					char create_database_yn,
					char *session,
					char *login_name,
					char *role_name,
					char *database_string,
					char *appaserver_error_directory,
					char *new_application_title,
					char *document_root_directory,
					char *appaserver_data_directory,
					char *appaserver_home_directory,
					char really_yn );

void make_appaserver_error_file(	char *destination_application,
					char *appaserver_error_directory,
					char really_yn );

void remove_appaserver_error_file(	char *destination_application,
					char *appaserver_error_directory,
					char really_yn );

void delete_appaserver_user_rows(	char *destination_application,
					char *database_string,
					char create_database_yn,
					char really_yn );

void insert_appaserver_user_row(	char *destination_application,
					char *database_string,
					char create_database_yn,
					char *login_name,
					char really_yn );

void delete_process_non_appaserver_rows(
					char *destination_application,
					char *database_string,
					char create_database_yn,
					char really_yn );

void delete_attribute_non_appaserver_rows(
					char *destination_application,
					char *current_application,
					char *database_string,
					char *login_name,
					char *role_name,
					char create_database_yn,
					char really_yn );

void delete_folder_non_appaserver_rows(	char *destination_application,
					char *current_application,
					char *database_string,
					char *login_name,
					char *role_name,
					char create_database_yn,
					char really_yn );

void delete_non_appaserver_rows(	char *destination_application,
					char *current_application,
					char *database_string,
					char *login_name,
					char *role_name,
					char create_database_yn,
					char really_yn );

void insert_appaserver_rows(		char *destination_application,
					char *current_application,
					char *appaserver_data_directory,
					char *session,
					char *login_name,
					char *database_string,
					char create_database_yn,
					char really_yn );

void create_system_tables(		char *destination_application,
					char *current_application,
					char *appaserver_data_directory,
					char *session,
					char *login_name,
					char *role_name,
					char *database_string,
					char create_database_yn,
					char really_yn );

/*
void integrate_dynarch_menu(		char *destination_application,
					char *current_application,
					char *document_root_directory,
					char *appaserver_data_directory,
					char really_yn );
*/

void fix_index_dot_php(			char *destination_application,
					char *document_root_directory,
					char *new_application_title,
					char create_database_yn,
					char really_yn );

void populate_document_root_directory(	char *destination_application,
					char *document_root_directory,
					char *appaserver_home_directory,
					char really_yn );

void make_document_root_directory(	char *destination_application,
					char *document_root_directory,
					char *appaserver_home_directory,
					char really_yn );

void make_appaserver_home_directory(	char *destination_application,
					char *appaserver_home_directory,
					char really_yn );

void remove_home_directory(		char *destination_application,
					char *document_root_directory,
					char really_yn );

void remove_document_root_links(	char *destination_application,
					char *document_root_directory,
					char really_yn );

void remove_dynarch_menu(		char *destination_application,
					char really_yn );

void insert_application_row(		char *current_application,
					char *destination_application,
					char create_database_yn,
					char *session,
					char *login_name,
					char *role_name,
					char *database_string,
					char *appaserver_data_directory,
					char really_yn );

void update_application_row(		char *destination_application,
					char create_database_yn,
					char *database_string,
					char *new_application_title,
					char really_yn );

void create_database(			char *destination_application,
					char really_yn );

void drop_database(			char *destination_application,
					char really_yn );

void set_database_environment(		char *destination_application,
					char create_database_yn,
					char really_yn );

void restore_database_environment(	char *database_string,
					char create_database_yn,
					char really_yn );

void create_application_table(		char *current_application,
					char *destination_application,
					char create_database_yn,
					char *session,
					char *login_name,
					char *role_name,
					char *database_string,
					char *appaserver_data_directory,
					char really_yn );

void get_all_environment_variables(	char **appaserver_error_directory,
					char **document_root_directory,
					char **appaserver_data_directory,
					char **appaserver_home_directory );

int main( int argc, char **argv )
{
	char *current_application;
	char *destination_application;
	char *new_application_title;
	char *session;
	char *login_name;
	char delete_application_yn;
	char really_yn;
	char create_database_yn;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *role_name;
	char *process_name;
	char *database_string = {0};
	char buffer[ 128 ];
	char *appaserver_error_directory = {0};
	char *document_root_directory = {0};
	char *appaserver_data_directory = {0};
	char *appaserver_home_directory = {0};

	output_starting_argv_stderr( argc, argv );

	if ( argc != 11 )
	{
		fprintf(stderr,
"Usage: %s current_application session login_name role process destination_application new_application_title create_database_yn delete_application_yn really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	current_application = argv[ 1 ];
	session = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	process_name = argv[ 5 ];
	destination_application = argv[ 6 ];
	new_application_title = argv[ 7 ];
	create_database_yn = *argv[ 8 ];
	delete_application_yn = *argv[ 9 ];
	really_yn = *argv[ 10 ];

	if ( !*new_application_title
	||   strcmp( new_application_title, "new_application_title" ) == 0 )
	{
		new_application_title = "Title Omitted";
	}

	if ( timlib_parse_database_string(	&database_string,
						current_application ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				current_application );

	appaserver_parameter_file =
		appaserver_parameter_default_file_new();

	if ( !session_access(	current_application,
				session,
				login_name ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: no session=%s for user=%s found.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 session,
			 login_name );
		exit( 1 ); 
	}

	document = document_new( "", current_application );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				current_application ),
			0 /* not with_dynarch_menu */ );

	document_output_body( 	current_application,
				document->onload_control_string );

	printf( "<h1>%s: %s</h1>\n",
		format_initial_capital( buffer, process_name ),
		new_application_title );
	fflush( stdout );

	get_all_environment_variables(	&appaserver_error_directory,
					&document_root_directory,
					&appaserver_data_directory,
					&appaserver_home_directory );

	if ( delete_application_yn != 'y'
	&&   appaserver_library_application_exists(
					destination_application,
					appaserver_error_directory ) )
	{
		printf(
		"<h3>Error: application name of %s is not allowed.</h3>\n",
			destination_application );
		document_close();
		exit( 1 );
	}

	if ( !DELETE_OK && delete_application_yn == 'y' )
	{
			printf(
"<h3>Error: the delete function is turned off for security. Set DELETE_OK to 1.</h3>\n" );
			document_close();
			exit( 1 );
	}
	else
	if ( delete_application_yn == 'y' )
	{
		if ( !appaserver_library_application_exists(
					destination_application,
					appaserver_error_directory )
		||   !delete_existing_application(
					destination_application,
					create_database_yn,
					appaserver_error_directory,
					document_root_directory,
					really_yn ) )
		{
			printf(
			"<h3>Error: application %s doesn't exist.</h3>\n",
				destination_application );
			document_close();
			exit( 0 );
		}
	}
	else
	{
		if ( !create_empty_application(
					current_application,
					destination_application,
					create_database_yn,
					session,
					login_name,
					role_name,
					database_string,
					appaserver_error_directory,
					new_application_title,
					document_root_directory,
					appaserver_data_directory,
					appaserver_home_directory,
					really_yn ) )
		{
			printf(
			"<h3>Error: application %s already exists.</h3>\n",
				destination_application );
			document_close();
			exit( 1 );
		}
	}

	if ( really_yn == 'y' )
	{
		process_increment_execution_count(
					current_application,
					process_name,
					appaserver_parameter_file_get_dbms() );
	}

	document_close();

	exit( 0 );

} /* main() */

void get_all_environment_variables(	char **appaserver_error_directory,
					char **document_root_directory,
					char **appaserver_data_directory,
					char **appaserver_home_directory )
{
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	appaserver_parameter_file =
		appaserver_parameter_default_file_new();

	*appaserver_error_directory =
		appaserver_parameter_file->
			appaserver_error_directory;

	*document_root_directory =
		appaserver_parameter_file->
			document_root;

	*appaserver_data_directory =
		appaserver_parameter_file->
			appaserver_data_directory;

	*appaserver_home_directory =
		appaserver_parameter_file->
			appaserver_mount_point;

} /* get_all_environment_variables() */

void restore_database_environment(	char *database_string,
					char create_database_yn,
					char really_yn )
{
	if ( create_database_yn == 'y' )
	{
		if ( database_string && *database_string )
		{
			environ_set_environment(
				APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
				database_string );

			if ( really_yn != 'y' )
			{
				printf( "<p>Restoring environment: %s\n",
					database_string );
			}
		}
		else
		{
			environ_set_environment(
				APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
				"" );

			if ( really_yn != 'y' )
			{
				printf( "<p>Restoring environment: %s\n",
					"" );
			}
		}
	}
} /* restore_database_environment() */

void set_database_environment(	char *destination_application,
				char create_database_yn,
				char really_yn )
{
	if ( create_database_yn == 'y' )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			destination_application );

		if ( really_yn != 'y' )
		{
			printf( "<p>Setting environment: %s\n",
				destination_application );
		}
	}

} /* set_database_environment() */

void create_database(		char *destination_application,
				char really_yn )
{
	char sys_string[ 1024 ];

	if ( really_yn != 'y' )
	{
		sprintf( sys_string,
		 	"echo \"create database %s;\"	|"
			"html_paragraph_wrapper.e",
		 	destination_application );
	}
	else
	{
		sprintf( sys_string,
		 	"echo \"create database %s;\"	|"
			"sql.e 2>&1			|"
			"html_paragraph_wrapper.e",
		 	destination_application );
	}
	fflush( stdout );
	system( sys_string );
	fflush( stdout );

} /* create_database() */

void create_application_table(		char *current_application,
					char *destination_application,
					char create_database_yn,
					char *session,
					char *login_name,
					char *role_name,
					char *database_string,
					char *appaserver_data_directory,
					char really_yn )
{
	char sys_string[ 1024 ];
	char output_string[ 128 ];

	if ( really_yn == 'y' )
		strcpy( output_string, ">/dev/null" );
	else
		*output_string = '\0';

	if ( create_database_yn != 'y' )
	{
		sprintf( sys_string,
"create_table %s %c %s %s \"%s\" %s application %c mysql nohtml %s",
		 	current_application,
		 	'n' /* build_shell_script_yn */,
			session,
			login_name,
		 	role_name,
		 	destination_application,
		 	really_yn,
			output_string );
		fflush( stdout );
		system( sys_string );
		fflush( stdout );
	}
	else
	{
		sprintf( sys_string,
"create_table %s %c %s %s \"%s\" %s application %c mysql nohtml %s",
			current_application,
		 	'y' /* build_shell_script_yn */,
			session,
			login_name,
		 	role_name,
		 	destination_application,
		 	'n' /* really_yn */,
			output_string );
		fflush( stdout );
		system( sys_string );
		fflush( stdout );

		set_database_environment(
					destination_application,
					create_database_yn,
					really_yn );

		if ( really_yn == 'y' )
		{
			sprintf( sys_string,
			 	"%s/create_table.sh %s",
			 	appaserver_data_directory,
			 	destination_application );
		}
		else
		{
			sprintf( sys_string,
"echo \"Will execute %s/create_table.sh %s\" | html_paragraph_wrapper.e",
			 	appaserver_data_directory,
				destination_application );
		}
		fflush( stdout );
		system( sys_string );
		fflush( stdout );

		restore_database_environment(
					database_string,
					create_database_yn,
					really_yn );
	}

} /* create_application_table() */

void insert_application_row(		char *current_application,
					char *destination_application,
					char create_database_yn,
					char *session,
					char *login_name,
					char *role_name,
					char *database_string,
					char *appaserver_data_directory,
					char really_yn )
{
	char sys_string[ 1024 ];
	char output_string[ 128 ];

	if ( really_yn == 'y' )
		strcpy( output_string, ">/dev/null" );
	else
		*output_string = '\0';


	if ( create_database_yn != 'y' )
	{
		sprintf( sys_string,
"clone_folder %s n %s %s \"%s\" %s application application %s %s nohtml n %c mysql %s",
		 	current_application,
			session,
			login_name,
		 	role_name,
		 	destination_application,
		 	current_application,
		 	destination_application,
		 	really_yn,
			output_string );
		fflush( stdout );
		system( sys_string );
		fflush( stdout );
	}
	else
	{
		sprintf( sys_string,
"clone_folder %s n %s %s \"%s\" %s application application %s %s nohtml n n mysql y %s",
		 	current_application,
			session,
			login_name,
		 	role_name,
		 	destination_application,
		 	current_application,
		 	destination_application,
			output_string );
		fflush( stdout );
		system( sys_string );
		fflush( stdout );

		set_database_environment(
					destination_application,
					create_database_yn,
					really_yn );

		if ( really_yn == 'y' )
		{
			sprintf( sys_string,
"zcat %s/insert_%s_application.sql.gz | sql.e",
			 	appaserver_data_directory,
			 	destination_application );
		}
		else
		{
			sprintf( sys_string,
"zcat %s/insert_%s_application.sql.gz | html_paragraph_wrapper.e",
			 	appaserver_data_directory,
			 	destination_application );
		}
		fflush( stdout );
		system( sys_string );
		fflush( stdout );

		restore_database_environment(
					database_string,
					create_database_yn,
					really_yn );
	}

} /* insert_application_row() */

void update_application_row(		char *destination_application,
					char create_database_yn,
					char *database_string,
					char *new_application_title,
					char really_yn )
{
	char sys_string[ 1024 ];
	char *destination_process;
	char buffer[ 128 ];

	timlib_strcpy( buffer, new_application_title, 128 );

	if ( really_yn == 'y' )
	{
		destination_process = "sql.e";
	}
	else
	{
		destination_process = "html_paragraph_wrapper.e";
	}

	sprintf( sys_string,
"echo \"update %s_application set application_title = '%s', relative_source_directory = 'src_%s' where application = '%s';\" | %s",
	 	destination_application,
		escape_single_quotes( buffer ),
	 	destination_application,
	 	destination_application,
		destination_process );

	if ( create_database_yn != 'y' )
	{
		fflush( stdout );
		system( sys_string );
		fflush( stdout );
	}
	else
	{
		set_database_environment(
					destination_application,
					create_database_yn,
					really_yn );

		fflush( stdout );
		system( sys_string );
		fflush( stdout );

		restore_database_environment(
					database_string,
					create_database_yn,
					really_yn );

	}

	if ( create_database_yn != 'y' )
	{
		sprintf( sys_string,
"echo \"update %s_application set only_one_primary_yn = 'n' where application = '%s';\" | %s",
	 	destination_application,
	 	destination_application,
		destination_process );

		fflush( stdout );
		system( sys_string );
		fflush( stdout );
	}
	else
	{
		set_database_environment(
					destination_application,
					create_database_yn,
					really_yn );

		sprintf( sys_string,
"echo \"update %s_application set only_one_primary_yn = 'y' where application = '%s';\" | %s",
	 	destination_application,
	 	destination_application,
		destination_process );

		fflush( stdout );
		system( sys_string );
		fflush( stdout );

		restore_database_environment(
					database_string,
					create_database_yn,
					really_yn );
	}

} /* update_application_row() */

void make_document_root_directory(	char *destination_application,
					char *document_root_directory,
					char *appaserver_home_directory,
					char really_yn )
{
	char sys_string[ 1024 ];

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	"mkdir %s/%s && chmod g+rwxs %s/%s",
		 	document_root_directory,
		 	destination_application,
		 	document_root_directory,
		 	destination_application );
	}
	else
	{
		sprintf( sys_string,
	"echo \"mkdir %s/%s && chmod g+rwxs %s/%s\" | html_paragraph_wrapper.e",
		 	document_root_directory,
		 	destination_application,
		 	document_root_directory,
		 	destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	"mkdir %s/%s/%s && chmod g+rwxs %s/%s/%s",
		 	document_root_directory,
		 	destination_application,
			"appaserver",
		 	document_root_directory,
		 	destination_application,
			"appaserver" );
	}
	else
	{
		sprintf( sys_string,
"echo \"mkdir %s/%s/%s && chmod g+rwxs %s/%s/%s\" | html_paragraph_wrapper.e",
		 	document_root_directory,
		 	destination_application,
			"appaserver",
		 	document_root_directory,
		 	destination_application,
			"appaserver" );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	"ln -s %s/%s/src_%s %s/%s",
		 	appaserver_home_directory,
		 	destination_application,
			destination_application,
		 	document_root_directory,
		 	destination_application );
	}
	else
	{
		sprintf( sys_string,
"echo \"ln -l %s/%s/src_%s %s/%s\" | html_paragraph_wrapper.e",
		 	appaserver_home_directory,
		 	destination_application,
		 	destination_application,
		 	document_root_directory,
		 	destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

} /* make_document_root_directory() */

void make_appaserver_home_directory(	char *destination_application,
					char *appaserver_home_directory,
					char really_yn )
{
	char sys_string[ 1024 ];

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	"mkdir %s/src_%s && chmod g+rwxs %s/src_%s",
		 	appaserver_home_directory,
		 	destination_application,
		 	appaserver_home_directory,
		 	destination_application );
	}
	else
	{
		sprintf( sys_string,
"echo \"mkdir %s/src_%s && chmod g+rwxs %s/src_%s\" | html_paragraph_wrapper.e",
		 	appaserver_home_directory,
		 	destination_application,
		 	appaserver_home_directory,
		 	destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

} /* make_appaserver_home_directory() */

void populate_document_root_directory(	char *destination_application,
					char *document_root_directory,
					char *appaserver_home_directory,
					char really_yn )
{
	char sys_string[ 1024 ];

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 "umask 0002 && cp -r %s/%s/* %s/%s && chmod g+w %s/%s/*",
		 	appaserver_home_directory,
		 	"template",
		 	document_root_directory,
		 	destination_application,
		 	document_root_directory,
		 	destination_application );
	}
	else
	{
		sprintf( sys_string,
"echo 'umask 0002 && cp -r %s/%s/* %s/%s && chmod g+w %s/%s/*' | html_paragraph_wrapper.e",
		 	appaserver_home_directory,
		 	"template",
		 	document_root_directory,
		 	destination_application,
		 	document_root_directory,
		 	destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

#ifdef NOT_DEFINED
	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	"rm -f %s/%s/*_[1-9][0-9]*",
		 	document_root_directory,
		 	destination_application );
	}
	else
	{
		sprintf( sys_string,
	"echo 'rm -f %s/%s/*_[1-9][0-9]*' | html_paragraph_wrapper.e",
		 	document_root_directory,
		 	destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

/*
	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	"rm -f %s/%s/skin*.css",
		 	appaserver_home_directory,
		 	destination_application );
	}
	else
	{
		sprintf( sys_string,
"echo 'rm -f %s/%s/skin*.css' | html_paragraph_wrapper.e",
		 	appaserver_home_directory,
		 	destination_application );
	}
*/

	fflush( stdout );
	system( sys_string );
	fflush( stdout );
#endif

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	"umask 0002 && ln %s/src_%s/*.pdf %s/src_%s/",
		 	appaserver_home_directory,
		 	"template",
		 	appaserver_home_directory,
		 	destination_application );
	}
	else
	{
		sprintf( sys_string,
"echo 'umask 0002 && ln %s/src_%s/*.pdf %s/src_%s/' | html_paragraph_wrapper.e",
		 	appaserver_home_directory,
		 	"template",
		 	appaserver_home_directory,
		 	destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	"ln %s/%s/style.css %s/%s/appaserver",
		 	document_root_directory,
		 	destination_application,
			document_root_directory,
			destination_application );
	}
	else
	{
		sprintf( sys_string,
"echo 'ln %s/%s/style.css %s/%s/appaserver' | html_paragraph_wrapper.e",
		 	document_root_directory,
		 	destination_application,
		 	document_root_directory,
		 	destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	"ln %s/%s/index.php %s/%s/appaserver",
		 	document_root_directory,
		 	destination_application,
			document_root_directory,
			destination_application );
	}
	else
	{
		sprintf( sys_string,
"echo 'ln %s/%s/index.php %s/%s/appaserver' | html_paragraph_wrapper.e",
		 	document_root_directory,
		 	destination_application,
		 	document_root_directory,
		 	destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

} /* populate_document_root_directory() */

void fix_index_dot_php(			char *destination_application,
					char *appaserver_home_directory,
					char *new_application_title,
					char create_database_yn,
					char really_yn )
{
	char sys_string[ 2048 ];
	char execute_string[ 2048 ];
	char index_filename[ 128 ];
	char *post_login_sed_executable;

	sprintf(	index_filename,
			"%s/%s/index.php",
		 	appaserver_home_directory,
		 	destination_application );

	post_login_sed_executable =
		get_post_login_sed_executable(
			destination_application,
			create_database_yn );

	sprintf( execute_string,
		 "cat %s						 |"
		 "%s							 |"
		 "cat > /tmp/create_empty_zzz				&&"
		 "mv /tmp/create_empty_zzz %s				  ",
	 	 index_filename,
		 post_login_sed_executable,
	 	 index_filename );

	if ( really_yn == 'y' )
	{
		strcpy( sys_string, execute_string );
	}
	else
	{
		sprintf( sys_string,
			 "echo '%s' | html_paragraph_wrapper.e",
			 execute_string );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	/* Fix application title */
	/* --------------------- */
	sprintf(execute_string,
		"cat %s						 |"
		"sed \"s|<H1>.*</H1>|<H1>%s</H1>|\"		 |"
		"sed \"s|<h1>.*</h1>|<h1>%s</h1>|\"		 |"
		"sed \"s|<TITLE>.*</TITLE>|<TITLE>%s</TITLE>|\"	 |"
		"sed \"s|<title>.*</title>|<title>%s</title>|\"	 |"
		"cat > /tmp/create_empty_zzz			&&"
		"mv /tmp/create_empty_zzz %s			  ",
		index_filename,
		new_application_title,
		new_application_title,
		new_application_title,
		new_application_title,
		index_filename );

	if ( really_yn == 'y' )
	{
		strcpy( sys_string, execute_string );
	}
	else
	{
		sprintf( sys_string,
			 "echo '%s' | html_paragraph_wrapper.e",
			 execute_string );

		search_replace_string( sys_string, "<", "&lt;" );
		search_replace_string( sys_string, ">", "&gt;" );

	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	/* If redirect to cloudacus, change application_key. */
	/* ------------------------------------------------- */
	sprintf(execute_string,
	"cat %s						 		|"
	"sed \"s/set_application_key=template/set_application_key=%s/\"	|"
	"cat > /tmp/create_empty_zzz				       &&"
	"mv /tmp/create_empty_zzz %s			  		",
		index_filename,
		destination_application,
		index_filename );

	if ( really_yn == 'y' )
	{
		strcpy( sys_string, execute_string );
	}
	else
	{
		sprintf( sys_string,
			 "echo '%s' | html_paragraph_wrapper.e",
			 execute_string );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	/* Set the write bit for group */
	/* --------------------------- */
	sprintf(execute_string,
	 	"chmod g+w %s",
	 	index_filename );

	if ( really_yn == 'y' )
	{
		strcpy( sys_string, execute_string );
	}
	else
	{
		sprintf( sys_string,
			 "echo '%s' | html_paragraph_wrapper.e",
			 execute_string );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

} /* fix_index_dot_php() */

#ifdef NOT_DEFINED
void link_appaserver_home_to_document_root(
					char *destination_application,
					char *appaserver_home_directory,
					char *document_root_directory,
					char really_yn )
{
	char sys_string[ 1024 ];

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
			"ln -s %s/%s %s",
			appaserver_home_directory,
		 	destination_application,
		 	document_root_directory );
	}
	else
	{
		sprintf( sys_string,
			"echo 'ln -s %s/%s %s' | html_paragraph_wrapper.e",
			appaserver_home_directory,
		 	destination_application,
		 	document_root_directory );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
			"ln -s %s/src_%s %s",
			appaserver_home_directory,
		 	destination_application,
		 	document_root_directory );
	}
	else
	{
		sprintf( sys_string,
			"echo 'ln -s %s/src_%s %s' | html_paragraph_wrapper.e",
			appaserver_home_directory,
		 	destination_application,
		 	document_root_directory );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

}/* link_appaserver_home_to_document_root() */

void integrate_dynarch_menu(		char *destination_application,
					char *current_application,
					char *document_root_directory,
					char *appaserver_home_directory,
					char really_yn )
{
	char sys_string[ 1024 ];

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
			"cp %s/src/skin-%s.css %s/src/skin-%s.css",
			dynarch_home_directory,
		 	current_application,
		 	dynarch_home_directory,
			destination_application );
	}
	else
	{
		sprintf( sys_string,
"echo 'cp %s/src/skin-%s.css %s/src/skin-%s.css' | html_paragraph_wrapper.e",
			dynarch_home_directory,
		 	current_application,
		 	dynarch_home_directory,
			destination_application );
	}

	fflush( stdout );

	system( sys_string );
	fflush( stdout );

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
			"ln %s/src/skin-%s.css %s/%s",
			dynarch_home_directory,
		 	destination_application,
		 	document_root_directory,
			destination_application );
	}
	else
	{
		sprintf( sys_string,
	"echo 'ln %s/src/skin-%s.css %s/%s' | html_paragraph_wrapper.e",
			dynarch_home_directory,
		 	destination_application,
		 	document_root_directory,
			destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
			"ln %s/%s/style.css %s/src/style-%s.css",
			appaserver_home_directory,
		 	destination_application,
		 	dynarch_home_directory,
			destination_application );
	}
	else
	{
		sprintf( sys_string,
"echo 'ln %s/%s/style.css %s/src/style-%s.css' | html_paragraph_wrapper.e",
			appaserver_home_directory,
		 	destination_application,
		 	dynarch_home_directory,
			destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

} /* integrate_dynarch_menu() */
#endif

void create_system_tables(	char *destination_application,
				char *current_application,
				char *appaserver_data_directory,
				char *session,
				char *login_name,
				char *role_name,
				char *database_string,
				char create_database_yn,
				char really_yn )
{
	char sys_string[ 1024 ];
	char output_string[ 128 ];

	if ( really_yn == 'y' )
		strcpy( output_string, ">/dev/null" );
	else
		*output_string = '\0';


	if ( create_database_yn != 'y' )
	{
		sprintf( sys_string,
		"create_application %s %c %s %s \"%s\" %s y %c %c mysql %s",
			 current_application,
			 'n' /* build_shell_script_yn */,
			 session,
			 login_name,
			 role_name,
			 destination_application,
			 'y' /* no_content_type_yn */,
			 really_yn,
			 output_string );

		fflush( stdout );
		system( sys_string );
		fflush( stdout );
	}
	else
	{
		sprintf( sys_string,
		"create_application %s %c %s %s \"%s\" %s y %c %c mysql %s",
			 current_application,
			 'y' /* build_shell_script_yn */,
			 session,
			 login_name,
			 role_name,
			 current_application,
			 'y' /* no_content_type_yn */,
			 'n' /* really_yn */,
			 output_string );

		fflush( stdout );
		system( sys_string );
		fflush( stdout );

		if ( really_yn == 'y' )
		{
			sprintf( sys_string,
			 	"%s/create_%s_appaserver.sh %s",
			 	appaserver_data_directory,
			 	current_application,
				destination_application );
		}
		else
		{
			sprintf( sys_string,
	"echo '%s/create_%s_appaserver.sh %s' | html_paragraph_wrapper.e",
			 	appaserver_data_directory,
			 	current_application,
				destination_application );
		}

		set_database_environment(
				destination_application,
				create_database_yn,
				really_yn );

		fflush( stdout );
		system( sys_string );
		fflush( stdout );

		restore_database_environment(
				database_string,
				create_database_yn,
				really_yn );
	}
} /* create_system_tables() */

void insert_appaserver_rows(		char *destination_application,
					char *current_application,
					char *appaserver_data_directory,
					char *session,
					char *login_name,
					char *database_string,
					char create_database_yn,
					char really_yn )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
"export_application %s %s %s filler filler delete_yn '%c' filler '%c' mysql shell_script >/dev/null",
		 current_application,
		 session,
		 login_name,
		 'y' /* system_folders_yn */,
		 'y' /* nocontent_type_yn */ );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
			 "gunzip < %s/insert_%s_appaserver.sh.gz 	|"
			 "cat > %s/insert_empty_zzz.sh			;"
			 "chmod +x %s/insert_empty_zzz.sh		;"
			 "%s/insert_empty_zzz.sh %s			;"
			 "rm -f %s/insert_%s_appaserver.sh.gz		;"
			 "rm -f %s/insert_empty_zzz.sh			 ",
			 appaserver_data_directory,
			 current_application,
			 appaserver_data_directory,
			 appaserver_data_directory,
			 appaserver_data_directory,
			 destination_application,
			 appaserver_data_directory,
			 destination_application,
			 appaserver_data_directory );
	}
	else
	{
		sprintf( sys_string,
"echo 'Will decompress and execute %s/insert_%s_appaserver.sh.gz %s' | html_paragraph_wrapper.e",
			 appaserver_data_directory,
			 destination_application,
			 destination_application );
	}

	if ( create_database_yn == 'y' )
	{
		set_database_environment(
					destination_application,
					create_database_yn,
					really_yn );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	if ( create_database_yn == 'y' )
	{
		restore_database_environment(
					database_string,
					create_database_yn,
					really_yn );
	}

} /* insert_appaserver_rows() */

void delete_non_appaserver_rows(	char *destination_application,
					char *current_application,
					char *database_string,
					char *login_name,
					char *role_name,
					char create_database_yn,
					char really_yn )
{
/*
	delete_attribute_non_appaserver_rows(
					destination_application,
					current_application,
					database_string,
					login_name,
					role_name,
					create_database_yn,
					really_yn );
*/

	delete_folder_non_appaserver_rows(
				destination_application,
				current_application,
				database_string,
				login_name,
				role_name,
				create_database_yn,
				really_yn );

	delete_process_non_appaserver_rows(
				destination_application,
				database_string,
				create_database_yn,
				really_yn );

	delete_appaserver_user_rows(
				destination_application,
				database_string,
				create_database_yn,
				really_yn );

} /* delete_non_appaserver_rows() */

void delete_folder_non_appaserver_rows(	char *destination_application,
					char *current_application,
					char *database_string,
					char *login_name,
					char *role_name,
					char create_database_yn,
					char really_yn )
{
	char sys_string[ 1024 ];
	char folder_name[ 128 ];
	FILE *input_pipe;
	FILE *delete_pipe;
	char *sql_executable;

	if ( really_yn == 'y' )
	{
		sql_executable = "sql.e";
	}
	else
	{
		printf( "<p>Will delete non-appaserver folders.\n" );
		return;
	}

	/* Open folder input_pipe */
	/* ---------------------- */
	sprintf( sys_string,
		 "application_folder_list.sh %s n",
		 current_application );

	input_pipe = popen( sys_string, "r" );

	if ( create_database_yn == 'y' )
	{
		set_database_environment(
					destination_application,
					create_database_yn,
					really_yn );
	}

	/* Open output pipe */
	/* ---------------- */
	sprintf( sys_string,
	"delete_folder_row %s session %s %s \"%s\" %s \"%s\"",
		 destination_application,
		 login_name,
		 "folder",
		 role_name,
		 "stdin",
		 sql_executable );

	delete_pipe = popen( sys_string, "w" );

	fflush( stdout );
	while( get_line( folder_name, input_pipe ) )
	{
		fprintf( delete_pipe, "%s\n", folder_name );
	}
	fflush( stdout );

	pclose( input_pipe );
	pclose( delete_pipe );

	if ( create_database_yn == 'y' )
	{
		restore_database_environment(
					database_string,
					create_database_yn,
					really_yn );
	}

} /* delete_folder_non_appaserver_rows() */

void delete_attribute_non_appaserver_rows(
					char *destination_application,
					char *current_application,
					char *database_string,
					char *login_name,
					char *role_name,
					char create_database_yn,
					char really_yn )
{
	char sys_string[ 65536 ];
	char *folder_list_string;
	char *in_clause;
	char *table_name;
	FILE *input_pipe;
	FILE *delete_pipe;
	char *sql_executable;
	char attribute_name[ 128 ];

	if ( really_yn == 'y' )
	{
		sql_executable = "sql.e";
	}
	else
	{
		printf( "<p>Will delete non-appaserver attributes.\n" );
		return;
	}

	/* Open folder input_pipe */
	/* ---------------------- */
	sprintf( sys_string,
		 "application_folder_list.sh %s n | joinlines.e ','",
		 current_application );

	folder_list_string = pipe2string( sys_string );

	if ( !folder_list_string || !*folder_list_string ) return;

	in_clause = timlib_get_in_clause( folder_list_string );

	application_reset();
	table_name = get_table_name(	current_application,
					"folder_attribute" );

	sprintf( sys_string,
		 "echo \"	select distinct attribute	 "
		 "		from %s				 "
		 "		where folder in (%s);\"		|"
		 "sql.e						 ",
		 table_name,
		 in_clause );

	input_pipe = popen( sys_string, "r" );

	if ( create_database_yn == 'y' )
	{
		set_database_environment(
					destination_application,
					create_database_yn,
					really_yn );
	}

	/* Open output pipe */
	/* ---------------- */
	sprintf( sys_string,
	"delete_folder_row %s session %s %s \"%s\" %s \"%s\"",
		 destination_application,
		 login_name,
		 "attribute",
		 role_name,
		 "stdin",
		 sql_executable );

	delete_pipe = popen( sys_string, "w" );

	fflush( stdout );
	while( get_line( attribute_name, input_pipe ) )
	{
		fprintf( delete_pipe, "%s\n", attribute_name );
	}
	fflush( stdout );

	pclose( delete_pipe );
	pclose( input_pipe );

	if ( create_database_yn == 'y' )
	{
		restore_database_environment(
					database_string,
					create_database_yn,
					really_yn );
	}

} /* delete_attribute_non_appaserver_rows() */

void delete_process_non_appaserver_rows(
				char *destination_application,
				char *database_string,
				char create_database_yn,
				char really_yn )
{
	char sys_string[ 1024 ];
	char *where;
	char *table_name;
	char *sql_executable;

	if ( really_yn == 'y' )
	{
		sql_executable = "sql.e";
	}
	else
	{
		printf( "<p>Will delete non-appaserver processes.\n" );
		return;
	}

	if ( create_database_yn == 'y' )
	{
		set_database_environment(
					destination_application,
					create_database_yn,
					really_yn );
	}

	where = "appaserver_yn is null or appaserver_yn != 'y'";
	application_reset();
	table_name = get_table_name( destination_application, "process" );

	sprintf( sys_string,
		 "echo \"delete from %s where %s;\" | %s",
		 table_name,
		 where,
		 sql_executable );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	if ( create_database_yn == 'y' )
	{
		restore_database_environment(
					database_string,
					create_database_yn,
					really_yn );
	}

} /* delete_process_non_appaserver_rows() */

void delete_appaserver_user_rows(
				char *destination_application,
				char *database_string,
				char create_database_yn,
				char really_yn )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *sql_executable;

	if ( really_yn == 'y' )
	{
		sql_executable = "sql.e";
	}
	else
	{
		printf( "<p>Will delete from appaserver_user.\n" );
		printf( "<p>Will delete all role_appaserver_user.\n" );
		printf( "<p>Will delete all login_default_role.\n" );
		return;
	}

	if ( create_database_yn == 'y' )
	{
		set_database_environment(
					destination_application,
					create_database_yn,
					really_yn );
	}

	application_reset();

	/* APPASERVER_USER */
	/* --------------- */
	table_name =
		get_table_name(
			destination_application, "appaserver_user" );

	sprintf( sys_string,
		 "echo \"delete from %s;\" | %s",
		 table_name,
		 sql_executable );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	/* ROLE_APPASERVER_USER */
	/* -------------------- */
	table_name =
		get_table_name(
			destination_application, "role_appaserver_user" );

	sprintf( sys_string,
		 "echo \"delete from %s;\" | %s",
		 table_name,
		 sql_executable );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	/* LOGIN_DEFAULT_ROLE */
	/* ------------------ */
	table_name =
		get_table_name(
			destination_application, "login_default_role" );

	sprintf( sys_string,
		 "echo \"delete from %s;\" | %s",
		 table_name,
		 sql_executable );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	if ( create_database_yn == 'y' )
	{
		restore_database_environment(
					database_string,
					create_database_yn,
					really_yn );
	}
} /* delete_appaserver_user_rows() */

void insert_appaserver_user_row(
				char *destination_application,
				char *database_string,
				char create_database_yn,
				char *login_name,
				char really_yn )
{
	char sys_string[ 1024 ];
	char *sql_executable;
	char *password;

	if ( really_yn == 'y'
	&&   timlib_login_name_email_address( login_name ) )
	{
		char sys_string[ 256 ];
		char password_expression[ 256 ];

		sprintf( sys_string,
			 "session_number_new.sh %s",
			 destination_application );

		sprintf( password_expression,
			 "password('%s')",
			 pipe2string( sys_string ) );

		password = strdup( password_expression );
	}
	else
	{
		password = RESETTED_PASSWORD;
	}

	if ( really_yn == 'y' )
	{
		sql_executable = "sql.e";
	}
	else
	{
		sql_executable = "html_paragraph_wrapper.e";
	}

	if ( create_database_yn == 'y' )
	{
		set_database_environment(
					destination_application,
					create_database_yn,
					really_yn );
	}

	application_reset();

	sprintf( sys_string,
"echo \"%s^%s^%s\" | insert_statement.e table=%s field=%s delimiter='^' | %s",
		 login_name,
		 password,
		 "international",
		 "appaserver_user",
		 "login_name,password,user_date_format",
		 sql_executable );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	/* ------------------------ */
	/* Insert into three roles. */
	/* ------------------------ */
	application_reset();

	sprintf( sys_string,
"echo \"%s^%s\" | insert_statement.e table=%s field=%s delimiter='^' | %s",
		 login_name,
		 SYSTEM_ADMINISTRATION_ROLE,
		 "role_appaserver_user",
		 "login_name,role",
		 sql_executable );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	sprintf( sys_string,
"echo \"%s^%s\" | insert_statement.e table=%s field=%s delimiter='^' | %s",
		 login_name,
		 SUPERVISOR_ROLE,
		 "role_appaserver_user",
		 "login_name,role",
		 sql_executable );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	sprintf( sys_string,
"echo \"%s^%s\" | insert_statement.e table=%s field=%s delimiter='^' | %s",
		 login_name,
		 DATAENTRY_ROLE,
		 "role_appaserver_user",
		 "login_name,role",
		 sql_executable );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	if ( create_database_yn == 'y' )
	{
		restore_database_environment(
					database_string,
					create_database_yn,
					really_yn );
	}

} /* insert_appaserver_user_row() */

void make_appaserver_error_file(	char *destination_application,
					char *appaserver_error_directory,
					char really_yn )
{
	char sys_string[ 1024 ];
	char *error_filename;

	error_filename =
		get_error_filename(
			appaserver_error_directory,
			destination_application );

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	 "echo '' > %s",
		 	 error_filename );
		system( sys_string );

		sprintf( sys_string,
		 	 "chmod g+rw %s",
		 	 error_filename );
		system( sys_string );

		sprintf( sys_string,
		 	 "chmod o-r %s",
		 	 error_filename );
		system( sys_string );
	}
	else
	{
		printf( "<p>Will create %s.\n", error_filename );
	}

} /* make_appaserver_error_file() */

boolean create_empty_application(
				char *current_application,
				char *destination_application,
				char create_database_yn,
				char *session,
				char *login_name,
				char *role_name,
				char *database_string,
				char *appaserver_error_directory,
				char *new_application_title,
				char *document_root_directory,
				char *appaserver_data_directory,
				char *appaserver_home_directory,
				char really_yn )
{
	if ( appaserver_library_application_exists(
					destination_application,
					appaserver_error_directory ) )
	{
		return 0;
	}

	if ( create_database_yn == 'y' )
	{
		create_database(	destination_application,
					really_yn );
	}

	create_application_table(	current_application,
					destination_application,
					create_database_yn,
					session,
					login_name,
					role_name,
					database_string,
					appaserver_data_directory,
					really_yn );

	insert_application_row(		current_application,
					destination_application,
					create_database_yn,
					session,
					login_name,
					role_name,
					database_string,
					appaserver_data_directory,
					really_yn );

	update_application_row(		destination_application,
					create_database_yn,
					database_string,
					new_application_title,
					really_yn );

	make_appaserver_home_directory(	destination_application,
					appaserver_home_directory,
					really_yn );

	make_document_root_directory(	destination_application,
					document_root_directory,
					appaserver_home_directory,
					really_yn );

	populate_document_root_directory(
					destination_application,
					document_root_directory,
					appaserver_home_directory,
					really_yn );

	fix_index_dot_php(		destination_application,
					document_root_directory,
					new_application_title,
					create_database_yn,
					really_yn );

	create_system_tables(		destination_application,
					current_application,
					appaserver_data_directory,
					session,
					login_name,
					role_name,
					database_string,
					create_database_yn,
					really_yn );

	insert_appaserver_rows(		destination_application,
					current_application,
					appaserver_data_directory,
					session,
					login_name,
					database_string,
					create_database_yn,
					really_yn );

	delete_non_appaserver_rows(	destination_application,
					current_application,
					database_string,
					login_name,
					role_name,
					create_database_yn,
					really_yn );

	insert_appaserver_user_row(
					destination_application,
					database_string,
					create_database_yn,
					login_name,
					really_yn );

	make_appaserver_error_file(	destination_application,
					appaserver_error_directory,
					really_yn );

	if ( really_yn == 'y'
	&&   !timlib_login_name_email_address( login_name ) )
	{
		printf(
"<p>Process completed. Log into %s using password = %s.\n",
			destination_application,
			RESETTED_PASSWORD );
	}

	return 1;

} /* create_empty_application() */

void drop_database(		char *destination_application,
				char really_yn )
{
	char sys_string[ 1024 ];

	if ( really_yn != 'y' )
	{
		sprintf( sys_string,
		 	"echo \"drop database %s;\"	|"
			"html_paragraph_wrapper.e",
		 	destination_application );
	}
	else
	{
		sprintf( sys_string,
		 	"echo \"drop database %s;\"	|"
			"sql.e 2>&1			|"
			"html_paragraph_wrapper.e",
		 	destination_application );
	}
	fflush( stdout );
	system( sys_string );
	fflush( stdout );

} /* drop_database() */

void drop_application_tables(		char *destination_application,
					char really_yn )
{
	char sys_string[ 1024 ];
	char *sql_executable;

	if ( really_yn == 'y' )
	{
		sql_executable = "sql.e 2>&1";
	}
	else
	{
		sql_executable = "cat";
	}

	sprintf( sys_string,
		 "echo \"show tables;\"					|"
		 "sql.e							|"
		 "grep '^%s_'						|"
		 "sed 's/.*/drop table &;/'				|"
		 "%s							|"
		 "html_paragraph_wrapper.e				 ",
		 destination_application,
		 sql_executable );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

} /* drop_application_tables() */

void remove_home_directory(		char *destination_application,
					char *appaserver_home_directory,
					char really_yn )
{
	char sys_string[ 1024 ];

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	"rm -fr %s/%s 2>&1 | html_paragraph_wrapper",
		 	appaserver_home_directory,
		 	destination_application );
	}
	else
	{
		sprintf( sys_string,
		 	"echo \"rm -fr %s/%s\" | html_paragraph_wrapper.e",
		 	appaserver_home_directory,
		 	destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	"rm -fr %s/src_%s 2>&1 | html_paragraph_wrapper",
		 	appaserver_home_directory,
		 	destination_application );
	}
	else
	{
		sprintf( sys_string,
		 	"echo \"rm -fr %s/src_%s\" | html_paragraph_wrapper.e",
		 	appaserver_home_directory,
		 	destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

} /* remove_home_directory() */

void remove_appaserver_error_file(	char *destination_application,
					char *appaserver_error_directory,
					char really_yn )
{
	char sys_string[ 1024 ];
	char *error_filename;

	error_filename =
		get_error_filename(
			appaserver_error_directory,
			destination_application );

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	 "rm %s 2>&1 | html_paragraph_wrapper",
		 	 error_filename );
		system( sys_string );
	}
	else
	{
		printf( "<p>rm %s\n", error_filename );
	}

} /* remove_appaserver_error_file() */

void remove_document_root_links(
				char *destination_application,
				char *document_root_directory,
				char really_yn )
{
	char sys_string[ 1024 ];

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
			"rm %s/%s 2>&1 | html_paragraph_wrapper",
		 	document_root_directory,
		 	destination_application );
	}
	else
	{
		sprintf( sys_string,
			"echo \"rm %s/%s\" | html_paragraph_wrapper.e",
		 	document_root_directory,
		 	destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
			"rm %s/src_%s 2>&1 | html_paragraph_wrapper",
		 	document_root_directory,
		 	destination_application );
	}
	else
	{
		sprintf( sys_string,
			"echo \"rm %s/src_%s\" | html_paragraph_wrapper.e",
		 	document_root_directory,
		 	destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

} /*remove_document_root_links() */

#ifdef NOT_DEFINED
void remove_dynarch_menu(		char *destination_application,
					char *dynarch_home_directory,
					char really_yn )
{
	char sys_string[ 1024 ];

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
			"rm %s/src/skin-%s.css 2>&1 | html_paragraph_wrapper",
			dynarch_home_directory,
		 	destination_application );
	}
	else
	{
		sprintf( sys_string,
			"echo \"rm %s/src/skin-%s.css\"	|"
			"html_paragraph_wrapper.e",
			dynarch_home_directory,
		 	destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
			"rm %s/src/style-%s.css 2>&1 | html_paragraph_wrapper",
			dynarch_home_directory,
		 	destination_application );
	}
	else
	{
		sprintf( sys_string,
			"echo \"rm %s/src/style-%s.css\"	|"
			"html_paragraph_wrapper.e",
			dynarch_home_directory,
		 	destination_application );
	}

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

} /* remove_dynarch_menu() */
#endif

boolean delete_existing_application(
				char *destination_application,
				char create_database_yn,
				char *appaserver_error_directory,
				char *document_root_directory,
				char really_yn )
{
	if ( !appaserver_library_application_exists(
					destination_application,
					appaserver_error_directory ) )
	{
		return 0;
	}

	if ( create_database_yn == 'y' )
	{
		drop_database(		destination_application,
					really_yn );
	}
	else
	{
		drop_application_tables(
					destination_application,
					really_yn );
	}

	remove_home_directory(		destination_application,
					document_root_directory,
					really_yn );

/*
	remove_document_root_links(	destination_application,
					document_root_directory,
					really_yn );
*/

	remove_appaserver_error_file(	destination_application,
					appaserver_error_directory,
					really_yn );

/*
	remove_dynarch_menu(		destination_application,
					dynarch_home_directory,
					really_yn );
*/

	if ( really_yn != 'y' )
		printf( "<p>Process simulation complete.\n" );
	else
		printf( "<p>Process complete.\n" );

	return 1;

} /* delete_existing_application() */

char *get_post_login_sed_executable(
			char *destination_application,
			char create_database_yn )
{
	static char sed_executable[ 512 ];

	if ( create_database_yn == 'y' )
	{
		sprintf( sed_executable,
	 "sed \"s/post_login?.*\\\"/post_login?%s:%s\\\"/\"",
			 destination_application,
			 destination_application );
	}
	else
	{
		sprintf( sed_executable,
	 "sed \"s/post_login?.*\\\"/post_login?%s:%s\\\"/\"",
			 destination_application,
			 appaserver_parameter_file_get_database() );
	}

	return sed_executable;
} /* get_post_login_sed_executable() */

char *get_error_filename(	char *appaserver_error_directory,
				char *destination_application )
{
	static char error_filename[ 256 ];

	sprintf( error_filename,
		 "%s/appaserver_%s.err",
		 appaserver_error_directory,
		 destination_application );

	return error_filename;

} /* get_error_filename() */

