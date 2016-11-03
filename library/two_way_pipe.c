/* library/two_way_pipe.c */
/* ---------------------- */
/* Tim Riley		  */
/* ---------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "two_way_pipe.h"

TWO_WAY_PIPE *new_two_way_pipe( char *output_process,
				char *fifo_directory )
{
	TWO_WAY_PIPE *two_way_pipe;
	char output_process_redirected[ 1024 ];

	two_way_pipe = (TWO_WAY_PIPE *)calloc( 1, sizeof( TWO_WAY_PIPE ) ) ;

	two_way_pipe->fifo_filename =
		two_way_pipe_create_fifo( fifo_directory );

	sprintf( output_process_redirected,
	 	 "%s > %s",
		 output_process,
		 two_way_pipe->fifo_filename);

	two_way_pipe->output_pipe =
		popen( output_process_redirected, "w" );
	two_way_pipe->input_file =
		fopen( two_way_pipe->fifo_filename, "r" );

	return two_way_pipe;

} /* new_two_way_pipe() */

char *two_way_pipe_get_string(  TWO_WAY_PIPE *two_way_pipe )
{
	*two_way_pipe->input_line = '\0';
	get_line( two_way_pipe->input_line, two_way_pipe->input_file );
	return two_way_pipe->input_line;

} /* two_way_pipe_get_string() */

void two_way_pipe_set_string(	TWO_WAY_PIPE *two_way_pipe,
				char *output_string )
{
	fprintf( two_way_pipe->output_pipe, "%s\n", output_string );
	fflush( two_way_pipe->output_pipe );

} /* two_way_pipe_set_string() */

void two_way_pipe_close( TWO_WAY_PIPE *two_way_pipe )
{
	pclose( two_way_pipe->output_pipe );
	pclose( two_way_pipe->input_file );
	two_way_pipe_remove_fifo( two_way_pipe );
}

void two_way_pipe_remove_fifo( TWO_WAY_PIPE *two_way_pipe )
{
	char sys_string[ 1024 ];
	int results;

	sprintf( sys_string,
		 "rm %s",
		 two_way_pipe->fifo_filename );
	results = system( sys_string );
}

char *two_way_pipe_create_fifo( char *directory )
{
	char sys_string[ 1024 ];
	int results;
	char fifo_filename[ 1024 ];

	if ( directory && *directory )
	{
		sprintf( fifo_filename, 
			 "%s/fifo_%d",
			 directory,
			 getpid() );
	}
	else
	{
		sprintf( fifo_filename, 
			 "fifo_%d",
			 getpid() );
	}
	sprintf( sys_string,
		 "mkfifo %s",
		 fifo_filename );
	results = system( sys_string );
	return strdup( fifo_filename );
} /* two_way_pipe_create_fifo() */

