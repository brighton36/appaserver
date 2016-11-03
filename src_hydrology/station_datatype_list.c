/* $APPASERVER_HOME/src_hydrology/station_datatype_list.c */
/* ------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "dictionary.h"
#include "appaserver_library.h"

DICTIONARY *get_station_datatype_manipulate_agency_dictionary(
				char *application_name );

void parse_input_buffer(
				char *station,
				char *datatype,
				char *units,
				char *agency,
				char *input_buffer );

char *get_key(			char *station,
				char *datatype );

void station_datatype_list(	char *application,
				char *login_name,
				char *process_name,
				char *station,
				char *datatype,
				char *plot_for_station_check_yn,
				char *validation_required_yn,
				boolean dont_filter_manipulate );

int main( int argc, char **argv )
{
	char *application;
	char *login_name;
	char *process_name;
	char *station;
	char *datatype;
	char *plot_for_station_check_yn;
	char *validation_required_yn;
	char *prompt;
	boolean dont_filter_manipulate;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 9 )
	{
		fprintf(stderr,
"Usage: %s application login_name process station datatype plot_for_station_check_yn prompt validation_required_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application = argv[ 1 ];
	login_name = argv[ 2 ];
	process_name = argv[ 3 ];
	station = argv[ 4 ];
	datatype = argv[ 5 ];
	plot_for_station_check_yn = argv[ 6 ];
	prompt = argv[ 7 ];
	validation_required_yn = argv[ 8 ];

	dont_filter_manipulate =
		( strcmp(
			process_name,
			"estimation_nearest_neighbor" ) == 0
		&&
		  strcmp(
			prompt,
			"destination_station|destination_datatype" ) != 0 );

	station_datatype_list(	application,
				login_name,
				process_name,
				station,
				datatype,
				plot_for_station_check_yn,
				validation_required_yn,
				dont_filter_manipulate );
	exit( 0 );
} /* main() */

void station_datatype_list(	char *application_name,
				char *login_name,
				char *process_name,
				char *station,
				char *datatype,
				char *plot_for_station_check_yn,
				char *validation_required_yn,
				boolean dont_filter_manipulate )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	DICTIONARY *manipulate_agency_dictionary;
	char input_station[ 128 ];
	char input_datatype[ 128 ];
	char units[ 128 ];
	char agency[ 128 ];
	char *manipulate_agency;
	char *key;

	manipulate_agency_dictionary =
		get_station_datatype_manipulate_agency_dictionary(
				application_name );

	sprintf( sys_string,
"station_datatype_agency_list.sh '%s' '%s' '%s' '%s' '%s' '%s' '%c' '%s'",
		 application_name,
		 login_name,
		 process_name,
		 station,
		 datatype,
		 plot_for_station_check_yn,
		 (dont_filter_manipulate) ? 'y' : 'n',
		 validation_required_yn );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		parse_input_buffer(
			input_station,
			input_datatype,
			units,
			agency,
			input_buffer );

		key = get_key( input_station, input_datatype );

		if ( ( manipulate_agency =
				dictionary_fetch(
					manipulate_agency_dictionary,
					key ) ) )
		{
			strcpy( agency, manipulate_agency );
		}

		printf( "%s^%s[%s--%s]\n",
			input_station,
			input_datatype,
			units,
			agency );
	}
	pclose( input_pipe );

} /* station_datatype_list() */

DICTIONARY *get_station_datatype_manipulate_agency_dictionary(
				char *application_name )
{
	DICTIONARY *dictionary;
	char sys_string[ 1024 ];
	char *select;
	char *where;
	char station[ 128 ];
	char datatype[ 128 ];
	char manipulate_agency[ 128 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char *key;

	select = "station,datatype,manipulate_agency";
	where = "manipulate_agency is not null";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=station_datatype	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	dictionary = dictionary_medium_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( station, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( datatype, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		piece(	manipulate_agency,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		key = get_key( station, datatype );
		dictionary_set_pointer(
			dictionary,
			strdup( key ),
			strdup( manipulate_agency ) );
	}

	pclose( input_pipe );
	return dictionary;
} /* get_station_datatype_manipulate_agency_dictionary() */

char *get_key(	char *station,
		char *datatype )
{
	static char key[ 128 ];
	sprintf( key, "%s|%s", station, datatype );
	return key;
} /* get_key() */

void parse_input_buffer(
			char *station,
			char *datatype,
			char *units,
			char *agency,
			char *input_buffer )
{
	char piece_buffer[ 128 ];

	/* ---------------------------------------------------------- */
	/* Sample input_buffer = WB^air_temperature[celsius--NPS_ENP] */
	/* ---------------------------------------------------------- */

	piece( station, '^', input_buffer, 0 );
	piece( piece_buffer, '[', input_buffer, 0 );
	piece( datatype, '^', piece_buffer, 1 );
	piece( piece_buffer, '[', input_buffer, 1 );
	piece( units, '-', piece_buffer, 0 );
	piece( agency, '-', piece_buffer, 2 );
	*(agency + ( strlen( agency ) - 1 ) ) = '\0';

} /* parse_input_buffer() */

