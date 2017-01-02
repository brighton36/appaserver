/* sql.c		 						*/
/* -------------------------------------------------------------------- */
/* This compiles to sql, sql.e, and sql_quick.e.			*/
/* To override the database_management_system found in			*/
/* /etc/appaserver.config, pass as					*/
/* parameters:								*/
/* 1) the delimiter							*/
/* 2) the database_management_system					*/
/* 3) the database_name							*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include "timlib.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "sql.h"
#include "environ.h"
#include "basename.h"

#define SLEEP_MICRO_SECONDS		10000

int main( int argc, char **argv )
{
	APPASERVER_PARAMETER_FILE *h;
	char sys_string[ 8192 ];
	char delimiter;
	char *base_name;
	char *quick_flag;
	char *database_management_system = {0};
	char *override_database = {0};
	int results;

	/* ------------------------------------------ */
	/* Usage: sql.e [delimiter] [DBMS] [database] */
	/* ------------------------------------------ */

	if ( argc == 4 )
	{
		delimiter = *argv[ 1 ];
		database_management_system = argv[ 2 ];
		override_database = argv[ 3 ];
	}
	else
	if ( argc == 3 )
	{
		delimiter = *argv[ 1 ];
		database_management_system = argv[ 2 ];
	}
	else
	if ( argc == 2 )
	{
		delimiter = *argv[ 1 ];
	}
	else
	{
		delimiter = '^';
	}

	environ_prepend_dot_to_path();
	add_local_bin_to_path();
	add_dot_to_path();
	add_etc_to_path();

	base_name = basename_get_base_name( argv[ 0 ], 1 );

	if ( strcmp( base_name, "sql_quick" ) == 0 )
		quick_flag = "--quick";
	else
		quick_flag = "";

	h = new_appaserver_parameter_file();

	if ( !h )
	{
		fprintf( stderr,
			 "%s/%s()/%d: cannot load appaserver parameter file\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( database_management_system )
		h->database_management_system = database_management_system;
	else
		h->database_management_system = "mysql";

/*
	if ( strcmp( h->database_management_system, "oracle" ) == 0 )
	{
		sprintf(	sys_string,
				"sqlplus.sh %s '%c'",
				h->oracle_uidpwd,
				delimiter );
		results = system( sys_string );
	}
	else
*/
	if ( timlib_strcmp( h->database_management_system, "mysql" ) == 0 )
	{
		char *database_connection;

		if ( h->MYSQL_HOST ) 
			environ_set_environment(
				"MYSQL_HOST", h->MYSQL_HOST );

		if ( h->MYSQL_TCP_PORT ) 
			environ_set_environment(
				"MYSQL_TCP_PORT", h->MYSQL_TCP_PORT );
		
		if ( h->MYSQL_PWD ) 
			environ_set_environment( "MYSQL_PWD", h->MYSQL_PWD );

		
		if ( override_database && *override_database )
		{
			database_connection = override_database;
		}
		else
		{
			database_connection =
				environ_get_environment(
				APPASERVER_DATABASE_ENVIRONMENT_VARIABLE );
	
			if ( !database_connection
			||   !*database_connection )
			{
				database_connection =
					environ_get_environment( "DATABASE" );
			}
	
			if ( !database_connection
			||   !*database_connection )
			{
				database_connection =
					h->default_database_connection;
			}
		}

		if ( h->mysql_password_syntax )
		{
			sprintf( sys_string,
"mysql --defaults-extra-file=%s %s -u%s %s %s				|"
"tr '\011' '%c'								|"
"sed 's/NULL//g'							|"
"cat									 ",
		 	h->parameter_file_full_path,
		 	h->flags,
		 	h->user,
		 	quick_flag,
		 	database_connection,
		 	delimiter );
		}
		else
		{
			sprintf( sys_string,
"(							 "
"	echo \"connect %s;\"				;"
"	cat -						 "
") 							|"
"mysql %s -p%s -u%s %s					|"
"tr '\011' '%c'						|"
"sed 's/NULL//g'					|"
"cat							 ",
		 	database_connection,
		 	h->flags,
		 	h->password,
		 	h->user,
		 	quick_flag,
		 	delimiter );
		}

		results = system( sys_string );
	}
	else
	{
		fprintf(stderr,
		"ERROR in %s/%s(): invalid database_management_system = (%s)\n",
			__FILE__,
			__FUNCTION__,
			database_management_system );
		exit( 1 );
	}

	return 0;

} /* main() */

