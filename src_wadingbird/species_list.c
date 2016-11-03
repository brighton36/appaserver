/* src_wadingbird/species_list.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "additional_drop_down_attribute.h"
#include "appaserver_library.h"
#include "dictionary.h"
#include "folder.h"
#include "timlib.h"
#include "query.h"
#include "piece.h"
#include "attribute.h"
#include "appaserver_parameter_file.h"

/* Prototypes */
/* ---------- */
char *get_species_sys_string(
			char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *sys_string;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s application login_name\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	login_name = argv[ 2 ];

	sys_string = get_species_sys_string(
				application_name );

	system( sys_string );

	return 0;
} /* main() */

#ifdef NOT_DEFINED
char *get_species_sys_string(	char *application_name,
				char *login_name )
{
	char sys_string[ 2048 ];
	char *species_table_name;
	ADDITIONAL_DROP_DOWN_ATTRIBUTE *additional_drop_down_attribute;
	char select_clause[ 1024 ];
	char *constant_select = "common_name";
	char *first_attribute_name = "";

	additional_drop_down_attribute =
		additional_drop_down_attribute_new(
			application_name,
			login_name,
			"species" );

	additional_drop_down_attribute_get_select_clause(
			select_clause,
			&first_attribute_name,
			additional_drop_down_attribute->
				application_name,
			constant_select,
			additional_drop_down_attribute->
				drop_down_folder_name,
			additional_drop_down_attribute );

	species_table_name =
		get_table_name(	application_name,
				"species" );

/*
	sprintf( sys_string,
"echo \"select %s from %s where species_code is not null;\" | sql.e | sed 's/\\^/ /1' | sed 's/\\^/ /1' | sed 's/\\^|\\^/|/'",
		 select_clause,
		 species_table_name );
*/

	sprintf( sys_string,
"echo \"select %s from %s where species_code is not null;\" | sql.e | sed 's/\\^|\\^/^/'",
		 select_clause,
		 species_table_name );

	return strdup( sys_string );

} /* get_species_sys_string() */
#endif

char *get_species_sys_string(	char *application_name )
{
	char sys_string[ 2048 ];
	char *species_table_name;
	char *constant_select = "common_name";

	species_table_name =
		get_table_name(	application_name,
				"species" );

	sprintf( sys_string,
"echo \"select %s from %s where species_code is not null;\" | sql.e",
		 constant_select,
		 species_table_name );

	return strdup( sys_string );

} /* get_species_sys_string() */
