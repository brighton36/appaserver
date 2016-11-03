/* sudoku.h */
/* -------- */

#ifndef SUDOKU_H
#define SUDOKU_H

#define SUDOKU_SIZE 9

typedef struct
{
	int value;
} CELL;

typedef struct
{
	CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ];
} SUDOKU;


/* SUDOKU Function Prototypes */
/* -------------------------- */
void sudoku_set_manual(		CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ],
				int column, int row, int value );
int *sudoku_get_compare_array(	CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ],
				int upper_left_x,
				int upper_left_y,
				int lower_right_x,
				int lower_right_y );
int sudoku_completely_populated(CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ],
				int upper_left_x,
				int upper_left_y,
				int lower_right_x,
				int lower_right_y );
int sudoku_compare_array_completely_populated(
				int compare_array[ SUDOKU_SIZE ] );
int sudoku_play(		SUDOKU sudoku,
				int display_delimited );
void sudoku_display(		CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ] );
void sudoku_display_delimited(	CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ] );
int sudoku_board_complete(	CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ] );
int sudoku_contradictions(	CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ] );
int sudoku_segment_contradictions(
				CELL cell_array[ SUDOKU_SIZE ][ SUDOKU_SIZE ],
				int upper_left_x,
				int upper_left_y,
				int lower_right_x,
				int lower_right_y );
int sudoku_compare_array_contradictions(
				int compare_array[ SUDOKU_SIZE ] );
int sudoku_play_current(	SUDOKU sudoku,
				int current_value,
				int display_delimited );
void sudoku_get_next_current_x_y(
				int *x, int *y );
#endif
