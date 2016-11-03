/* date_convert_anywhere.c				*/
/* ---------------------------------------------------- */
/* This process differs from date_convert.c in that	*/
/* there is no delimiter.				*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "boolean.h"
#include "sed.h"
#include "date_convert.h"

void date_convert_anywhere(	char *source_format,
				char *destination_format,
				boolean to_date_function );

int main( int argc, char **argv )
{
	char *source_format;
	char *destination_format;
	boolean to_date_function = 0;

	if ( argc < 3 )
	{
		fprintf( stderr,
"Usage: %s source_format destination_format [to_date_function_yn]\n",
			 argv[ 0 ] );
		fprintf( stderr,
"\nNote: format may be either \"military,international,american,oracle,mysql\"\n" );
		fprintf( stderr,
"However, american format is not yet implemented.\n" );

		fprintf( stderr,
"If the destination format is oracle (or military), then you probably\n"
" want to have the date within Oracle's to_date() function.\n\n" );
		exit( 1 );
	}

	source_format = argv[ 1 ];

	if ( strcmp( source_format, "mysql" ) == 0 )
		source_format = "international";
	else
	if ( strcmp( source_format, "oracle" ) == 0 )
		source_format = "military";

	destination_format = argv[ 2 ];

	if ( strcmp( destination_format, "mysql" ) == 0 )
		destination_format = "international";
	else
	if ( strcmp( destination_format, "oracle" ) == 0 )
		destination_format = "military";

	if ( strcmp( source_format, destination_format ) == 0 )
	{
		fprintf( stderr,
	"Error in %s: source and destination formats cannot be the same.\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	if ( argc == 4 ) to_date_function = (*argv[ 3 ] == 'y');

	date_convert_anywhere(	source_format,
				destination_format,
				to_date_function );
	return 0;
} /* main() */

void date_convert_anywhere(	char *source_format,
				char *destination_format,
				boolean to_date_function )
{
	char input_buffer[ 65536 ];
	DATE_CONVERT *date_convert = {0};
	char source_date[ 16 ];
	int all_done;
	SED *sed;
	char *regular_expression;
	char date_convert_string[ 128 ];
	int source_length;
	enum date_convert_format destination_date_convert_format;

	if ( strcmp( source_format, "international" ) == 0 )
	{
		regular_expression =
			"\'[12][0-9][0-9][0-9]-[01][0-9]-[0123][0-9]\'";
		source_length = 10;
	}
	else
	if ( strcmp( source_format, "military" ) == 0 )
	{
/* JAN,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC */
/* ----------------------------------------------- */
		regular_expression =
	"\'[0-9][1-9]-[JBMASOND][AEPUCO][NBRYLGPTVC]-[12][0-9][0-9][0-9]\'";
		source_length = 11;
	}
	else
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid source format = (%s).\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			source_format );
		exit( 1 );
	}

	if ( strcmp( destination_format, "international" ) == 0 )
	{
		destination_date_convert_format = international;
	}
	else
	if ( strcmp( destination_format, "military" ) == 0 )
	{
		destination_date_convert_format = military;
	}
	else
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: invalid destination format = (%s).\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			destination_format );
		exit( 1 );
	}

	sed = sed_new( regular_expression, (char *)0 );

	while( get_line( input_buffer, stdin ) )
	{
		all_done = 0;
		while( !all_done )
		{
			all_done = 1;
			if ( sed_will_replace( input_buffer, sed ) )
			{
				strncpy(	source_date,
						input_buffer + sed->begin + 1,
						source_length );
				*(source_date + source_length) = '\0';

				if ( !date_convert )
				{
					date_convert =
					date_convert_new_date_convert( 
						destination_date_convert_format,
						source_date );
				}
				else
				{
					date_convert_populate_return_date(
					date_convert->return_date,
					date_convert->source_format,
					date_convert->destination_format,
					source_date );
				}

				if ( to_date_function )
				{
					sprintf(date_convert_string,
						"to_date('%s','DD-MON-YYYY')",
						date_convert->return_date );
						
				}
				else
				{
					sprintf(date_convert_string,
						"'%s'",
						date_convert->return_date );
				}

				sed->replace = date_convert_string;
				sed_search_replace( input_buffer, sed );
				all_done = 0;
			}

		}
		printf( "%s\n", input_buffer );
	}

	if ( date_convert ) date_convert_free( date_convert );

} /* date_convert_anywhere() */

