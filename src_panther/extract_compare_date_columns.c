/* src_panther/extract_compare_date_columns.c		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "folder.h"
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "date_convert.h"
#include "appaserver_parameter_file.h"

/* Constants */
/* --------- */


/* Prototypes */
/* ---------- */
void parse_observation_date1(	char **observation_date1,
				char *input_observation_date1 );
void parse_observation_date2(	char **observation_date2,
				char *input_observation_date2 );
void parse_observation_date3(	char **observation_date3,
				char *input_observation_date3 );
void parse_observation_date4(	char **observation_date4,
				char *input_observation_date4 );

int main( int argc, char **argv )
{
	char *input_filename;
	FILE *input_file;
	char input_string[ 1024 ];
	char observation_id[ 128 ];
	char input_observation_date1[ 128 ];
	char *observation_date1;
	char input_observation_date2[ 128 ];
	char *observation_date2;
	char input_observation_date3[ 128 ];
	char *observation_date3;
	char input_observation_date4[ 128 ];
	char *observation_date4;

	if ( argc != 2 )
	{
		fprintf( stderr,
	"Usage: %s input_filename\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	input_filename = argv[ 1 ];

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf(stderr,
			"ERROR: cannot open %s for read.\n",
			input_filename );
		exit( 0 );
	}

	printf(
	"\"ID\",\"column 8\",\"column 9\",\"column 11\",\"column 12\"\n" );

	while( get_line( input_string, input_file ) )
	{
		if ( *input_string == '#' ) continue;

		trim( input_string );
		if ( !*input_string ) continue;

		piece_double_quoted(
				observation_id,
				',',
				input_string,
				0 );

		piece_double_quoted(
				input_observation_date1,
				',',
				input_string,
				7 );

		parse_observation_date1(
				&observation_date1,
				input_observation_date1 );

		piece_double_quoted(
				input_observation_date2,
				',',
				input_string,
				8 );

		parse_observation_date2(
				&observation_date2,
				input_observation_date2 );

		if ( strncmp( observation_date2, "1899", 4 ) == 0 )
			*observation_date2 = '\0';

		piece_double_quoted(
				input_observation_date3,
				',',
				input_string,
				10 );

		parse_observation_date3(
				&observation_date3,
				input_observation_date3 );

		piece_double_quoted(
				input_observation_date4,
				',',
				input_string,
				11 );

		parse_observation_date4(
				&observation_date4,
				input_observation_date4 );

		if ((strcmp( observation_date1, observation_date2 ) != 0
		&&   *observation_date2 )
		||   strcmp( observation_date1, observation_date3 ) != 0
		||   strcmp( observation_date1, observation_date4 ) != 0 )
		{
			printf( "%c%s%c,%c%s%c,%c%s%c,%c%s%c,%c%s%c\n",
				'"',
				observation_id,
				'"',
				'"',
				input_observation_date1,
				'"',
				'"',
				input_observation_date2,
				'"',
				'"',
				input_observation_date3,
				'"',
				'"',
				input_observation_date4,
				'"' );
		}
	}

	fclose( input_file );

	exit( 0 );
} /* main() */

void parse_observation_date1(
				char **observation_date1,
				char *input_observation_date1 )
{
	char local_observation_date[ 128 ];
	static char return_observation_date[ 128 ];
	char *return_observation_date_pointer;
	int str_len;

	*observation_date1 = return_observation_date;
	*return_observation_date = '\0';

	if ( !*input_observation_date1 ) return;

	piece(	local_observation_date,
		'.',
		input_observation_date1,
		0 );

	str_len = strlen( local_observation_date );

	if ( str_len != 8 ) return;

	return_observation_date_pointer = return_observation_date;

	return_observation_date_pointer +=
		sprintf(return_observation_date_pointer,
			"%.4s-",
			local_observation_date );

	return_observation_date_pointer +=
		sprintf(return_observation_date_pointer,
			"%.2s-",
			local_observation_date + 4 );

	return_observation_date_pointer +=
		sprintf(return_observation_date_pointer,
			"%s",
			local_observation_date + 6 );

} /* parse_observation_date1() */

void parse_observation_date2(
				char **observation_date2,
				char *input_observation_date2 )
{
	char local_observation_date[ 128 ];
	static char return_observation_date[ 128 ];
	DATE_CONVERT *date_convert;

	*observation_date2 = return_observation_date;
	*return_observation_date = '\0';

	if ( !*input_observation_date2 ) return;

	column(	local_observation_date,
		0,
		input_observation_date2 );

	date_convert = date_convert_new_date_convert(
				international,
				local_observation_date );
	strcpy( return_observation_date, date_convert->return_date );
	date_convert_free( date_convert );
	return;
} /* parse_observation_date2() */

void parse_observation_date3(
				char **observation_date3,
				char *input_observation_date3 )
{
	char local_observation_date[ 128 ];
	static char return_observation_date[ 128 ];
	DATE_CONVERT *date_convert;

	*observation_date3 = return_observation_date;
	*return_observation_date = '\0';

	if ( !*input_observation_date3 ) return;

	column(	local_observation_date,
		0,
		input_observation_date3 );

	date_convert = date_convert_new_date_convert(
				international,
				local_observation_date );
	strcpy( return_observation_date, date_convert->return_date );
	date_convert_free( date_convert );
	return;
} /* parse_observation_date3() */

void parse_observation_date4(
				char **observation_date4,
				char *input_observation_date4 )
{
	static char return_observation_date[ 128 ];
	DATE_CONVERT *date_convert;

	*observation_date4 = return_observation_date;
	*return_observation_date = '\0';

	if ( !*input_observation_date4 ) return;

	date_convert = date_convert_new_date_convert(
				international,
				input_observation_date4 );
	strcpy( return_observation_date, date_convert->return_date );
	date_convert_free( date_convert );
	return;
} /* parse_observation_date4() */

