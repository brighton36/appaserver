/* table.h   					*/
/* -------------------------------------------- */
/* This is the header file for the TABLE ADT 	*/
/* Tim Riley 					*/
/* -------------------------------------------- */

#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include "dictionary.h"

#define MAX_ROWS			65535
#define MAX_COLUMN_GROUPS		64
#define MAX_COLUMNS_PER_GROUP		64

#define DATATYPE_INTEGER		0
#define DATATYPE_DOUBLE			1
#define DATATYPE_STRING			2

#define LOAD_DATA_FORMAT_DELIMITED	0
#define LOAD_DATA_FORMAT_LABELED	1

typedef struct
{
	char *name;
	char *heading;
	char justify_plus_minus;
	int width;
	int precision;
	int datatype;
	DICTIONARY *value_dictionary;
	int display_off;
	char *default_value;
} COLUMN;

typedef struct
{
	char *name;
	char *heading;
	COLUMN *column_array[ MAX_COLUMNS_PER_GROUP ];
	int number_columns;
	int current_column;
} COLUMN_GROUP;

typedef struct 
{
	char *title;
	char *title_continued;
	char *footer;
	int width;
	COLUMN_GROUP *column_group[ MAX_COLUMN_GROUPS ];
	int number_column_groups;
	int current_column_group;
	int rows_per_page;

	/* These are used to maintain the primary key column */
	/* ------------------------------------------------- */
	char *key_column_name;
	char *key_column_array[ MAX_ROWS ];
	int number_key_column_array;

	/* This is used for loading data from a file */
	/* ----------------------------------------- */
	char *group_label;

	/* Misc. */
	/* ----- */
	int group_key_length_skip;
	int suppress_output_on_any_empty_column;
	int key_column_loaded_from_process;
	int heading_off;
} TABLE;

/* Operations on TABLE */
/* ------------------- */
int at_last_column_group( TABLE *table );
void table_output( FILE *destination, TABLE *table );
COLUMN *table_get_column( char *column_name );
void set_suppress_output_on_any_empty_column( TABLE *table );
void set_heading_off( TABLE *table );
void set_key_column_name( TABLE *table, char *column_name );
void set_key_data( TABLE *table, char *data );
void load_key_process( TABLE *table, char *process );
void load_data_labeled( TABLE *table, FILE *input_file );
void load_width( TABLE *table, char *data );
void load_rows_per_page( TABLE *table, char *data );
void load_footer( TABLE *table, char *data );
void load_column_group( TABLE *table, char *data );
void load_column( TABLE *table, char *data );
void load_column_display_off( TABLE *table, char *data );
void load_configuration_file( TABLE *table, char *filename );
void set_column_display_off( TABLE *table, char *group_name, char *column_name);
void set_is_key_column( TABLE *table, char *group_name, char *column_name );
void append_key_column( TABLE *table, char *key_column );
void append_column_group( TABLE *table, char *group_name, char *heading );
void set_column(	TABLE *table, 
			char *group_name, 
			char *column_name, 
			int datatype,
			char *heading,
			char justify_plus_minus,
			int width,
			int precision,
			char *default_value );
TABLE *table_new( void );
void set_rows_per_page( TABLE *table, int rows_per_page );
void set_title( TABLE *table, char *title );
void set_width( TABLE *table, int width );
void set_value(		TABLE *table, 
			char *group_name,
			char *column_name,
			char *key,
			void *value );
int set_value_string(	TABLE *table, 
			char *group_name,
			char *column_name,
			char *key,
			char *value );
void table_display( FILE *destination, TABLE *table );
int reset_get_column_group( TABLE *table );
COLUMN_GROUP *get_next_column_group( TABLE *table );
COLUMN_GROUP *search_column_group( 	TABLE *table, 
					char *group_name );
COLUMN *search_column( 	TABLE *table, char *group_name, char *column_name );
void set_footer( TABLE *table, char *footer );
void set_title_continued( TABLE *table, char *title_continued );
void load_group( TABLE *table, char *group_command );
void set_group_key_length_skip( TABLE *table, int group_key_length_skip );

/* This is used for loading data from a file */
/* ----------------------------------------- */
void set_group_label( TABLE *table, char *group_label );

/* Operations on COLUMN_GROUP */
/* -------------------------- */
int some_column_is_empty( COLUMN_GROUP *column_group );
COLUMN_GROUP *column_group_new( char *name, char *heading );
COLUMN *get_next_column( COLUMN_GROUP *column_group );
int exists_some_data( COLUMN_GROUP *column_group );

/* Operations on COLUMN */
/* -------------------- */
COLUMN *get_column( COLUMN_GROUP *column_group );
COLUMN *column_new( 	char *column_name, 
			int datatype,
			char *heading, 
			char plus_minus, 
			int width, 
			int precision,
			char *default_value );
void output_column( 	FILE *destination, 
			COLUMN *column, 
			char *key );
int get_column_width( COLUMN *column );

/* Misc */
/* ---- */
void exit_early( char *from_where );
int get_spaces_between_column_groups( 	int *spaces_after_first_column,
					TABLE *table );
void output_column_group_heading( FILE *destination, 
				  TABLE *table,
				  int spaces_between_column_groups,
				  int spaces_after_first_column,
				  int suppress_output_on_any_empty_column );
void output_column_heading( 	FILE *destination, 
				TABLE *table,
				int spaces_between_column_groups,
				int spaces_after_first_column,
				int suppress_output_on_any_empty_column );
void output_heading( 		FILE *destination, 
				TABLE *table,
				int spaces_between_column_groups,
				int spaces_after_first_column,
				int suppress_output_on_any_empty_column );
void output_spaces_between_column_groups( FILE *destination,
					  int spaces_between_column_groups );
void output_spaces_after_first_column_group( FILE *destination,
				             int spaces_after_first_column );
void output_title( FILE *destination, char *title, int width );
void output_line( FILE *destination, int width );
void output_new_page( 	FILE *destination, 
			TABLE *table,
			int spaces_between_column_groups,
			int spaces_after_first_column,
			int suppress_output_on_any_empty_column );
void output_footer( 	FILE *destination, 
			TABLE *table );
int datatype_string2int( char *datatype );
int search_fn_array( char *label );
int reset_get_column( COLUMN_GROUP *column_group );
int get_sum_column_widths( COLUMN_GROUP *column_group,
			   int suppress_output_on_any_empty_column );

#endif
