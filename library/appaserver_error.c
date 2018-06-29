/* library/appaserver_error.c						*/
/* --------------------------------------------------------------------	*/
/*									*/
/* These functions support outputting error messages.			*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "date.h"
#include "timlib.h"
#include "environ.h"

void appaserver_output_error_message(	char *application_name,
					char *message,
					char *login_name )
{
	FILE *f;

	f = appaserver_error_open_append_file( application_name );

	if ( login_name && *login_name )
	{
		fprintf(f,
			"%s %s %s: %s\n",
		 	date_get_now_yyyy_mm_dd( date_get_utc_offset() ),
		 	date_get_now_hhmm( date_get_utc_offset() ),
			login_name, message );
	}
	else
	{
		fprintf(f,
			"%s %s: %s\n",
		 	date_get_now_yyyy_mm_dd( date_get_utc_offset() ),
		 	date_get_now_hhmm( date_get_utc_offset() ),
		       	message );
	}

	fclose( f );

} /* appaserver_output_error_message() */

char *appaserver_get_error_filename( char *application_name )
{
	return appaserver_error_get_filename( application_name );
}

#ifdef NOT_DEFINED
char *appaserver_error_get_filename( char *application_name )
{
	static char filename[ 128 ] = {0};

	if ( !application_name )
	{
		application_name =
			environ_get_environment(
				APPASERVER_DATABASE_ENVIRONMENT_VARIABLE );
		strcpy( filename, APPASERVER_ERROR_FILE );
	}
	else
	{
		char local_application_name[ 128 ];
		char *database_string = {0};

		timlib_strcpy( local_application_name, application_name, 128 );

		timlib_parse_database_string(	&database_string,
						local_application_name );

		sprintf(
		filename,
		"%s/appaserver_%s.err",
		appaserver_parameter_file_get_appaserver_error_directory(),
		local_application_name );

		if ( !timlib_file_exists( filename ) )
		{
			strcpy( filename, APPASERVER_ERROR_FILE );
		}
	}

	return filename;
} /* appaserver_error_get_filename() */
#endif

char *appaserver_error_get_filename( char *application_name )
{
	static char filename[ 128 ] = {0};
	char local_application_name[ 128 ];
	char *database_string = {0};

	if ( !application_name )
	{
		application_name =
			environ_get_environment(
				APPASERVER_DATABASE_ENVIRONMENT_VARIABLE );
	}

	if ( application_name && *application_name )
	{
		timlib_strcpy( local_application_name, application_name, 128 );

		timlib_parse_database_string(	&database_string,
						local_application_name );

		sprintf(
		filename,
		"%s/appaserver_%s.err",
		appaserver_parameter_file_get_appaserver_error_directory(),
		local_application_name );

		if ( !timlib_file_exists( filename ) )
		{
			strcpy( filename, APPASERVER_ERROR_FILE );
		}
	}
	else
	{
		strcpy( filename, APPASERVER_ERROR_FILE );
	}

	if ( !timlib_file_exists( filename ) )
	{
		strcpy( filename, APPASERVER_OLD_ERROR_FILE );
	}

	return filename;
} /* appaserver_error_get_filename() */

void appaserver_output_starting_argv_append_file(
			int argc,
			char **argv,
			char *application_name )
{
	appaserver_error_starting_argv_append_file(
			argc,
			argv,
			application_name );
}

void appaserver_error_login_name_append_file(
					int argc,
					char **argv,
					char *application_name,
					char *login_name )
{
	FILE *f;

	if ( !login_name || !*login_name )
	{
		appaserver_error_starting_argv_append_file(
			argc,
			argv,
			application_name );
		return;
	}

	f = appaserver_error_open_append_file( application_name );

	fprintf( f, "%s %s %s: %s",
		 date_get_now_yyyy_mm_dd( date_get_utc_offset() ),
		 date_get_now_hhmm( date_get_utc_offset() ),
		 login_name,
		 argv[ 0 ] );

	while( --argc ) fprintf( f, " %s", *++argv );

	fprintf( f, "\n" );
	fclose( f );

} /* appaserver_error_login_name_append_file() */

void appaserver_error_starting_argv_append_file(
			int argc,
			char **argv,
			char *application_name )
{
	FILE *f;
	char *remote_ip_address;

	f = appaserver_error_open_append_file( application_name );

	remote_ip_address =
		environ_get_environment(
			"REMOTE_ADDR" );

	if ( remote_ip_address && *remote_ip_address )
	{
		fprintf(f,
			"%s %s %s: %s",
			remote_ip_address,
		 	date_get_now_yyyy_mm_dd( date_get_utc_offset() ),
		 	date_get_now_hhmm( date_get_utc_offset() ),
		 	argv[ 0 ] );
	}
	else
	{
		fprintf(f,
			"%s %s: %s",
		 	date_get_now_yyyy_mm_dd( date_get_utc_offset() ),
		 	date_get_now_hhmm( date_get_utc_offset() ),
		 	argv[ 0 ] );
	}

	while( --argc ) fprintf( f, " %s", *++argv );

	fprintf( f, "\n" );
	fclose( f );

} /* appaserver_error_starting_argv_append_file() */

FILE *appaserver_error_open_append_file(
			char *application_name )
{
	FILE *f;
	char *filename;

	filename = appaserver_error_get_filename( application_name );

	f = fopen( filename, "a" );
	if ( !f )
	{
		fprintf( stderr,
			 "Warning in %s()/%s/%d: cannot open %s for append.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 filename );

		filename = appaserver_error_get_filename( (char *)0 );
		f = fopen( filename, "a" );
		if ( !f )
		{
			fprintf( stderr, 
			 	"Error in %s/%s()/%d: cannot open %s.\n",
			 	__FILE__,
			 	__FUNCTION__,
				__LINE__,
			 	filename );
			exit( 1 );
		}
	}
	return f;

} /* appaserver_error_open_append_file() */

void output_error_message( char *message, char *login_name )
{
	return appaserver_output_error_message(
					(char *)0 /* application_name */,
					message,
					login_name );
} /* output_error_message() */

void m( char *message )
{
	appaserver_output_error_message( (char *)0, message, (char *)0 );
}

void m2( char *application_name, char *message )
{
	appaserver_output_error_message( application_name, message, (char *)0 );
}

void appaserver_output_starting_argv_stderr(
					int argc,
					char **argv )
{
	appaserver_error_output_starting_argv_stderr(
					argc,
					argv );
}

void appaserver_error_stderr(		int argc,
					char **argv )
{
	appaserver_error_output_starting_argv_stderr(
					argc,
					argv );
}

void appaserver_error_output_starting_argv_stderr(
					int argc,
					char **argv )
{
	fprintf( stderr, "%s %s: %s",
		 date_get_now_yyyy_mm_dd( date_get_utc_offset() ),
		 date_get_now_hhmm( date_get_utc_offset() ),
		 argv[ 0 ] );

	fprintf( stderr, "%s", *argv );
	while( --argc ) fprintf( stderr, " %s", *++argv );
	fprintf( stderr, "\n" );
	fflush( stderr );

} /* appaserver_error_output_starting_argv_stderr() */
