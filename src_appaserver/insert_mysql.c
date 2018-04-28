/* ---------------------------------------------------------------	*/
/* src_appaserver/insert_mysql.c					*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"

/* #define DEBUG_MODE		1 */

int main( int argc, char **argv )
{
	char *application_name, *folder;
	char *attribute_comma_string;
	char *carrot_delimited_row_filename;
	char buffer[ 65536 ];
	FILE *p;
	int rows_inserted;
	char *table_name;
	char *results;
	char *sql_executable;

	if ( ! ( application_name =
			environ_get_environment(
				APPASERVER_DATABASE_ENVIRONMENT_VARIABLE ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot get environment of %s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE );
		exit( 1 );
	}

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr, 
"Usage: %s ignored folder attribute_comma_list carrot_delimited_row_filename\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	folder = argv[ 2 ];
	attribute_comma_string = argv[ 3 ];
	carrot_delimited_row_filename = argv[ 4 ];

	table_name = get_table_name( application_name, folder );

	/* Count rows expected */
	/* ------------------- */
	sprintf( buffer, 
		 "wc -l %s | awk '{print $1}'",
		 carrot_delimited_row_filename );

	results = pipe2string( buffer );

	if ( !results )
		rows_inserted = 0;
	else
		rows_inserted = atoi( results );

	if ( !rows_inserted )
	{
		printf( "0\n" );
		exit( 0 );
	}

#ifdef DEBUG_MODE
	sql_executable = "html_paragraph_wrapper.e";
#else
	sql_executable = "sql.e";
#endif
	sprintf( buffer,
		 "cat %s			|"
		 "unescape_single_quotes.e	|"
		 "insert_statement.e %s %s '^'	|"
		 "sed 's/\\\\\\$/$/g'		|"
		 "%s 2>&1			 ",
		 carrot_delimited_row_filename,
		 table_name,
		 attribute_comma_string,
		 sql_executable );

	p = popen( buffer, "r" );

	/* Each line of input is a row not inserted */
	/* ---------------------------------------- */
	while( get_line( buffer, p ) )
	{
		appaserver_output_error_message(
			application_name, buffer, (char *)0 );
		fprintf( stderr, "%s\n", buffer );
		rows_inserted--;
	}

	pclose( p );
	printf( "%d\n", rows_inserted );
	exit( 0 );
} /* main() */

