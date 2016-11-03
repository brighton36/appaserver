/* --------------------------------------------------- 	*/
/* calculate_hydroperiod.c			      	*/
/* --------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "boolean.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
enum stage_relative_elevation { below, above };

/* Prototypes */
/* ---------- */
char *get_stage_relative_elevation_string(
			enum stage_relative_elevation );

void output_chart_data(	double stage,
			double elevation,
			char *measurement_date );

void output_transmit_data(
			double stage,
			double elevation,
			char *measurement_date,
			char *previous_measurement_date,
			boolean last_one );

int main( int argc, char **argv )
{
	char *application_name;
	char *begin_date_string, *end_date_string;
	char *station;
	char *datatype;
	char *output_medium;
	char *database_string;
	char where_clause[ 1024 ];
	char aggregate_process[ 1024 ];
	char sys_string[ 1024 ];
	double elevation;
	char input_buffer[ 64 ];
	char measurement_date[ 16 ];
	char previous_measurement_date[ 16 ];
	char stage_string[ 32 ];
	double stage = 0.0;
	FILE *input_pipe;
	int did_any = 0;

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s application station datatype begin_date end_date elevation output_medium\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	station = argv[ 2 ];
	datatype = argv[ 3 ];
	begin_date_string = argv[ 4 ];
	end_date_string = argv[ 5 ];
	elevation = atof( argv[ 6 ] );
	output_medium = argv[ 7 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	sprintf( where_clause,
 	"station = '%s' and 				      "
 	"datatype = '%s' and 				      "
 	"measurement_date >= '%s' and measurement_date <= '%s'",
		station,
		datatype,
		begin_date_string,
		end_date_string );

	sprintf( aggregate_process, 
	 "real_time2aggregate_value.e average %d %d %d '%c' daily n %s",
	 		0 /* date_piece */,
	 		-1 /* time_piece */,
	 		1 /* value_piece */,
			FOLDER_DATA_DELIMITER,
			end_date_string );

	sprintf( sys_string,
	"get_folder_data	application=%s		    "
	"			folder=measurement	    "
	"			select=\"%s\"		    "
	"			where=\"%s\"		   |"
	"%s						   |"
	"cat						    ",
		application_name,
		"measurement_date,measurement_value",
		where_clause,
		aggregate_process );

	input_pipe = popen( sys_string, "r" );;
	*previous_measurement_date = '\0';
	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	measurement_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );
		piece(	stage_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );
		stage = atof( stage_string );

		if ( strcmp( output_medium, "chart" ) == 0 )
		{
			output_chart_data(	stage,
						elevation,
						measurement_date );
		}
		else
		if ( strcmp( output_medium, "transmit" ) == 0
		||   strcmp( output_medium, "text_file" ) == 0
		||   strcmp( output_medium, "spreadsheet" ) == 0
		||   strcmp( output_medium, "table" ) == 0
		||   strcmp( output_medium, "stdout" ) == 0 )
		{
			if ( !*previous_measurement_date )
			{
				strcpy(	previous_measurement_date,
					measurement_date );
			}

			output_transmit_data(	stage,
						elevation,
						measurement_date,
						previous_measurement_date,
						0 /* not last_one */ );
			strcpy(	previous_measurement_date,
				measurement_date );
		}
		did_any = 1;
	}
	pclose( input_pipe );

	if ( ( strcmp( output_medium, "transmit" ) == 0
	||     strcmp( output_medium, "text_file" ) == 0
	||     strcmp( output_medium, "spreadsheet" ) == 0
	||     strcmp( output_medium, "table" ) == 0
	||     strcmp( output_medium, "stdout" ) == 0 )
	&&   did_any )
	{
		output_transmit_data(	stage,
					elevation,
					measurement_date,
					previous_measurement_date,
					1 /* last_one */ );
	}
	return 0;
} /* main() */

char *get_stage_relative_elevation_string(
			enum stage_relative_elevation stage_relative_elevation )
{
	if ( stage_relative_elevation == below )
		return "below";
	else
		return "above";
}

void output_chart_data(	double stage,
			double elevation,
			char *measurement_date )
{
	if ( double_virtually_same( stage, elevation ) )
		printf( "%s^0\n", measurement_date );
	else
	if ( stage < elevation )
		printf( "%s^-1\n", measurement_date );
	else
		printf( "%s^1\n", measurement_date );
} /* output_chart_data() */

void output_transmit_data(	double stage,
				double elevation,
				char *measurement_date,
				char *previous_measurement_date,
				boolean last_one )
{
	static enum stage_relative_elevation stage_relative_elevation = 0;
	static enum stage_relative_elevation
				previous_stage_relative_elevation = 0;
	static boolean first_time = 1;
	static int number_of_days = 0; 

	if ( stage < elevation )
		stage_relative_elevation = below;
	else
		stage_relative_elevation = above;

	if ( first_time )
	{
		number_of_days = 1;
		first_time = 0;
		printf( "%s^", measurement_date );
	}
	else
	if ( last_one
	|| ( previous_stage_relative_elevation != stage_relative_elevation ) )
	{
		printf( "%d^%s^%s\n",
			number_of_days,
			get_stage_relative_elevation_string(
				previous_stage_relative_elevation ),
			previous_measurement_date );
		number_of_days = 1;
		if ( !last_one ) printf( "%s^", measurement_date );
	}
	else
	{
		number_of_days++;
	}
	previous_stage_relative_elevation = stage_relative_elevation;
} /* output_transmit_data() */

