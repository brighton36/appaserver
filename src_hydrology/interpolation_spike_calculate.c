/* src_hydrology/interpolation_spike_calculate.c		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "basename.h"
#include "list.h"
#include "document.h"
#include "timlib.h"
#include "name_arg.h"
#include "appaserver_library.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "application.h"

/* Constants */
/* --------- */
#define VALUE_DELIMITER		','
#define VALUE_PIECE		4

/* Prototypes */
/* ---------- */
NAME_ARG *setup_named_command_line_arguments( int argc, char **argv );
void output_spike_records( LIST *list );
LIST *begin_list_buffer( char *old_buffer, char *input_buffer );

int main( int argc, char **argv )
{
	char *application_name;
	char *station;
	char *datatype;
	char *begin_date;
	char *end_date;
	char input_buffer[ 1024 ];
	char old_buffer[ 1024 ];
	char value_string[ 1024 ];
	double value;
	double save_value = 0.0;
	double old_value = 0.0;
	double minimum_spike;
	double results;
	int maximum_out_of_range;
	int number_out_of_range = 0;
	int inside_of_spike = 0;
	NAME_ARG *name_arg;
	int first_time = 1;
	LIST *list = {0};
	char sys_string[ 1024 ];
	FILE *input_pipe;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	output_starting_argv_stderr( argc, argv );

	name_arg = setup_named_command_line_arguments( argc, argv );

	/* application_name = fetch_arg( name_arg, "application" ); */
	/* login_name = fetch_arg( name_arg, "login_name" ); */
	begin_date = fetch_arg( name_arg, "begin_date" );
	end_date = fetch_arg( name_arg, "end_date" );
	station = fetch_arg( name_arg, "station" );
	datatype = fetch_arg( name_arg, "datatype" );

	minimum_spike = atof( fetch_arg( name_arg, "minimum_spike" ) );

	maximum_out_of_range = 
		atoi( fetch_arg( name_arg, "maximum_out_of_range" ) );

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					appaserver_parameter_file->
						database_management_system,
					(PROCESS_GENERIC_OUTPUT *)0,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	sprintf( sys_string,
		 "extract_measurements.sh %s '%s' '%s' %s %s",
		 application_name,
		 station,
		 datatype,
		 begin_date,
		 end_date );

	input_pipe = popen( sys_string, "r" );

	/* Sample input: "BA,rain,1999-10-27,2350,25.600000" */
	/* ------------------------------------------------- */
	while( get_line( input_buffer, input_pipe ) )
	{
		if ( !piece(	value_string, 
				VALUE_DELIMITER, 
				input_buffer, 
				VALUE_PIECE ) )
		{
			fprintf( stderr, 
				 "Cannot piece the value in (%s)\n",
				 input_buffer );
			continue;
		}

		value = atof( value_string );

		if ( first_time )
		{
			strcpy( old_buffer, input_buffer );
			old_value = value;
			first_time = 0;
			printf( "%s\n", input_buffer );
		}
		else
		if ( inside_of_spike )
		{
			results = abs_float( value - save_value );

			/* If back on track */
			/* ---------------- */
			if ( results <= minimum_spike )
			{
				list_append_string( list, input_buffer );

				list_interpolate_string_record(	
							list,
							VALUE_DELIMITER );

				output_spike_records( list );

				printf( "%s\n", input_buffer );

				inside_of_spike = 0;
				old_value = value;
			}
			else
			/* ------------------ */
			/* See if reset range */
			/* ------------------ */
			{
				if ( ++number_out_of_range == 
				       maximum_out_of_range )
				{
					/* Reset */
					/* ----- */
					list = begin_list_buffer( 
								old_buffer, 
								input_buffer );
					inside_of_spike = 0;
					number_out_of_range = 0;
					save_value = old_value = value;
				}
				else
				{
					/* ---------- */
					/* Same spike */
					/* ---------- */
					list_append_string( list, input_buffer);
				}
			}
		}
		else
		/* ------------------ */
		/* Not inside a spike */
		/* ------------------ */
		{
			results = abs_float( value - old_value );

			/* If new spike */
			/* ------------ */
			if ( results > minimum_spike )
			{
				inside_of_spike = 1;
				save_value = old_value;

				list = begin_list_buffer( old_buffer, 
							  input_buffer );
			}
			else
			/* --------------------- */
			/* Everything's still OK */
			/* --------------------- */
			{
				old_value = value;
				strcpy( old_buffer, input_buffer );
				printf( "%s\n", input_buffer );
			}
		} /* if not inside spike */

	} /* while( get_line() ) */

	return 0;

} /* main() */

LIST *begin_list_buffer( char *old_buffer, char *buffer )
{
	LIST *l = list_new();
	list_append_string( l, old_buffer );
	list_append_string( l, buffer );
	return l;
}

void output_spike_records( LIST *list )
{
	/* Skip the first and the last */
	/* --------------------------- */
	if ( list_rewind( list ) && list_length( list ) > 1 )
	{
		list_next( list );
		do {
			if ( !at_end( list ) )
				printf( "%s\n", list_get_string( list ) );
		} while( list_next( list ) );
	}
} /* output_spike_records() */

NAME_ARG *setup_named_command_line_arguments( int argc, char **argv )
{
	int ticket;
        NAME_ARG *arg = init_arg( argv[ 0 ] );

        ticket = add_valid_option( arg, "application" );
        set_default_value( arg, ticket,"");

        ticket = add_valid_option( arg, "login_name" );
        set_default_value( arg, ticket,"");

        ticket = add_valid_option( arg, "maximum_out_of_range" );
        ticket = add_valid_option( arg, "minimum_spike" );
        ticket = add_valid_option( arg, "begin_date" );
        ticket = add_valid_option( arg, "end_date" );
        ticket = add_valid_option( arg, "datatype" );
        ticket = add_valid_option( arg, "station" );

        ins_all( arg, argc, argv );

	return arg;
} /* setup_named_command_line_arguments() */


