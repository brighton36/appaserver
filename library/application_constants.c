/* application_constants.c				*/
/* ==================================================== */
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#include "stdlib.h"
#include "string.h"
#include "application_constants.h"
#include "appaserver_parameter_file.h"
#include "appaserver_library.h"
#include "timlib.h"
#include "piece.h"

APPLICATION_CONSTANTS *application_constants_new( void )
{
	APPLICATION_CONSTANTS *a;

	a = (APPLICATION_CONSTANTS *)
		calloc( 1, sizeof( APPLICATION_CONSTANTS ) );

	return a;
} /* application_constants_new() */

DICTIONARY *application_constants_get_dictionary(
				char *application_name )
{
	char sys_string[ 1024 ];
	DICTIONARY *dictionary;
	char input_buffer[ 65536 ];
	char key[ 128 ];
	char data[ 65536 ];
	FILE *input_pipe;
	char *select = "application_constant,application_constant_value";

	dictionary = dictionary_new();

	sprintf( sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=application_constants	",
		application_name,
		select );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		piece( key, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( data, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		dictionary_set_pointer(	dictionary,
					strdup( key ),
					strdup( data ) );
	}
	pclose( input_pipe );
	return dictionary;

} /* application_constants_get_dictionary() */

char *application_constants_safe_fetch(
				DICTIONARY *application_constants_dictionary,
				char *key )
{
	char *results;

	if ( ! ( results =
			dictionary_fetch(
				application_constants_dictionary,
				key ) ) )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: cannot fetch key = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 key );
		return "";
	}
	return results;
} /* application_constants_safe_fetch() */

char *application_constants_fetch(
				DICTIONARY *application_constants_dictionary,
				char *key )
{
	return dictionary_fetch( application_constants_dictionary, key );

} /* application_constants_fetch() */

void application_constants_get_easycharts_width_height(
			int *easycharts_width,
			int *easycharts_height,
			char *application_name )
{
	APPLICATION_CONSTANTS *application_constants;
	char *results;

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	if ( ( results =
		application_constants_fetch(
			application_constants->dictionary,
			"easycharts_width" ) ) )
	{
		*easycharts_width = atoi( results );
	}
	else
	{
		*easycharts_width = 0;
	}

	if ( ( results =
		application_constants_fetch(
			application_constants->dictionary,
			"easycharts_height" ) ) )
	{
		*easycharts_height = atoi( results );
	}
	else
	{
		*easycharts_height = 0;
	}

} /* application_constants_get_easycharts_width_height() */

void application_constants_free(
			APPLICATION_CONSTANTS *application_constants )
{
	dictionary_free( application_constants->dictionary );
	free( application_constants );
} /* application_constants_free() */

char *application_constants_quick_fetch(
				char *application_name,
				char *key )
{
	APPLICATION_CONSTANTS *application_constants;
	char *data;
	char *results;

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	data = application_constants_fetch(
				application_constants->dictionary,
				key );

	if ( !data ) return (char *)0;

	results = strdup( data );

	application_constants_free( application_constants );

	return results;

} /* application_constants_quick_fetch() */

boolean application_constants_cat_javascript_source( char *application_name )
{
	char *value;

	if ( ! ( value = application_constants_quick_fetch(
					application_name,
					"cat_javascript_source" ) ) )
	{
		return 0;
	}
	else
	{
		return ( timlib_strcmp( value, "yes" ) == 0 );
	}
} /* application_constants_cat_javascript_source() */

