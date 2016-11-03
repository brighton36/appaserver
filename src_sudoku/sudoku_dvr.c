/* sudoku_dvr.c			         */
/* ------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sudoku.h"

int main( int argc, char **argv )
{
	SUDOKU sudoku;
	int x, y, value;
	char display_delimited_yn;

	if ( argc != 2 )
	{
		fprintf(	stderr,
				"Usage: %s display_delimited_yn\n",
				argv[ 0 ] );
		exit( 1 );
	}

	display_delimited_yn = *argv[ 1 ];

	memset( &sudoku, 0, sizeof( SUDOKU ) );

	while( fscanf( stdin, "%d,%d,%d", &x, &y, &value ) == 3 )
		sudoku_set_manual( sudoku.cell_array, x, y, value );

	sudoku_play( sudoku, ( display_delimited_yn == 'y' ) );
	return 0;
} /* main() */

