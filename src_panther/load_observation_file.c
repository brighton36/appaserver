/* src_panther/load_observation_file.c			   */
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
#include "date_convert.h"
#include "appaserver_parameter_file.h"

/* Constants */
/* --------- */
/* #define DO_INSERT_PANTHER 1 */
/* #define DO_INSERT_COLLAR 1 */
/* #define DO_INSERT_PANTHER_COLLAR 1 */
/* #define DO_INSERT_RESEARCHER_OBSERVATION 1 */
#define DO_INSERT_OBSERVATION 1

#define INSERT_PANTHER			"panther_number"
#define INSERT_COLLAR			"collar_frequency_mhz"
#define INSERT_PANTHER_COLLAR		"panther_number,collar_frequency_mhz,installation_date"
#define INSERT_RESEARCHER_OBSERVATION	"researcher,panther_number,observation_date,observation_time"
#define INSERT_OBSERVATION		"panther_number,observation_date,observation_time,collar_frequency_mhz,utm_easting,utm_northing,primary_habitat,secondary_habitat,observation_description"

/* Prototypes */
/* ---------- */
void insert_panther_statements(	char *application_name,
				char *input_filename );

/*
char *get_activity(
				char *application_name,
				char *activity_code );
*/

void get_observation_date_time(	char **observation_date,
				char **observation_time,
				char *input_string );

void load_observation_file(	char *application_name,
				char *input_filename );

char *get_habitat(		char *application_name,
				char *habitat_code );

char *get_researcher(
				char *application_name,
				char *researcher_code );

int main( int argc, char **argv )
{
	char *application_name;
	char *input_filename;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 3 )
	{
		fprintf( stderr,
	"Usage: %s application input_filename\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	input_filename = argv[ 2 ];

	load_observation_file( application_name, input_filename );

	exit( 0 );
} /* main() */

void load_observation_file(	char *application_name,
				char *input_filename )
{
	char sys_string[ 1024 ];
	FILE *input_file;
	char input_string[ 1024 ];
	char *table_name;
	FILE *researcher_observation_insert_pipe = {0};
	FILE *panther_insert_pipe = {0};
	FILE *panther_collar_insert_pipe = {0};
	FILE *collar_insert_pipe = {0};
	FILE *observation_insert_pipe = {0};
	char utm_easting_string[ 128 ];
	char utm_northing_string[ 128 ];
	int utm_easting;
	int utm_northing;
	char collar_frequency_compressed[ 128 ];
	char collar_frequency_expanded[ 128 ];
	char *observation_date;
	char *observation_time;
	char observation_description[ 1024 ];
	char panther_number[ 128 ];
	char observation_number[ 128 ];
	char *habitat_pointer;
	char habitat_code[ 128 ];
	char secondary_habitat_code[ 128 ];
	char primary_habitat[ 128 ];
	char secondary_habitat[ 128 ];
	char researcher_code[ 128 ];
	char *researcher;
	char researcher_code_second[ 128 ];
	char *researcher_second;
	int str_len;
/*
	char activity_code[ 128 ];
	char *activity;
*/

	/* Open the input file */
	/* ------------------- */
	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf(stderr,
			"ERROR: cannot open %s for read.\n",
			input_filename );
		exit( 0 );
	}

	/* Open the panther insert pipe */
	/* ---------------------------- */
	table_name =
		get_table_name(	application_name,
				"panther" );

	sprintf( sys_string,
"sort -u | insert_statement.e table=%s field=%s delimiter='|'",
	 	table_name,
	 	INSERT_PANTHER );

	panther_insert_pipe = popen( sys_string, "w" );

#ifdef DO_INSERT_PANTHER
	printf( "delete from %s;\n", table_name );
	fflush( stdout );
#endif

	/* Open the collar insert pipe */
	/* --------------------------- */
	table_name =
		get_table_name(	application_name,
				"collar" );

	sprintf( sys_string,
"sort -u | insert_statement.e table=%s field=%s delimiter='|'",
	 	table_name,
	 	INSERT_COLLAR );

	collar_insert_pipe = popen( sys_string, "w" );

#ifdef DO_INSERT_COLLAR
	printf( "delete from %s;\n", table_name );
	printf( "insert into %s values ( 'null' );\n", table_name );
	fflush( stdout );
#endif

	/* Open the panther_collar insert pipe */
	/* ----------------------------------- */
	table_name =
		get_table_name(	application_name,
				"panther_collar" );

	/* Don't do sort -u so the installation date is accepted. */
	/* ------------------------------------------------------ */
	sprintf( sys_string,
"insert_statement.e table=%s field=%s delimiter='|'",
	 	table_name,
	 	INSERT_PANTHER_COLLAR );

	panther_collar_insert_pipe = popen( sys_string, "w" );

#ifdef DO_INSERT_PANTHER_COLLAR
	printf( "delete from %s;\n", table_name );
	fflush( stdout );
#endif

	/* Open the researcher_observation insert pipe */
	/* ------------------------------------------- */
	table_name =
		get_table_name(	application_name,
				"researcher_observation" );
	
	sprintf( sys_string,
"sort -u | insert_statement.e %s %s '|'",
		 	table_name,
		 	INSERT_RESEARCHER_OBSERVATION );

	researcher_observation_insert_pipe = popen( sys_string, "w" );

#ifdef DO_INSERT_RESEARCHER_OBSERVATION
	printf( "delete from %s;\n", table_name );
	fflush( stdout );
#endif

	/* Open the observation insert pipe */
	/* -------------------------------- */
	table_name =
		get_table_name(	application_name,
				"observation" );
	
	sprintf( sys_string,
"insert_statement.e %s %s '|'",
		 	table_name,
		 	INSERT_OBSERVATION );

	observation_insert_pipe = popen( sys_string, "w" );

#ifdef DO_INSERT_OBSERVATION
	printf( "delete from %s;\n", table_name );
	fflush( stdout );
#endif

	while( get_line( input_string, input_file ) )
	{
		if ( *input_string == '#' ) continue;
		trim( input_string );
		if ( !*input_string ) continue;

		piece_double_quoted(
				observation_number,
				',',
				input_string,
				0 );

/* if ( strcmp( observation_number, "15069" ) != 0 ) continue; */

		if ( !piece_double_quoted(
				panther_number,
				',',
				input_string,
				1 ) )
		{
			fprintf( stderr,
				 "Warning: cannot get panther number in %s\n",
				 input_string );
			continue;
		}

		if ( !*panther_number ) continue;

		if ( !piece_double_quoted(
				collar_frequency_compressed,
				',',
				input_string,
				2 ) )
		{
			fprintf( stderr,
				 "Warning: cannot get panther number in %s\n",
				 input_string );
			continue;
		}

		str_len = strlen( collar_frequency_compressed );

		if ( str_len && str_len != 6 )
		{
			fprintf(stderr,
				"Warning: frequency %d characters in %s\n",
				str_len,
				input_string );
			*collar_frequency_expanded = '\0';
		}
		else
		{
			strncpy(	collar_frequency_expanded,
					collar_frequency_compressed,
					3 );
			sprintf( collar_frequency_expanded + 3,
			 	".%s",
			 	collar_frequency_compressed + 3 );
		}

		if ( !*collar_frequency_expanded )
		{
			timlib_strcpy(	collar_frequency_expanded,
					"null" ,
					128 );
		}

		if ( !piece_double_quoted(
				utm_easting_string,
				',',
				input_string,
				3 ) )
		{
			fprintf( stderr,
				 "Warning: cannot get UTM Easting in %s\n",
				 input_string );
			continue;
		}
		utm_easting = atoi( utm_easting_string );

		if ( !piece_double_quoted(
				utm_northing_string,
				',',
				input_string,
				4 ) )
		{
			fprintf( stderr,
				 "Warning: cannot get UTM Northing in %s\n",
				 input_string );
			continue;
		}
		utm_northing = atoi( utm_northing_string );

		get_observation_date_time(
				&observation_date,
				&observation_time,
				input_string );

/*
if ( strcmp( panther_number, "TX105" ) != 0
||   strcmp( observation_date, "2000-04-19" ) != 0
||   strcmp( observation_time, "1155" ) != 0 )
{
	continue;
}
*/

		if ( !*observation_date )
		{
			fprintf( stderr,
				 "Warning: cannot get observation date in %s\n",
				 input_string );
			continue;
		}

/*
		if ( !piece_double_quoted(
				activity_code,
				',',
				input_string,
				5 ) )
		{
			fprintf( stderr,
				 "Warning: cannot get activity_code in %s\n",
				 input_string );
			continue;
		}

		if ( ! ( activity = get_activity(
					application_name,
					activity_code ) ) )
		{
			fprintf( stderr,
"Ignoring: cannot get activity for code = (%s) in %s. Ignoring activity column.\n",
				 activity_code,
				 input_string );
			activity = "";
		}
*/

		if ( !piece_double_quoted(
				habitat_code,
				',',
				input_string,
				5 ) )
		{
			fprintf( stderr,
				 "Warning: cannot get habitat_code in %s\n",
				 input_string );
			continue;
		}

		*primary_habitat = '\0';
		*secondary_habitat = '\0';
		*secondary_habitat_code = '\0';

		if ( timlib_character_exists( habitat_code, '/' ) )
		{
			char buffer[ 128 ];

			strcpy( buffer, habitat_code );

			piece( secondary_habitat_code, '/', buffer, 1 );
			piece( habitat_code, '/', buffer, 0 );
		}

		if ( ! ( habitat_pointer =
				get_habitat(
					application_name,
					habitat_code ) ) )
		{
			fprintf( stderr,
"Ignoring: cannot get habitat for code = (%s) in %s. Ignoring habitat column.\n",
				 habitat_code,
				 input_string );
		}
		else
		{
			strcpy( primary_habitat, habitat_pointer );
		}

		if ( *secondary_habitat_code )
		{
			if ( ! ( habitat_pointer =
				get_habitat(
					application_name,
					secondary_habitat_code ) ) )
			{
				fprintf( stderr,
"Ignoring: cannot get habitat for code = (%s) in %s. Ignoring habitat column.\n",
				 	secondary_habitat_code,
				 	input_string );
			}
			else
			{
				strcpy( secondary_habitat, habitat_pointer );
			}
		}

		if ( !piece_double_quoted(
				researcher_code,
				',',
				input_string,
				11 ) )
		{
			fprintf( stderr,
				 "Warning: cannot get researcher_code in %s\n",
				 input_string );
			continue;
		}

		if ( timlib_character_exists( researcher_code, '/' ) )
		{
			char buffer[ 128 ];

			strcpy( buffer, researcher_code );

			piece(	researcher_code_second, '/', buffer, 1 );
			piece( researcher_code, '/', buffer, 0 );
		}
		else
		{
			*researcher_code_second = '\0';
		}

		if ( ! ( researcher = get_researcher(
					application_name,
					researcher_code ) ) )
		{
			fprintf( stderr,
"Ignoring: cannot get researcher for code = (%s) in %s. Ignoring researcher.\n",
				 researcher_code,
				 input_string );
			researcher = "";
		}

		if ( *researcher_code_second )
		{
			if ( ! ( researcher_second =
					get_researcher(
						application_name,
						researcher_code_second ) ) )
			{
				fprintf( stderr,
"Ignoring: cannot get researcher for code = (%s) in %s. Ignoring researcher.\n",
				 	researcher_code_second,
				 	input_string );
				researcher_second = "";
			}
		}
		else
		{
			researcher_second = "";
		}

		if ( !piece_double_quoted(
				observation_description,
				',',
				input_string,
				12 ) )
		{
			fprintf( stderr,
			"Warning: cannot get observation_description in %s\n",
				 input_string );
			continue;
		}

#ifdef DO_INSERT_PANTHER
		fprintf( panther_insert_pipe,
			 "%s\n",
			 panther_number );
		fflush( panther_insert_pipe );
#endif

#ifdef DO_INSERT_OBSERVATION
		fprintf( observation_insert_pipe,
			 "%s|%s|%s|%s|%d|%d|%s|%s|%s\n",
			 panther_number,
			 observation_date,
			 observation_time,
			 collar_frequency_expanded,
			 utm_easting,
			 utm_northing,
			 primary_habitat,
			 secondary_habitat,
			 observation_description );
		fflush( observation_insert_pipe );
#endif


#ifdef DO_INSERT_RESEARCHER_OBSERVATION
		if ( *researcher )
		{
			fprintf(researcher_observation_insert_pipe,
			 	"%s|%s|%s|%s\n",
				researcher,
			 	panther_number,
			 	observation_date,
				observation_time );
			fflush( researcher_observation_insert_pipe );
		}

		if ( *researcher_second )
		{
			fprintf(researcher_observation_insert_pipe,
			 	"%s|%s|%s|%s\n",
				researcher_second,
			 	panther_number,
			 	observation_date,
				observation_time );
			fflush( researcher_observation_insert_pipe );
		}
#endif

#ifdef DO_INSERT_COLLAR
		if ( *collar_frequency_expanded )
		{
			fprintf(collar_insert_pipe,
			 	"%s\n",
			 	collar_frequency_expanded );
			fflush( collar_insert_pipe );
		}
#endif

#ifdef DO_INSERT_PANTHER_COLLAR
		if ( *collar_frequency_expanded )
		{
			fprintf(panther_collar_insert_pipe,
			 	"%s|%s|%s\n",
			 	panther_number,
			 	collar_frequency_expanded,
				observation_date );
			fflush( panther_collar_insert_pipe );
		}
#endif
	}

	fclose( input_file );

	pclose( panther_insert_pipe );
	pclose( collar_insert_pipe );
	pclose( panther_collar_insert_pipe );
	pclose( observation_insert_pipe );
	pclose( researcher_observation_insert_pipe );

} /* load_observation_file() */

char *get_habitat(
				char *application_name,
				char *habitat_code )
{
	static LIST *habitat_record_list = {0};
	static char habitat[ 128 ];
	char local_habitat_code[ 16 ];
	char *record;

	if ( !*habitat_code ) return "";

	if ( !habitat_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "habitat_code,habitat";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=habitat",
		application_name,
		select );

		habitat_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( habitat_record_list ) )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: empty habitat_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (char *)0;
	}

	do {
		record = list_get_pointer( habitat_record_list );

		piece(	local_habitat_code,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		if ( strcasecmp(local_habitat_code,
				habitat_code ) == 0 )
		{
			piece(	habitat,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			return habitat;
		}

	} while( list_next( habitat_record_list ) );

	return (char *)0;

} /* get_habitat() */

char *get_researcher(
				char *application_name,
				char *researcher_code )
{
	static LIST *researcher_record_list = {0};
	static char researcher[ 128 ];
	char local_researcher_code[ 16 ];
	char *record;

	if ( !*researcher_code ) return "";

	if ( !researcher_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "researcher_code,researcher";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=researcher",
		application_name,
		select );

		researcher_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( researcher_record_list ) )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: empty researcher_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (char *)0;
	}

	do {
		record = list_get_pointer( researcher_record_list );

		piece(	local_researcher_code,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		if ( strcasecmp(local_researcher_code,
				researcher_code ) == 0 )
		{
			piece(	researcher,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			return researcher;
		}

	} while( list_next( researcher_record_list ) );

	return (char *)0;

} /* get_researcher() */

void get_observation_date_time(	char **observation_date,
				char **observation_time,
				char *input_string )
{
	char observation_date_time[ 128 ];
	char local_observation_date[ 128 ];
	char observation_date_american[ 128 ];
	static char return_observation_date[ 128 ];
	char *return_observation_date_pointer;
	static char return_observation_time[ 128 ];
	int str_len;

	*observation_date = return_observation_date;
	*observation_time = return_observation_time;

	*return_observation_date = '\0';
	*return_observation_time = '\0';

	if ( !piece_double_quoted(
			observation_date_time,
			',',
			input_string,
			6 ) )
	{
		return;
	}

	if ( !*observation_date_time
	||   strncmp( observation_date_time, "20001040", 8 ) == 0
	||   strncmp( observation_date_time, "20001041", 8 ) == 0
	||   strncmp( observation_date_time, "20001042", 8 ) == 0
	||   strncmp( observation_date_time, "20001043", 8 ) == 0 )
	{
		DATE_CONVERT *date_convert;

		if ( !piece_double_quoted(
				observation_date_american,
				',',
				input_string,
				10 ) )
		{
			return;
		}

		if ( !*observation_date_american )
		{
			piece_double_quoted(
					observation_date_american,
					',',
					input_string,
					10 );
			if ( !*observation_date_american )
			{
				return;
			}
		}

		date_convert = date_convert_new_date_convert(
					international,
					observation_date_american );
		strcpy( return_observation_date, date_convert->return_date );
		date_convert_free( date_convert );
		strcpy( return_observation_time, "0000" );
		return;
	}

	piece(	local_observation_date,
		'.',
		observation_date_time,
		0 );

	str_len = strlen( local_observation_date );

	if ( str_len != 8 )
	{
		return;
	}

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

	if ( timlib_character_exists( observation_date_time, '.' ) )
	{
		piece(	return_observation_time,
			'.',
			observation_date_time,
			1 );

		if ( strlen( return_observation_time ) != 4 )
		{
			strcpy( return_observation_time, "0000" );
		}
	}
	else
	{
		strcpy( return_observation_time, "0000" );
	}

} /* get_observation_date_time() */

char *get_activity(
				char *application_name,
				char *activity_code )
{
	static LIST *activity_record_list = {0};
	static char activity[ 128 ];
	char local_activity_code[ 16 ];
	char *record;

	if ( !*activity_code ) return "";

	if ( !activity_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "activity_code,activity";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			 "
		"			select=%s			 "
		"			folder=activity",
		application_name,
		select );

		activity_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( activity_record_list ) )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: empty activity_record_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (char *)0;
	}

	do {
		record = list_get_pointer( activity_record_list );

		piece(	local_activity_code,
			FOLDER_DATA_DELIMITER,
			record,
			0 );

		if ( strcasecmp(local_activity_code,
				activity_code ) == 0 )
		{
			piece(	activity,
				FOLDER_DATA_DELIMITER,
				record,
				1 );

			return activity;
		}

	} while( list_next( activity_record_list ) );

	return (char *)0;

} /* get_activity() */

