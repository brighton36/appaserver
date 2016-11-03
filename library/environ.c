/* library/environ.c					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "timlib.h"
#include "piece.h"
#include "appaserver_parameter_file.h"
#include "environ.h"

char *resolve_environment_variables( char *return_string, char *s )
{
	char variable_name[ 128 ];
	char variable_value[ 128 ];
	char looking_string[ 1024 ];
	char looking_string_old[ 1024 ];

	/* Work only with local memory */
	/* --------------------------- */
	strcpy( looking_string, s );

	while( 1 )
	{
		strcpy( looking_string_old, looking_string );

		if ( environ_variable_exists( variable_name, looking_string ) )
		{
			environ_name_to_value( variable_value, variable_name );
			search_replace(	variable_name, 
					variable_value, 
					looking_string );
		}

		/* Exit if no changes */
		/* ------------------ */
		if ( strcmp( looking_string, looking_string_old ) == 0 )
			break;
	}

	strcpy( return_string, looking_string );
	return return_string;

} /* resolve_environment_variables() */


boolean environ_variable_exists( char *variable_name, char *looking_string )
{
	int next_delimiter;

	/* Look for the '$' */
	/* ---------------- */
	while( *looking_string )
	{
		if ( *looking_string == '$' )
		{
			next_delimiter = 
				environ_next_delimiter( looking_string );

			strncpy(	variable_name, 
					looking_string, 
					next_delimiter );

			/* TRAP: strncpy does not null terminate! */
			/* -------------------------------------- */
			variable_name[ next_delimiter ] = '\0';

			return 1;
		}

		looking_string++;
	}

	*variable_name = '\0';
	return 0;

} /* environ_variable_exists() */


int environ_next_delimiter( char *looking_string )
{
	int i = 0;

	/* Skip the '$' */
	/* ------------ */
	if ( *looking_string == '$' )
		looking_string++;

	while( *looking_string )
	{
		if ( *looking_string != '_' && !isalnum( *looking_string ) )
		{

			return i + 1;
		}
		else
		{
			looking_string++;
			i++;
		}
	}

	return i + 1;

} /* environ_next_delimiter() */

char *environ_get_environment( char *variable_name )
{
	static char datum[ 1024 ];

	if ( environ_name_to_value( datum, variable_name ) )
		return datum;
	else
		return (char *)0;
} /* environ_get_environment() */

boolean environ_name_to_value( char *variable_value, char *variable_name )
{
	char *value;

	/* Skip the '$' */
	/* ------------ */
	if ( *variable_name == '$' )
		variable_name++;

 	value = getenv( variable_name );

	if ( value )
	{
		strcpy( variable_value, value );
		return 1;
	}
	else
	{
		*variable_value = '\0';
		return 0;
	}

} /* environ_name_to_value() */

void environ_set_environment( char *environment, char *datum )
{
	set_environment( environment, datum );
}

void set_environment( char *environment, char *datum )
{
	char statement[ 4096 ];

	sprintf( statement, "%s=%s", environment, datum );
	putenv( strdup( statement ) );

} /* set_environment() */

void set_path( char *path_to_add )
{
	char *path_environment;
	char new_path[ 4096 ];

	path_environment = getenv( "PATH" );
	if ( !path_environment )
	{
		fprintf( stderr, 
			 "%s/%s(%s) cannot get PATH\n", 
			 __FILE__,
			 __FUNCTION__,
			 path_to_add );
		exit( 1 );
	}
	sprintf( new_path, "PATH=%s:%s", path_environment, path_to_add );

	putenv( strdup( new_path ) );

} /* set_path() */

boolean environ_get_browser_internet_explorer( void )
{
	char http_user_agent[ 512 ];

	if ( !environ_name_to_value( http_user_agent, "HTTP_USER_AGENT" ) )
		return 0;

/*
	fprintf( stderr, "got http_user_agent = (%s)\n", http_user_agent );
*/
	return ( instr( "MSIE", http_user_agent, 1 ) != -1 );
} /* environ_get_browser_internet_explorer() */

char *environ_get_http_referer(	void )
{
	char http_referer_with_parameter[ 512 ];
	static char http_referer[ 512 ];

	if ( !environ_name_to_value(
				http_referer_with_parameter,
				"HTTP_REFERER" ) )
	{
		return (char *)0;
	}
	else
	{
		piece( http_referer, '?', http_referer_with_parameter, 0 );
		return http_referer;
	}

} /* environ_get_http_referer() */

char *environ_get_http_referer_filename( void )
{
	char http_referer_with_parameter[ 512 ];
	char http_referer_with_pathname[ 512 ];
	static char http_referer_filename[ 512 ];
	int ch_count;

	if ( !environ_name_to_value(
				http_referer_with_parameter,
				"HTTP_REFERER" ) )
	{
		return (char *)0;
	}
	else
	{
		piece(	http_referer_with_pathname,
			'?',
			http_referer_with_parameter,
			0 );

		ch_count = character_count( '/', http_referer_with_pathname );

		piece(	http_referer_filename,
			'/',
			http_referer_with_pathname,
			ch_count );

		return http_referer_filename;
	}

} /* environ_get_http_referer_filename() */

