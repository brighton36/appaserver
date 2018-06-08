/* $APPASERVER_HOME/utility/pad_missing_times.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "aggregate_level.h"

/* Prototypes */
/* ---------- */
void output_null_value(		char delimiter,
				char *ticker_date_string,
				char *ticker_time_string,
				enum aggregate_level aggregate_level,
				int date_offset,
				int time_offset,
				int value_offset,
				char *append_string );

int main( int argc, char **argv )
{
	char input_buffer[ 1024 ];
	char input_date_string[ 16 ];
	char input_time_string[ 16 ];
	char input_date_time_string[ 32 ];
	char delimiter;
	enum aggregate_level aggregate_level;
	char *starting_date_string;
	char *starting_time_string;
	char *ending_date_string;
	char *ending_time_string;
	char *expected_count_list_string;
	int date_offset;
	int time_offset;
	int value_offset;
	FILE *ticker_date_time_pipe;
	char sys_string[ 1024 ];
	char ticker_date_time_string[ 128 ];
	DATE *ticker_date = date_calloc();
	DATE *input_date = date_calloc();
	char ticker_date_buffer[ 16 ];
	char ticker_time_buffer[ 16 ];
	char time_offset_buffer[ 16 ];
	char value_offset_buffer[ 16 ];
	int number_delimiters2append;
	char append_string[ 256 ];
	char *ptr;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 10 )
	{
		fprintf( stderr,
"Usage: %s delimiter date_offset,time_offset,value_offset aggregate_level starting_date starting_time ending_date ending_time number_delimiters2append expected_count_list_string\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	date_offset = atoi( argv[ 2 ] );

	if ( !piece( time_offset_buffer, ',', argv[ 2 ], 1 )
	||   !piece( value_offset_buffer, ',', argv[ 2 ], 2 ) )
	{
		fprintf(	stderr,
"Error in %s: [date_offset,time_offset,value_offset] are all required.\n",
				argv[ 0 ] );
	}

	time_offset = atoi( time_offset_buffer );
	value_offset = atoi( value_offset_buffer );

	aggregate_level = aggregate_level_get_aggregate_level( argv[ 3 ] );
	starting_date_string = argv[ 4 ];
	starting_time_string = argv[ 5 ];
	ending_date_string = argv[ 6 ];
	ending_time_string = argv[ 7 ];
	number_delimiters2append = atoi( argv[ 8 ] );

	ptr = append_string;
	while( number_delimiters2append-- )
		*ptr++ = delimiter;
	*ptr = '\0';

	expected_count_list_string = argv[ 9 ];

	sprintf(sys_string,
		"time_ticker.e '^' %s %s:%s %s:%s '%s'",
		aggregate_level_get_string( aggregate_level ),
		starting_date_string,
		starting_time_string,
		ending_date_string,
		ending_time_string,
		expected_count_list_string );

	ticker_date_time_pipe = popen( sys_string, "r" );

	timlib_reset_line_queue();

	while( get_line( input_buffer, stdin ) )
	{
		if ( ! piece(
				input_date_string,
				delimiter,
				input_buffer,
				date_offset ) )
		{
			fprintf(stderr,
				"Warning in %s/%d: cannot get date from (%s)\n",
		 		argv[ 0 ],
		 		__LINE__,
		 		input_buffer );
			continue;
		}

		if ( time_offset != -1 )
		{
			if ( !piece(	input_time_string,
					delimiter,
					input_buffer,
					time_offset ) )
			{
				fprintf(stderr,
				"Warning in %s/%d: cannot get date from (%s)\n",
			 		argv[ 0 ],
			 		__LINE__,
			 		input_buffer );
				continue;
			}
		}
		else
		{
			*input_time_string = '\0';
		}

		sprintf(	input_date_time_string,
				"%s^%s",
				input_date_string,
				input_time_string );

		if ( !date_set_yyyy_mm_dd_hhmm_delimited(
				input_date,
				input_date_time_string,
				0 /* date_offset */,
				1 /*time_offset */,
				'^' ) )
		{
			fprintf( stderr,
			"Warning in %s/%s()/%d: invalid date_time of (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_date_time_string );
			continue;
		}

		while( 1 )
		{
			if ( !get_line_queue(	ticker_date_time_string,
						ticker_date_time_pipe ) )
			{
				break;
			}

			if ( !date_set_yyyy_mm_dd_hhmm_delimited(
					ticker_date,
					ticker_date_time_string,
					0 /* date_piece */,
					1 /* time_piece */,
					'^' ) )
			{
				fprintf( stderr,
			"ERROR in %s/%s()/%d: invalid date_time of (%s)\n",
				 	__FILE__,
				 	__FUNCTION__,
				 	__LINE__,
				 	ticker_date_time_string );
				exit( 1 );
			}

			if ( input_date->current == ticker_date->current )
			{
				break;
			}
			else
			if ( input_date->current < ticker_date->current )
			{
				unget_line_queue( ticker_date_time_string );
				break;
			}
			else
			{
				output_null_value(
					delimiter,
					date_get_yyyy_mm_dd(
						ticker_date_buffer,
						ticker_date ),
					date_get_hhmm(
						ticker_time_buffer,
						ticker_date ),
					aggregate_level,
					date_offset,
					time_offset,
				        value_offset,
					append_string );
			}
		}

		printf( "%s\n", input_buffer );

	}
	pclose( ticker_date_time_pipe );
	return 0;

} /* main() */

void output_null_value(		char delimiter,
				char *ticker_date_string,
				char *ticker_time_string,
				enum aggregate_level aggregate_level,
				int date_offset,
				int time_offset,
				int value_offset,
				char *append_string )
{
	if ( aggregate_level != real_time
	&&   aggregate_level != half_hour
	&&   aggregate_level != hourly
	&&   aggregate_level != aggregate_level_none )
	{
		ticker_time_string = "null";
	}

	if ( date_offset == 0
	&&   time_offset == 1
	&&   value_offset == 2 )
	{
		printf( "%s%c%s%c",
			ticker_date_string,
			delimiter,
			ticker_time_string,
			delimiter );
	}
	else
	if ( value_offset == 0
	&&   date_offset == 1
	&&   time_offset == 2 )
	{
		printf( "%c%s%c%s",
			delimiter,
			ticker_date_string,
			delimiter,
			ticker_time_string );
	}
	else
	if ( date_offset == 0
	&&   time_offset == -1
	&&   value_offset == 1 )
	{
		printf( "%s%c",
			ticker_date_string,
			delimiter );
	}
	else
	if ( value_offset == 0
	&&   time_offset == -1
	&&   date_offset == 1 )
	{
		printf( "%c%s",
			delimiter,
			ticker_date_string );
	}
	else
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: invalid date,time,value offsets = %d,%d,%d\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 date_offset,
			 time_offset,
			 value_offset );
		exit( 1 );
	}

	printf( "%s\n", append_string );

} /* output_null_value() */

