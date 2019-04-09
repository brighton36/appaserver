/* -----------------------------------------------------	*/
/* src_hydrology/post_hydrology_measurements_rectangle.c	*/
/* -----------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "date.h"
#include "folder.h"
#include "operation.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "query.h"
#include "environ.h"
#include "post2dictionary.h"
#include "dictionary_appaserver.h"
#include "application.h"
#include "session.h"
#include "station.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *login_name;
	char *application_name;
	char *session;
	char *role_name;
	DICTIONARY *post_dictionary;
	char *database_string = {0};
	char sys_string[ 1024 ];
	double rectangle_southwest_latitude;
	double rectangle_southwest_longitude;
	double rectangle_northeast_latitude;
	double rectangle_northeast_longitude;
	LIST *station_name_list;

	if ( argc != 5 )
	{
		fprintf( stderr, 
			 "Usage: %s application login_name session role\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	login_name = argv[ 2 ];
	session = argv[ 3 ];
	role_name = argv[ 4 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

/*
#include "appaserver_parameter_file.h"
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	appaserver_parameter_file = new_appaserver_parameter_file();
*/

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( !session_access(	application_name,
				session,
				login_name ) )
	{
		session_access_failed_message_and_exit(
			application_name, session, login_name );
	}

	post_dictionary =
		post2dictionary(stdin,
				(char *)0 /* appaserver_data_directory */,
				(char *)0 /* session */ );

	/* ------------------------------------------------------------ */
	/* Sample:							*/
	/* From:  "entity/application_1=appahost/appaserver"		*/
	/* To:    "entity_1=appahost and application_1=appaserver"	*/
	/* ------------------------------------------------------------ */
	dictionary_appaserver_parse_multi_attribute_keys(
		post_dictionary,
		(char *)0 /* prefix */ );

	rectangle_southwest_latitude =
		atof( dictionary_safe_fetch(
			post_dictionary,
			"rectangle_southwest_latitude" ) );

	rectangle_southwest_longitude =
		atof( dictionary_safe_fetch(
			post_dictionary,
			"rectangle_southwest_longitude" ) );

	rectangle_northeast_latitude =
		atof( dictionary_safe_fetch(
			post_dictionary,
			"rectangle_northeast_latitude" ) );

	rectangle_northeast_longitude =
		atof( dictionary_safe_fetch(
			post_dictionary,
			"rectangle_northeast_longitude" ) );

	station_name_list =
		station_get_rectangle_station_name_list(
			application_name,
			rectangle_southwest_latitude,
			rectangle_southwest_longitude,
			rectangle_northeast_latitude,
			rectangle_northeast_longitude );

	/* In case no stations, prevent a blind query. */
	/* ------------------------------------------- */
	dictionary_set_pointer( post_dictionary,
				"station_1",
				"BBOGUS_STATION" );

	if ( list_rewind( station_name_list ) )
	{
		int index = 2;
		char key[ 16 ];

		do {
			sprintf( key, "station_%d", index++ );

			dictionary_set_pointer(
				post_dictionary,
				strdup( key ),
				list_get_pointer( station_name_list ) );

		} while( list_next( station_name_list ) );
	}

	sprintf( sys_string,
"echo \"%s\" | post_prompt_process_form %s %s %s output_measurements %s n",
		 dictionary_display_delimited( post_dictionary, '&' ),
		 login_name,
		 application_name,
		 session,
		 role_name );

	if ( system( sys_string ) ) {};

	exit( 0 );

} /* main() */

