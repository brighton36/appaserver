/* ------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/grant_select_to_user.c	*/
/* ------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------	*/

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
/* #define CREATE_USER_COMMAND "grant usage on *.* to" */
#define CREATE_USER_COMMAND "create user"

#define ENCRYPTED_PASSWORD_LENGTH	41

/* Prototypes */
/* ---------- */
/*
char *get_ip_address(	char *application_name );
*/

char *get_revoke_only_sys_string(
			char *login_name,
			char really_yn );

char *get_sys_string(	char *application_name,
			char *login_name,
			char *connect_from_host,
			char really_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *login_name;
	char buffer[ 128 ];
	char *connect_from_host;
	char revoke_only_yn;
	char really_yn;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *sys_string;

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s application process login_name connect_from_host revoke_only_yn really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	login_name = argv[ 3 ];
	connect_from_host = argv[ 4 ];
	revoke_only_yn = *argv[ 5 ];
	really_yn = *argv[ 6 ];

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

	appaserver_parameter_file = appaserver_parameter_file_new();

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
	system( "date '+%x %H:%M'" );
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
						connect_from_host,
						really_yn );
	}

/* fprintf( stderr, "%s\n", sys_string ); */

	system( sys_string );

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

char *get_sys_string(	char *application_name,
			char *login_name,
			char *connect_from_host,
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
	/* char *ip_address; */

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

			sprintf( update_password_process,
"echo \"update user set password = '%s' where user = '%s';flush privileges;\" |"
"sql.e '^' mysql mysql",
			password,
			login_name );
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

	/* ip_address = get_ip_address( application_name ); */

	sprintf( sys_string,
	"(echo \"%s '%s'@'%s' identified by '%s';\"			;"
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
		 CREATE_USER_COMMAND,
		 login_name,
		 connect_from_host,
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

#ifdef NOT_DEFINED
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
#endif
