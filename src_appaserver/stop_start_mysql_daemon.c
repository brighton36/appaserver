/* --------------------------------------------------- 	*/
/* stop_start_mysql_daemon.c			       	*/
/* --------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "document.h"
#include "list.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"

/* Constants */
/* --------- */
#define SNOOK_MYSQL_ROOT_DIRECTORY	"/opt/mysql"
#define TARPON_MYSQL_ROOT_DIRECTORY	"/export/opt/mysql"
#define BARRACUDA_MYSQL_ROOT_DIRECTORY	"/usr"
#define GOOSE_MYSQL_ROOT_DIRECTORY	"/usrb2/local"
/* #define GOOSE_MYSQL_LOG_DIRECTIVE	"" */
#define GOOSE_MYSQL_LOG_DIRECTIVE	"--log-update=/usrb2/local/var/logfiles/goose"
#define DEFAULT_MYSQL_ROOT_DIRECTORY	"/usr/local"

/* Prototypes */
/* ---------- */
void kill_mysql_processes(	LIST *mysql_pid_string_list, 
				int html_ok,
				char really_yn );
char *get_mysql_pid_sys_string( void );

int main( int argc, char **argv )
{
	char *application_name;
	char really_yn;
	char sys_string[ 1024 ];
	LIST *mysql_pid_string_list;
	DOCUMENT *document = {0};
	int html_ok;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *uname;
	char html_output[ 1024 ];
	char *database_string = {0};

	if ( argc != 4 )
	{
		fprintf(stderr, "Usage: %s application html|nohtml really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	html_ok = (strcmp( argv[ 2 ], "html" ) == 0);
	really_yn = *argv[ 3 ];

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

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();

	appaserver_parameter_file = new_appaserver_parameter_file();
	uname = pipe2string( "uname -n" );

	if ( html_ok )
	{
		document = document_new( "", application_name );
		document_set_output_content_type( document );

		document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_get_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );


		document_output_body(
				document->application_name,
				document->onload_control_string );

		strcpy( html_output, "html_paragraph_wrapper" );
	}
	else
	{
		strcpy( html_output, "cat -" );
	}

	strcpy( sys_string, get_mysql_pid_sys_string() );
	mysql_pid_string_list = pipe2list( sys_string );

	kill_mysql_processes( mysql_pid_string_list, html_ok, really_yn );

	if ( strcmp( uname, "snook" ) == 0 )
	{
		sprintf( sys_string,
		 	"cd %s					;"
		 	"./bin/safe_mysqld 2>&1			|"
		 	"html_paragraph_wrapper &		 ",
			SNOOK_MYSQL_ROOT_DIRECTORY );
	}
	else
	if ( strcmp( uname, "tarpon" ) == 0 )
	{
		sprintf( sys_string,
		 	"cd %s					;"
		 	"./bin/safe_mysqld 2>&1			|"
		 	"html_paragraph_wrapper &		 ",
			TARPON_MYSQL_ROOT_DIRECTORY );
	}
	else
	if ( strcmp( uname, "barracuda" ) == 0 )
	{
		sprintf( sys_string,
		 	"cd %s					;"
		 	"./bin/safe_mysqld 2>&1 &		 ",
			BARRACUDA_MYSQL_ROOT_DIRECTORY );
	}
	else
	if ( strcmp( uname, "goose" ) == 0 )
	{
		sprintf( sys_string,
		 	"cd %s					;"
		 	"./bin/safe_mysqld %s 2>&1		|"
		 	"html_paragraph_wrapper &		 ",
			GOOSE_MYSQL_ROOT_DIRECTORY,
			GOOSE_MYSQL_LOG_DIRECTIVE );
	}
	else
	{
		sprintf( sys_string,
		 	"cd %s					;"
		 	"./bin/start_mysqld.sh 2>&1 &		 ",
			DEFAULT_MYSQL_ROOT_DIRECTORY );
	}

	printf( "\nExecuting: %s\n", sys_string );

	if ( toupper( really_yn ) == 'Y' )
		system( sys_string );	
	else
		printf( "Not executed\n" );

	if ( html_ok ) document_close();
	exit( 0 );
} /* main() */


char *get_mysql_pid_sys_string( void )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "ps -ef 					|"
		 "grep -v grep 					|"
		 "grep 'mysqld'					|"
		 "column.e 1					 " );
	return strdup( sys_string );

} /* get_mysql_pid_sys_string() */


void kill_mysql_processes(	LIST *mysql_pid_string_list, 
				int html_ok,
				char really_yn )
{
	char *pid_string;
	char sys_string[ 1024 ];

	if ( !list_rewind( mysql_pid_string_list ) )
	{
		
		printf( "\nWarning: cannot get pid of mysql daemon\n" );
		return;
	}

	do {
		pid_string = list_get_string( mysql_pid_string_list );

		if ( html_ok )
		{
			sprintf( sys_string, 
			 	"kill %s 2>&1 | html_paragraph_wrapper",
			 	pid_string );
		}
		else
		{
			sprintf( sys_string, 
			 	"kill %s 2>&1",
			 	pid_string );
		}

		if ( really_yn == 'y' )
			system( sys_string );
		else
			printf( "%s\n", sys_string );
	} while( list_next( mysql_pid_string_list ) );

	/* Do again with kill -9 */
	/* --------------------- */
	list_rewind( mysql_pid_string_list );
	do {
		pid_string = list_get_string( mysql_pid_string_list );

		if ( html_ok )
		{
			sprintf( sys_string, 
			 	"kill -9 %s 2>&1 | html_paragraph_wrapper",
			 	pid_string );
		}
		else
		{
			sprintf( sys_string, 
			 	"kill -9 %s 2>&1",
			 	pid_string );
		}

		if ( really_yn == 'y' )
			system( sys_string );
		else
			printf( "%s\n", sys_string );
	} while( list_next( mysql_pid_string_list ) );
} /* kill_mysql_processes() */

