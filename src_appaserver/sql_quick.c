Retired 2001-12-10
/* sql.c 								*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "sql.h"
#include "environ.h"

int main( int argc, char **argv )
{
	APPASERVER_PARAMETER_FILE *h;
	char sys_string[ 4096 ];
	char delimiter;

	if ( argc == 2 )
		delimiter = *argv[ 1 ];
	else
		delimiter = '^';

	add_dot_to_path();
	add_local_bin_to_path();

	h = new_appaserver_parameter_file();

	if ( h->MYSQL_HOST ) 
		set_environment( "MYSQL_HOST", h->MYSQL_HOST );

	if ( h->MYSQL_TCP_PORT ) 
		set_environment( "MYSQL_TCP_PORT", h->MYSQL_TCP_PORT );
		
	if ( h->MYSQL_PWD ) 
		set_environment( "MYSQL_PWD", h->MYSQL_PWD );

	sprintf( sys_string,
"(							 "
"	echo \"connect %s;\"				;"
"	cat -						 "
") 							|"
"mysql %s -p%s -u%s --quick				|"
"tr '\011' '%c'						|"
"sed 's/NULL//g'					|"
"cat							 ",
		 h->database,
		 h->flags,
		 h->password,
		 h->user,
		 delimiter );

	system( sys_string );

	exit( 0 );
} /* main() */


