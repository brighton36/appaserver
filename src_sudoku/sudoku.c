/* sudoku.c */
/* -------- */

#include <stdio.h>
#include <stdlib.h>
#include "sudoku.h"


void sudoku_set_manual(	CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ],
			int column, int row, int value )
{
	cell_array[ column ] [ row ].value = value;
} /* sudoku_set_manual() */

void sudoku_display_delimited( CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ] )
{
	int x, y;

	for( y = 0; y < SUDOKU_SIZE; y++ )
	{
		for( x = 0; x < SUDOKU_SIZE; x++ )
		{
			printf( "%d,%d,%d\n",
				x, y, cell_array[ x ] [ y ].value );
		}
	}
} /* sudoku_display_delimited() */

void sudoku_display( CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ] )
{
	int x, y;

	for( y = 0; y < SUDOKU_SIZE; y++ )
	{
		for( x = 0; x < SUDOKU_SIZE; x++ )
		{
			printf( "%d ", cell_array[ x ] [ y ].value );
		}
		printf( "\n" );
	}
	printf( "\n\n" );

} /* sudoku_display() */

int sudoku_contradictions( CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ] )
{
	int x, y;

	for ( y = 0; y < 9; y += 3 )
	{
		for( x = 0; x < 9; x += 3 )
		{
			if ( sudoku_segment_contradictions(
						cell_array,
						x /* upper_left_x */,
						y /* upper_left_y */,
						x + 2 /* lower_right_x */,
						y + 2 /* lower_right_y */ ) )
			{
				return 1;
			}
		}
	}

	for( x = 0; x < 9; x++ )
	{
		if ( sudoku_segment_contradictions(
				cell_array,
				x /* upper_left_x */,
				0 /* upper_left_y */,
				x /* lower_right_x */,
				8 /* lower_right_y */ ) )
		{
			return 1;
		}
	}

	for( y = 0; y < 9; y++ )
	{
		if ( sudoku_segment_contradictions(
				cell_array,
				0 /* upper_left_x */,
				y /* upper_left_y */,
				8 /* lower_right_x */,
				y /* lower_right_y */ ) )
		{
			return 1;
		}
	}
	return 0;
} /* sudoku_contradictions() */

int sudoku_board_complete( CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ] )
{
	int x, y;

	for ( y = 0; y < 9; y += 3 )
	{
		for( x = 0; x < 9; x += 3 )
		{
			if ( !sudoku_completely_populated(
						cell_array,
						x /* upper_left_x */,
						y /* upper_left_y */,
						x + 2 /* lower_right_x */,
						y + 2 /* lower_right_y */ ) )
			{
				return 0;
			}
		}
	}

	for( x = 0; x < 9; x++ )
	{
		if ( !sudoku_completely_populated(
				cell_array,
				x /* upper_left_x */,
				0 /* upper_left_y */,
				x /* lower_right_x */,
				8 /* lower_right_y */ ) )
		{
			return 0;
		}
	}

	for( y = 0; y < 9; y++ )
	{
		if ( !sudoku_completely_populated(
				cell_array,
				0 /* upper_left_x */,
				y /* upper_left_y */,
				8 /* lower_right_x */,
				y /* lower_right_y */ ) )
		{
			return 0;
		}
	}
	return 1;
} /* sudoku_board_complete() */

int sudoku_segment_contradictions(
				CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ],
				int upper_left_x,
				int upper_left_y,
				int lower_right_x,
				int lower_right_y )
{
	int *compare_array;

	compare_array = sudoku_get_compare_array(
				cell_array,
				upper_left_x,
				upper_left_y,
				lower_right_x,
				lower_right_y );

	return sudoku_compare_array_contradictions( compare_array );
} /* sudoku_segment_contradictions() */

int sudoku_completely_populated(CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ],
				int upper_left_x,
				int upper_left_y,
				int lower_right_x,
				int lower_right_y )
{
	int *compare_array;

	compare_array = sudoku_get_compare_array(
				cell_array,
				upper_left_x,
				upper_left_y,
				lower_right_x,
				lower_right_y );

	return sudoku_compare_array_completely_populated( compare_array );
} /* sudoku_completely_populated() */

int *sudoku_get_compare_array(	CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ],
				int upper_left_x,
				int upper_left_y,
				int lower_right_x,
				int lower_right_y )
{
	int x, y;
	int index = 0;
	static int compare_array[ SUDOKU_SIZE ];

	for( y = upper_left_y; y <= lower_right_y; y++ )
	{
		for( x = upper_left_x; x <= lower_right_x; x++ )
		{
			if ( index == 9 )
			{
				fprintf(stderr,
"ERROR in %s/%s()/%d: index = %d; upper_left = (%d,%d ); lower_right = (%d,%d)\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					index,
					upper_left_x,
					upper_left_y,
					lower_right_x,
					lower_right_y );
				exit( 1 );
			}
			compare_array[ index ] =
				cell_array[ x ][ y ].value;

			index++;
		}
	}

	return compare_array;
} /* sudoku_get_compare_array() */

int sudoku_compare_array_completely_populated(
				int compare_array[ SUDOKU_SIZE ] )
{
	int index;
	int bucket_array[ 9 ] = {0};

	for( index = 0; index < 9; index++ )
	{
		if ( !compare_array[ index ] ) return 0;
		bucket_array[ compare_array[ index ] - 1 ] = 1;
	}

	for( index = 0; index < 9; index++ )
		if ( !bucket_array[ index ] ) return 0;
	return 1;

} /* sudoku_compare_array_completely_populated() */

int sudoku_compare_array_contradictions( int compare_array[ SUDOKU_SIZE ] )
{
	int index;
	int bucket_array[ 9 ] = {0};

	for( index = 0; index < 9; index++ )
	{
		if ( compare_array[ index ] )
		{
			if ( bucket_array[ compare_array[ index ] - 1 ] )
				return 1;
			bucket_array[ compare_array[ index ] - 1 ] = 1;
		}
	}
	return 0;
} /* sudoku_compare_array_contradictions() */

void sudoku_get_next_current_x_y( int *x, int *y )
{
	(*x)++;
	if ( *x == SUDOKU_SIZE )
	{
		*x = 0;
		(*y)++;
	}
} /* sudoku_get_next_current_x_y() */

int sudoku_get_empty_slot(	int *x, int *y,
				CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ] )
{
	do {
		sudoku_get_next_current_x_y( x, y );
		if ( *y == SUDOKU_SIZE ) return 0;
		if ( !cell_array[ *x ][ *y ].value ) return 1;
	} while( 1 );

} /* sudoku_get_empty_slot() */

int sudoku_play( SUDOKU sudoku, int display_delimited )
{
	int current_value;

	if ( !display_delimited )
	{
		printf( "Starting\n" );
		printf( "--------\n" );
		sudoku_display( sudoku.cell_array );
	}

	for (	current_value = 1;
		current_value <= 9;
		current_value++ )
	{
		if ( sudoku_play_current(	sudoku,
						current_value,
						display_delimited ) )
		{
			return 1;
		}
	}
	return 0;
} /* sudoku_play() */

int sudoku_play_current(	SUDOKU sudoku,
				int current_value,
				int display_delimited )
{
	int x = -1, y = 0;

	if ( sudoku_board_complete( sudoku.cell_array ) )
	{
		if ( display_delimited )
		{
			sudoku_display_delimited( sudoku.cell_array );
		}
		else
		{
			printf( "Winner!\n" );
			printf( "-------\n" );
			sudoku_display( sudoku.cell_array );
		}
		return 1;
	}

	if ( sudoku_get_empty_slot( &x, &y, sudoku.cell_array ) )
	{
		sudoku.cell_array[ x ][ y ].value = current_value;

		if ( !sudoku_contradictions( sudoku.cell_array ) )
		{
			for (	current_value = 1;
				current_value <= 9;
				current_value++ )
			{
				if ( sudoku_play_current(
						sudoku,
						current_value,
						display_delimited ) )
				{
					return 1;
				}
			}
		}
	}
	return 0;
} /* sudoku_play() */

