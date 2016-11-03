/* attribute_list.c						*/
/* ------------------------------------------------------------ */
/* The purpose of this special process is to isolate 		*/
/* the attribute list for the insert relation drop down to	*/
/* be only those attributes (related_attribute) assigned to	*/
/* that folder.							*/
/* ------------------------------------------------------------ */
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "dictionary.h"
#include "folder.h"
#include "timlib.h"
#include "query.h"
#include "piece.h"
#include "attribute.h"
#include "boolean.h"
#include "appaserver_parameter_file.h"

/* Prototypes */
/* ---------- */
char *get_attribute_sys_string(
			char *application_name,
			char *folder_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *folder_name = {0};
	char *sys_string;
	DICTIONARY *post_dictionary = {0};

	output_starting_argv_stderr( argc, argv );

	if ( argc < 2 )
	{
		fprintf( stderr,
		"Usage: %s application [dictionary]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];

	if ( argc == 3 && strcmp( argv[ 2 ], "$dictionary" ) != 0 )
	{
		post_dictionary = 
			dictionary_string2dictionary( argv[ 2 ] );
	}

	sys_string = get_attribute_sys_string(
					application_name,
					folder_name );

	system( sys_string );

	return 0;
} /* main() */

char *get_attribute_sys_string(	char *application_name,
				char *folder_name )
{
	char sys_string[ 2048 ];
	char *attribute_table_name;

	attribute_table_name =
		get_table_name(	application_name,
				"attribute" );

	if ( folder_name && *folder_name )
	{
		char *folder_attribute_table_name;
		char where_clause[ 1024 ];
		char select_clause[ 128 ];

		folder_attribute_table_name =
			get_table_name(	application_name,
					"folder_attribute" );

		sprintf( where_clause,
			 "%s.folder = '%s' and			"
			 "%s.attribute = %s.attribute and	"
			 "%s.attribute != 'null'		",
			 folder_attribute_table_name,
			 folder_name,
			 folder_attribute_table_name,
			 attribute_table_name,
			 attribute_table_name );

		sprintf( select_clause,
			 "%s.attribute",
			 attribute_table_name );

		sprintf( sys_string,
"(echo null; echo \"select %s from %s,%s where %s;\" | sql.e) | sort",
		 	 select_clause,
			 folder_attribute_table_name,
			 attribute_table_name,
			 where_clause );
	}
	else
	{
		sprintf( sys_string,
"(echo null; echo \"select attribute from %s where attribute != 'null';\" | sql.e) | sort",
		 	attribute_table_name );
	}

	return strdup( sys_string );

} /* get_attribute_sys_string() */

