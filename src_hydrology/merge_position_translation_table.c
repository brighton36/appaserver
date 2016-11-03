/* merge_position_translation_table.c 				*/
/* ------------------------------------------------------------ */
/* Expected input from stdin: "record_type|datatype|position" 	*/
/* 10|stage|1							*/
/* 60|rainfall|1						*/
/* 60|surface_temperature|3					*/
/* 60|bottom_temperature|4					*/
/* 60|conductivity|5						*/
/* 60|salinity|6						*/
/* 60|voltage|7							*/
/*								*/
/* output: "record_type|datatype_1,...,datatype_n"	 	*/
/*	   10|stage						*/
/*	   60|rainfall,unused,surface_temperature,...		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"

/* Prototypes */
/* ---------- */
int output_unuseds( char *destination, int number_to_output, int first_time );

int main( int argc, char **argv )
{
	char input_buffer[ 1024 ];
	char output_buffer[ 1024 ] = {0};
	char *output_buffer_ptr = output_buffer;
	char old_record_type[ 128 ] = {0};
	char record_type[ 128 ];
	char datatype[ 128 ];
	char position_string[ 128 ];
	char *station;
	int line = 0;
	int position;
	int current_position = 0;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s station\n", argv[ 0 ] );
		exit( 1 );
	}

	station = argv[ 1 ];

	while( get_line( input_buffer, stdin ) )
	{
		line++;
		piece( record_type, '|', input_buffer, 0 );

		if ( !piece( datatype, '|', input_buffer, 1 ) )
		{
			fprintf( stderr,
				 "Warning: cannot parse line %d = (%s)\n",
				 line, input_buffer );
			continue;
		}

		if ( !piece( position_string, '|', input_buffer, 2 ) )
		{
			fprintf( stderr,
				 "Warning: cannot parse line %d = (%s)\n",
				 line, input_buffer );
			continue;
		}

		position = atoi( position_string );

		if ( position < 1 )
		{
			fprintf( stderr,
			"Warning: bad position entry at line %d in (%s)\n",
				 line, input_buffer );
			continue;
		}

		if ( strcmp( record_type, old_record_type ) != 0 )
		{
			strcpy( old_record_type, record_type );
			current_position = 1;

			if ( *output_buffer )
				printf( "%s\n", output_buffer );
			
			output_buffer_ptr = output_buffer;

			output_buffer_ptr += 
				sprintf( output_buffer_ptr,
					 "%s|",
					 record_type );

			if ( position != current_position )
			{
				output_buffer_ptr +=
					output_unuseds(
						output_buffer_ptr,
						position - current_position,
						1 /* first_time */ );

				current_position = position + 1;
				output_buffer_ptr += 
					sprintf( output_buffer_ptr,
				 	 	",%s",
				 	 	datatype );
			}
			else
			{
				current_position++;

				output_buffer_ptr += 
					sprintf( output_buffer_ptr,
				 	 	"%s",
				 	 	datatype );
			}
		}
		else
		{
			if ( position != current_position )
			{
				output_buffer_ptr +=
					output_unuseds(
						output_buffer_ptr,
						position - current_position,
						0 /* not first_time */ );

				current_position = position;
			}
			current_position++;

			output_buffer_ptr += 
				sprintf( output_buffer_ptr,
				 	 ",%s",
				 	 datatype );
		}
	}
	printf( "%s\n", output_buffer );
	exit( 0 );
} /* main() */


int output_unuseds( char *destination, int number_to_output, int first_time )
{
	int total_bytes_outputted = 0;
	int bytes_outputted;

	while( number_to_output-- )
	{
		if ( first_time )
		{
			bytes_outputted = 
				sprintf( destination, "%s", "unused" );
			first_time = 0;
		}
		else
		{
			bytes_outputted = 
				sprintf( destination, ",%s", "unused" );
		}
		destination += bytes_outputted;
		total_bytes_outputted += bytes_outputted;
	}
	return total_bytes_outputted;
} /* output_unused() */

