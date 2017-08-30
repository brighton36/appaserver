/* $APPASERVER_HOME/src_appaserver/sql.c				*/
/* -------------------------------------------------------------------- */
/* This compiles to sql, sql.e, and sql_quick.e.			*/
/* To override the database_name found in				*/
/* /etc/appaserver.config, pass as parameters:				*/
/* 1) the delimiter							*/
/* 2) the database_management_system (retired)				*/
/* 3) the database_name							*/
/* --OR--								*/
/* 1) the delimiter							*/
/* 2) the database_name							*/
/* --OR--								*/
/* 1) the database_name							*/
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

int main( int argc, char **argv )
{
	APPASERVER_PARAMETER_FILE *h;
	char sys_string[ 8192 ];
	char delimiter = FOLDER_DATA_DELIMITER;
	char *base_name;
	char *quick_flag;
	char *override_database = {0};
	char *database_connection;

	/* --------------------------------------------	*/
	/* Usage: sql.e [delimiter] ignored [database]	*/
	/* --OR--					*/
	/* Usage: sql.e delimiter [database]		*/
	/* --OR--					*/
	/* Usage: sql.e [database]			*/
	/* -------------------------------------------- */

	if ( argc == 4 )
	{
		delimiter = *argv[ 1 ];
		/* database_management_system = argv[ 2 ]; */
		override_database = argv[ 3 ];
	}
	else
	if ( argc == 3 )
	{
		if ( strlen( argv[ 1 ] ) == 1 )
			delimiter = *argv[ 1 ];

		if ( *argv[ 2 ]
		&&   strcmp( argv[ 2 ], "ignored" ) != 0
		&&   strcmp( argv[ 2 ], "mysql" ) != 0 )
		{
			override_database = argv[ 2 ];
		}
	}
	else
	if ( argc == 2 )
	{
		if ( strlen( argv[ 1 ] ) == 1 )
			delimiter = *argv[ 1 ];
		else
			override_database = argv[ 1 ];
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

	if ( override_database )
	{
		h = appaserver_parameter_file_application( override_database );
	}
	else
	{
		h = appaserver_parameter_file_new();
	}

	if ( !h )
	{
		fprintf( stderr,
			 "%s/%s()/%d: cannot load appaserver parameter file\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( h->MYSQL_HOST ) 
		environ_set_environment(
			"MYSQL_HOST", h->MYSQL_HOST );

	if ( h->MYSQL_TCP_PORT ) 
		environ_set_environment(
			"MYSQL_TCP_PORT", h->MYSQL_TCP_PORT );
	
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

	system( sys_string );

	return 0;

} /* main() */

