/* library/appaserver_parameter_file.c					*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "environ.h"
#include "dictionary.h"
#include "appaserver_error.h"
#include "appaserver_library.h"
#include "fopen_path.h"
#include "appaserver_parameter_file.h"

static APPASERVER_PARAMETER_FILE *global_appaserver_parameter_file = {0};

char *appaserver_parameter_file_get_appaserver_mount_point( void )
{
	if ( !global_appaserver_parameter_file )
		global_appaserver_parameter_file =
			new_appaserver_parameter_file();
	return global_appaserver_parameter_file->appaserver_mount_point;
}

char *appaserver_parameter_file_get_cgi_directory( void )
{
	return appaserver_parameter_file_get_apache_cgi_directory();
}

char *appaserver_parameter_file_get_apache_cgi_directory( void )
{
	if ( !global_appaserver_parameter_file )
		global_appaserver_parameter_file =
			new_appaserver_parameter_file();
	return global_appaserver_parameter_file->apache_cgi_directory;
}

char *appaserver_parameter_file_get_appaserver_error_directory( void )
{
	if ( !global_appaserver_parameter_file )
		global_appaserver_parameter_file =
			new_appaserver_parameter_file();
	return global_appaserver_parameter_file->appaserver_error_directory;
}

char *appaserver_parameter_file_get_appaserver_data_directory( void )
{
	if ( !global_appaserver_parameter_file )
		global_appaserver_parameter_file =
			new_appaserver_parameter_file();
	return global_appaserver_parameter_file->appaserver_data_directory;
}

char *appaserver_parameter_file_get_dbms( void )
{
	return appaserver_parameter_file_get_database_management_system();
}

char *appaserver_parameter_file_get_database_management_system( void )
{
	return APPASERVER_PARAMETER_FILE_DBMS;

/* Retired
	if ( !global_appaserver_parameter_file )
		global_appaserver_parameter_file =
			new_appaserver_parameter_file();
	return global_appaserver_parameter_file->database_management_system;
*/
}

char *appaserver_parameter_file_get_database( void )
{
	if ( !global_appaserver_parameter_file )
		global_appaserver_parameter_file =
			new_appaserver_parameter_file();
	return global_appaserver_parameter_file->default_database_connection;
}

char *appaserver_parameter_file_get_cgi_home( void )
{
	if ( !global_appaserver_parameter_file )
		global_appaserver_parameter_file =
			new_appaserver_parameter_file();
	return global_appaserver_parameter_file->cgi_home;
}

char *appaserver_parameter_file_get_document_root( void )
{
	if ( !global_appaserver_parameter_file )
		global_appaserver_parameter_file =
			new_appaserver_parameter_file();
	return global_appaserver_parameter_file->document_root;
}

APPASERVER_PARAMETER_FILE *new_appaserver_parameter_file( void )
{
	return appaserver_parameter_file_new();
}

APPASERVER_PARAMETER_FILE *appaserver_parameter_default_file_new( void )
{
	char filename[ 128 ];
	APPASERVER_PARAMETER_FILE *s;
	FILE *f;

	sprintf(	filename,
			"%s/%s",
			APPASERVER_PARAMETER_DEFAULT_DIRECTORY,
			APPASERVER_PARAMETER_FILE_NAME );

	f = fopen( filename, "r" );

	if ( !f ) return (APPASERVER_PARAMETER_FILE *)0;

	s = appaserver_parameter_file_fetch( f, strdup( filename ) );

	fclose( f );

	/* ------------------------------------------------------------ */
	/* umask() is here for convenience. However, need to move it	*/
	/* to the many places where it's truly needed.			*/
	/* ------------------------------------------------------------ */
	umask( APPASERVER_UMASK );

	return s;

} /* appaserver_parameter_default_file_new() */

APPASERVER_PARAMETER_FILE *appaserver_parameter_file_new( void )
{
	char *application;

	application =
		environ_get_environment(
		APPASERVER_DATABASE_ENVIRONMENT_VARIABLE );

	if ( !application
	||   !*application )
	{
		application =
			environ_get_environment( "DATABASE" );
	}

	return appaserver_parameter_file_application( application );

} /* appaserver_parameter_file_new() */

APPASERVER_PARAMETER_FILE *appaserver_parameter_file_fetch(
					FILE *f,
					char *parameter_file_full_path )
{
	APPASERVER_PARAMETER_FILE *s;
	DICTIONARY *d;
	char *a;

	s = (APPASERVER_PARAMETER_FILE *)
		calloc( 1, sizeof( APPASERVER_PARAMETER_FILE ) );

	s->parameter_file_full_path = parameter_file_full_path;

	d = appaserver_parameter_file_load_record_dictionary( f, '=' );

	a = "default_database";
	if ( ! ( s->default_database_connection =
			dictionary_fetch( d, a ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: Cannot fetch %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 a );
		fclose( f );
		exit( 1 );
	}

	a = "mysql_user";
	if ( ! ( s->user = dictionary_fetch( d, a ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: Cannot fetch %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 a );
		fclose( f );
		exit( 1 );
	}

	a = "mysql_flags";
	if ( ! ( s->flags = dictionary_fetch( d, a ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: Cannot fetch %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 a );
		fclose( f );
		exit( 1 );
	}

	a = "mysql_password";
	if ( ! ( s->password = dictionary_fetch( d, a ) ) )
	{
		a = "password";
		if ( ! ( s->password = dictionary_fetch( d, a ) ) )
		{
			fprintf( stderr,
			 	"Error in %s/%s()/%d: Cannot fetch %s\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	a );
			fclose( f );
			exit( 1 );
		}
		s->mysql_password_syntax = 1;
	}


	a = "mysql_host";
	s->MYSQL_HOST = dictionary_fetch( d, a );

	a = "mysql_tcp_port";
	s->MYSQL_TCP_PORT = dictionary_fetch( d, a );

/*
	a = "mysql_pwd";
	s->MYSQL_PWD = dictionary_fetch( d, a );
*/

	a = "appaserver_mount_point";
	if ( ! ( s->appaserver_mount_point =
			dictionary_fetch( d, a ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: Cannot fetch %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 a );
		fclose( f );
		exit( 1 );
	}

	/* cgi home from the browser's perspective. */
	/* ---------------------------------------- */
	a = "apache_cgi_directory";
	if ( ! ( s->apache_cgi_directory = dictionary_fetch( d, a ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: Cannot fetch %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 a );
		fclose( f );
		exit( 1 );
	}


/* Retired
	a = "database_management_system";
	s->database_management_system = dictionary_fetch( d, a );
*/
	s->database_management_system = APPASERVER_PARAMETER_FILE_DBMS;

	/* cgi home from the file system's perspective. */
	/* -------------------------------------------- */
	a = "cgi_home";
	s->cgi_home = dictionary_fetch( d, a );

	a = "document_root";
	s->document_root = dictionary_fetch( d, a );

	a = "appaserver_error_directory";
	s->appaserver_error_directory = dictionary_fetch( d, a );

	a = "appaserver_data_directory";
	s->appaserver_data_directory = dictionary_fetch( d, a );

	return s;

} /* appaserver_parameter_file_fetch() */

DICTIONARY *appaserver_parameter_file_load_record_dictionary(
			FILE *input_pipe,
			int delimiter )
{
	char buffer[ 4096 ];
	char key[ 1024 ];
	char data[ 3072 ];
	char first_key[ 1024 ];
	DICTIONARY *d = (DICTIONARY *)0;
	int first_time = 1;

	while( get_line( buffer, input_pipe ) )
	{
		if ( *buffer == '[' ) continue;

		piece( key, delimiter, buffer, 0 );

		if ( first_time )
		{
			d = dictionary_new();
			strcpy( first_key, key );
			first_time = 0;
		}
		else
		{
			if ( strcmp( key, first_key ) == 0 )
			{
				break;
			}
		}
		
		if ( piece( data, delimiter, buffer, 1 ) )
		{
			dictionary_set_pointer(	d,
						strdup( key ),
						strdup( data ) );
		}
	}

	return d;

} /* appaserver_parameter_file_load_record_dictionary() */

APPASERVER_PARAMETER_FILE *appaserver_parameter_file_application(
					char *application_name )
{
	char filename[ 256 ];
	char appaserver_filename[ 128 ];
	APPASERVER_PARAMETER_FILE *s;
	FILE *f;

	/* If have read permission to /etc/appaserver.config */
	/* ------------------------------------------------- */
	if ( ( s = appaserver_parameter_default_file_new() ) )
		return s;

	if ( !application_name || !*application_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty application_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf(	appaserver_filename,
			APPASERVER_PARAMETER_APPLICATION_FILE_NAME,
			application_name );
	
	sprintf(	filename,
			"%s/%s",
			APPASERVER_PARAMETER_DEFAULT_DIRECTORY,
			appaserver_filename );
	
	if ( !( f = fopen( filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 filename );
		exit( 1 );
	}

	s = appaserver_parameter_file_fetch( f, strdup( filename ) );

	fclose( f );

	/* ------------------------------------------------------------ */
	/* umask() is here for convenience. However, need to move it	*/
	/* to the many places where it's truly needed.			*/
	/* ------------------------------------------------------------ */
	umask( APPASERVER_UMASK );

	return s;

} /* appaserver_parameter_file_application() */

