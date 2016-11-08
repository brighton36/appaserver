/* ---------------------------------------------------	*/
/* grant_select_to_user.c				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "application_constants.h"
#include "appaserver_user.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "element.h"
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "environ.h"
#include "application.h"
#include "process.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define ENCRYPTED_PASSWORD_LENGTH	41

/* Prototypes */
/* ---------- */
char *get_ip_address(	char *application_name );

char *get_revoke_only_sys_string(
			char *login_name,
			char really_yn );

char *get_sys_string(	char *application_name,
			char *login_name,
			char really_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *login_name;
	char buffer[ 128 ];
	char revoke_only_yn;
	char really_yn;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *sys_string;
	int results;

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process login_name revoke_only_yn really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	login_name = argv[ 3 ];
	revoke_only_yn = *argv[ 4 ];
	really_yn = *argv[ 5 ];

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

/*
	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );
*/

	appaserver_parameter_file = new_appaserver_parameter_file();

	document = document_new( "", application_name );
	document_set_output_content_type( document );

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

	printf( "<h2>%s\n", format_initial_capital( buffer, process_name ) );
	fflush( stdout );
	results = system( "date '+%x %H:%M'" );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( revoke_only_yn == 'y' )
	{
		sys_string = get_revoke_only_sys_string(
					login_name,
					really_yn );
	}
	else
	{
		sys_string = get_sys_string(	application_name,
						login_name,
						really_yn );
	}

/* fprintf( stderr, "%s\n", sys_string ); */

	results = system( sys_string );

	if ( really_yn == 'y' )
	{
		printf( "<p>Process complete.\n" );
	}
	else
	{
		printf( "<p>Process not executed.\n" );
	}

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

#ifdef NOT_DEFINED
char *get_sys_string(	char *application_name,
			char *login_name,
			char really_yn )
{
	static char sys_string[ 2048 ];
	char where[ 1024 ];
	char where_join[ 512 ];
	char *folder;
	char sed_string[ 512 ];
	char execution_process[ 512 ];
	char delete_process[ 512 ];
	char testing_processes[ 1024 ];
	char update_password_process[ 512 ];
	char grant_update_process[ 512 ];
	char *password;
	char *ip_address;

	password = appaserver_user_get_password(
					application_name,
					login_name );

	folder = "role_appaserver_user,role_folder";

	strcpy( where_join, "role_appaserver_user.role = role_folder.role" );

	sprintf( where,
		 "( role_appaserver_user.login_name = '%s' and		"
		 "permission in ('lookup','update') ) or		"
		 "( role_folder.role like 'public%c' and		"
		 "permission in ('lookup','update') ) or		"
		 "( role_folder.folder = 'application' ) and		"
		 "%s							",
		 login_name,
		 '%',
		 where_join );

	sprintf( update_password_process, "echo '' > /dev/null" );

	/* Delete process */
	/* -------------- */
	if ( really_yn == 'y' )
	{
		sprintf( delete_process,
"echo \"delete from user where user = '%s' and password = '';\" |"
"sql.e '^' mysql mysql",
		 	login_name );

/*
		if ( strlen( password ) == ENCRYPTED_PASSWORD_LENGTH
		&&   *password == '*' )
		{
*/
			sprintf( update_password_process,
"echo \"update user set password = '%s' where user = '%s';flush privileges;\" |"
"sql.e '^' mysql mysql",
			password,
			login_name );
/*
		}
*/
	}
	else
	{
		sprintf( delete_process,
"echo \"delete from user where user = '%s' and password = '';\" |"
"html_paragraph_wrapper.e | html_table.e 'Delete Statement' '' '|'",
		 	login_name );
	}

	/* Grant update process */
	/* -------------------- */
	if ( really_yn == 'y' )
	{
		sprintf( grant_update_process,
"echo \"grant update on process to '%s';\" |"
"sql.e '^'",
		 	login_name );

	}
	else
	{
		sprintf( grant_update_process,
"echo \"grant update on process to '%s';\" |"
"html_paragraph_wrapper.e | html_table.e 'Grant Update' '' '|'",
		 	login_name );
	}

	if ( really_yn == 'y' )
	{
		strcpy( testing_processes, "cat" );
		strcpy( execution_process, "sql.e" );
	}
	else
	{
		sprintf(testing_processes,
		 	"queue_top_bottom_lines.e 50			|"
		 	"html_table.e 'Grant Statements' '' '|'	 " );
		strcpy( execution_process, "cat" );
		element_password_erase_data( password );
	}

	sprintf(	sed_string,
			"sed 's/.*/grant select on & to %s;/'",
			login_name );

	ip_address = get_ip_address( application_name );

	sprintf( sys_string,
	"(echo \"grant usage on *.* to '%s'@'%s' identified by '%s';\"	;"
	 "echo \"revoke all on *.* from %s;\"				;"
	 "get_folder_data	application=%s				 "
	 "			select=folder				 "
	 "			folder=%s				 "
	 "			where=\"%s\"				|"
	 "grep -v '^null$'						|"
	 "sed 's/^application$/%s_application/'				|"
	 "%s								|"
	 "sort -u)							|"
	 "%s								|"
	 "%s								|"
	 "cat								;"
	 "%s								;"
	 "%s								;"
	 "%s								 ",
		 login_name,
		 ip_address,
		 password,
		 login_name,
		 application_name,
		 folder,
		 where,
		 application_name,
		 sed_string,
		 testing_processes,
		 execution_process,
		 delete_process,
		 update_password_process,
		 grant_update_process );

	return sys_string;

} /* get_sys_string() */
#endif

char *get_sys_string(	char *application_name,
			char *login_name,
			char really_yn )
{
	static char sys_string[ 2048 ];
	char sed_string[ 512 ];
	char execution_process[ 512 ];
	char delete_process[ 512 ];
	char testing_processes[ 1024 ];
	char update_password_process[ 512 ];
	char grant_update_process[ 512 ];
	char *password;
	char *ip_address;

	password = appaserver_user_get_password(
					application_name,
					login_name );

	sprintf( update_password_process, "echo '' > /dev/null" );

	/* Delete process */
	/* -------------- */
	if ( really_yn == 'y' )
	{
		sprintf( delete_process,
"echo \"delete from user where user = '%s' and password = '';\" |"
"sql.e '^' mysql mysql",
		 	login_name );

/*
		if ( strlen( password ) == ENCRYPTED_PASSWORD_LENGTH
		&&   *password == '*' )
		{
*/
			sprintf( update_password_process,
"echo \"update user set password = '%s' where user = '%s';flush privileges;\" |"
"sql.e '^' mysql mysql",
			password,
			login_name );
/*
		}
*/
	}
	else
	{
		sprintf( delete_process,
"echo \"delete from user where user = '%s' and password = '';\" |"
"html_paragraph_wrapper.e | html_table.e 'Delete Statement' '' '|'",
		 	login_name );
	}

	/* Grant update process */
	/* -------------------- */
	if ( really_yn == 'y' )
	{
		sprintf( grant_update_process,
"echo \"grant update on process to '%s';\" |"
"sql.e '^'",
		 	login_name );

	}
	else
	{
		sprintf( grant_update_process,
"echo \"grant update on process to '%s';\" |"
"html_paragraph_wrapper.e | html_table.e 'Grant Update' '' '|'",
		 	login_name );
	}

	if ( really_yn == 'y' )
	{
		strcpy( testing_processes, "cat" );
		strcpy( execution_process, "sql.e" );
	}
	else
	{
		sprintf(testing_processes,
		 	"queue_top_bottom_lines.e 50			|"
		 	"html_table.e 'Grant Statements' '' '|'	 " );
		strcpy( execution_process, "cat" );
		element_password_erase_data( password );
	}

	sprintf(	sed_string,
			"sed 's/.*/grant select on & to %s;/'",
			login_name );

	ip_address = get_ip_address( application_name );

	sprintf( sys_string,
	"(echo \"grant usage on *.* to '%s'@'%s' identified by '%s';\"	;"
	 "echo \"revoke all on *.* from %s;\"				;"
	 "application_folder_list.sh %s n				|"
	 "grep -v '^null$'						|"
	 "sed 's/^application$/%s_application/'				|"
	 "%s								|"
	 "sort -u)							|"
	 "%s								|"
	 "%s								|"
	 "cat								;"
	 "%s								;"
	 "%s								;"
	 "%s								 ",
		 login_name,
		 ip_address,
		 password,
		 login_name,
		 application_name,
		 application_name,
		 sed_string,
		 testing_processes,
		 execution_process,
		 delete_process,
		 update_password_process,
		 grant_update_process );

	return sys_string;

} /* get_sys_string() */

char *get_revoke_only_sys_string(
			char *login_name,
			char really_yn )
{
	static char sys_string[ 1024 ];

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
	"echo \"revoke all on *.* from %s;\" | sql.e;"
	"echo \"delete from user where user = '%s';\" | sql.e '^' mysql mysql;"
	"echo \"flush privileges;\" | sql.e '^' mysql mysql",
			 login_name,
			 login_name );
	}
	else
	{
		sprintf( sys_string,
"echo \"revoke all on *.* from %s; | sql.e\" | html_paragraph_wrapper;"
"echo \"delete from user where user = '%s' | sql.e '^' mysql mysql\" | html_paragraph_wrapper;"
"echo \"flush privileges; | sql.e '^' mysql mysql\" | html_paragraph_wrapper",
			 login_name,
			 login_name );
	}

	return sys_string;

} /* get_revoke_only_sys_string() */

char *get_ip_address( char *application_name )
{
	char *ip_address;
	APPLICATION_CONSTANTS *application_constants;

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	if ( ! ( ip_address =
		application_constants_fetch(
			application_constants->dictionary,
			"grant_select_ip_address" ) ) )
	{
		return "localhost";
	}
	return ip_address;
} /* get_ip_address() */
