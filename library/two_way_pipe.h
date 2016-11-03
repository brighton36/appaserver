/* library/two_way_pipe.h	*/
/* ---------------------------- */
/* Tim Riley		        */
/* ---------------------------- */

#ifndef TWO_WAY_PIPE_H
#define TWO_WAY_PIPE_H

#define TWO_WAY_PIPE_MAX_INPUT_LINE	4096

typedef struct
{
	FILE *output_pipe;
	FILE *input_file;
	char input_line[ TWO_WAY_PIPE_MAX_INPUT_LINE ];
	char *fifo_filename;
} TWO_WAY_PIPE;

TWO_WAY_PIPE *new_two_way_pipe( 	char *output_process,
					char *fifo_directory);
void two_way_pipe_remove_fifo(		TWO_WAY_PIPE *two_way_pipe );
char *two_way_pipe_get_string(  	TWO_WAY_PIPE *two_way_pipe );
void two_way_pipe_set_string(		TWO_WAY_PIPE *two_way_pipe,
					char *output_string );
void two_way_pipe_close( 		TWO_WAY_PIPE *two_way_pipe );
char *two_way_pipe_create_fifo(		char *fifo_directory );

#endif
