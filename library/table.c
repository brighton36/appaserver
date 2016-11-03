/* table.c 		  */
/* ---------------------- */
/* Code for the TABLE ADT */
/* ---------------------- */
/* Tim Riley		  */
/* ---------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"
#include "column.h"
#include "timlib.h"
#include "piece.h"

/* Constants */
/* --------- */
#define DEBUG 0

/* Array of functions used to load the configuration from a file */
/* ------------------------------------------------------------- */
static struct
{
	char *fn_name;
	void (*fn)();
} fn_array[] = {{"title",		set_title},
		{"title_continued",	set_title_continued},
		{"width", 		load_width},
		{"rows_per_page", 	load_rows_per_page},
		{"footer",		load_footer},
		{"column_group",		load_column_group},
		{"column",		load_column},
		{"column_display_off",	load_column_display_off},
		{"group_label",		set_group_label},
		{"group",		load_group},
		{"load_key_process",	load_key_process},
		{"key_column_name",	set_key_column_name},
		{"heading_off",		set_heading_off},
		{"suppress_output_on_any_empty_column",
				set_suppress_output_on_any_empty_column},
		{(char *)0,		NULL }};

/* Operations on TABLE */
/* ------------------- */
int at_last_column_group( TABLE *table )
{
	return ( table->number_column_groups == table->current_column_group ) ?
		1 : 0;
}

void set_suppress_output_on_any_empty_column ( TABLE *table )
{
	table->suppress_output_on_any_empty_column = 1;
}

void set_heading_off( TABLE *table )
{
	table->heading_off = 1;
}

void set_key_column_name( TABLE *table, char *column_name )
{
	table->key_column_name = strdup( column_name );
}

/* ----------------------------------------------- */
/* Sample: "load_key_process: report_key.sh 1997   */
/*                 	      ^		   	   */
/*		   	      |		   	   */
/*			process_command		   */
/* ----------------------------------------------- */

void load_key_process( TABLE *table, char *process )
{
	char *group_name = "";
	char buffer[ 512 ];
	FILE *p;
	int return_value;

	if ( !table->key_column_name )
	{
		fprintf(stderr,
			"ERROR: You must set key_column_name\n" );
		exit_early( "load_key_process" );
	}

 	p = popen( process, "r" );
	while ( get_line( buffer, p ) )
	{
		if ( !*buffer || *buffer == '#' ) continue;

		return_value = set_value_string( table, 
						 group_name,
						 table->key_column_name, 
						 buffer, 
						 buffer );
		if ( DEBUG )
		{
			fprintf(stderr, 
				"set_value_string(%s,%s,%s,%s) results: %s\n",
				group_name,
				table->key_column_name,
				buffer,
				buffer,
				(return_value)?"Succeeded":"Failed" );
		}
	}
	fclose( p );
	table->key_column_loaded_from_process = 1;

} /* load_key_process() */


void load_data_labeled( TABLE *table, FILE *f )
{
	char buffer[ 512 ];
	char column_name[ 128 ];
	char group_name[ 128 ];
	char key_column_data[ 128 ] = {0};
	char data[ 128 ] = {0};
	int return_value;

	if ( !table->key_column_name || !table->group_label )
	{
		fprintf(stderr,
		"ERROR: You must set key_column_name and group_label\n" );
		exit_early( "load_data_labeled" );
	}

	while ( get_line( buffer, f ) )
	{
		if ( !*buffer || *buffer == '#' ) continue;

		piece( column_name, ':', buffer, 0 );
		if ( !piece( data, ':', buffer, 1 ) ) continue;

		if ( strcmp( column_name, table->group_label ) == 0 )
			strcpy( group_name, data );
		else
		if ( strcmp( column_name, table->key_column_name ) == 0 )
			strcpy( key_column_data, data );

		if ( !*data || strcmp( data, "null" ) == 0 ) continue;

		if ( strcmp( table->key_column_name, column_name ) == 0
		&&   table->key_column_loaded_from_process )
		{
			if ( DEBUG )
				fprintf(stderr,
					"Not setting for: %s\n", 
					buffer );
			continue;
		}

		if ( !*key_column_data )
		{
			if ( DEBUG )
				fprintf(stderr,
		"Warning: key_column_data is not set correctly for: (%s)\n", 
					buffer );

			continue;
		}

		return_value = set_value_string( table, 
						 group_name,
						 column_name, 
						 key_column_data, 
						 data );
		if ( DEBUG )
		{
			fprintf(stderr, 
	"set_value_string(group=%s,column=%s,key=%s,data=%s) status=%s\n",
				group_name,
				column_name,
				key_column_data,
				data,
				(return_value)?"Succeeded":"Failed" );
		}
	}
	fclose( f );

} /* load_data_labeled() */

/* ------------------------------------ */
/* Sample: "group: key_length skip 2" 	*/
/*                 ^			*/
/*		   |			*/
/*		group_command		*/
/* ------------------------------------ */
void load_group( TABLE *table, char *group_command )
{
	char group_what[ 128 ];
	char action[ 128 ];
	char how_many_string[ 128 ];
	int how_many;

	column( group_what, 0, group_command );
	if ( strcmp( group_what, "key_length" ) != 0 )
	{
		fprintf(stderr,
		"Expected \"key_length\" following \"group:\" on (%s)\n",
		group_command );
		exit_early( "load_group" );
	}
	if ( !column( action, 1, group_command )
	||    strcmp( action, "skip" ) != 0 )
	{
		fprintf(stderr,
			"Currently, you can only skip on a key_length\n" );
		exit_early( "load_group" );
	}
	if ( !column( how_many_string, 2, group_command ) )
		how_many = 1;
	else
		how_many = atoi( how_many_string );
	
	set_group_key_length_skip( table, how_many );
	return;

} /* load_group() */

void set_group_key_length_skip( TABLE *table, int group_key_length_skip )
{
	table->group_key_length_skip = group_key_length_skip;
}

void set_group_label( TABLE *table, char *group_label )
{
	table->group_label = strdup( group_label );
}
void load_width( TABLE *table, char *data )
{
	set_width( table, atoi( data ) );
}
void load_rows_per_page( TABLE *table, char *data )
{
	set_rows_per_page( table, atoi( data ) );
}
void load_footer( TABLE *table, char *data )
{
	set_footer( table, data );
}
void load_column_group( TABLE *table, char *data )
{
	char group_name[ 128 ];
	char heading[ 128 ];
	piece( group_name, '|', data, 0 );
	if ( !piece( heading, '|', data, 1 ) )
	{
		fprintf(stderr, "ERROR: invalid format: %s\n", data );
		exit_early( "load_column_group" );
	}
	append_column_group( table, group_name, heading );
}

void load_column( TABLE *table, char *data )
{
	char buffer[ 128 ];
	char group_name[ 128 ];
	char column_name[ 128 ];
	char datatype[ 128 ];
	char heading[ 128 ];
	char justify_plus_minus = {0};
	int width = 0;
	int precision = 0;
	int datatype_int;
	char default_value[ 128 ];

	piece( group_name, '|', data, 0 );
	piece( column_name, '|', data, 1 );
	piece( datatype, '|', data, 2 );
	piece( heading, '|', data, 3 );
	if ( piece( buffer, '|', data, 4 ) )
		justify_plus_minus = *buffer;
	if ( piece( buffer, '|', data, 5 ) )
		width = atoi( buffer );
	if ( piece( buffer, '|', data, 6 ) )
		precision = atoi( buffer );
	if ( !piece( default_value, '|', data, 7 ) )
	{
		fprintf( stderr, "Not enough fields for: (%s)\n", data );
		exit_early( "load_column");
	}

	datatype_int = datatype_string2int( datatype );

	set_column(	table, 
			group_name, 
			column_name, 
			datatype_int, 
			heading,
			justify_plus_minus,
			width,
			precision,
			default_value );
} /* load_column() */

void load_column_display_off( TABLE *table, char *data )
{
	char group_name[ 128 ];
	char column_name[ 128 ];
	piece( group_name, '|', data, 0 );
	if ( !piece( column_name, '|', data, 1 ) )
	{
		fprintf(stderr, "ERROR: invalid format: %s\n", data );
		exit_early( "load_is_key_column" );
	}
	set_column_display_off( table, group_name, column_name );
}

void load_configuration_file( TABLE *table, char *filename )
{
	FILE *f = fopen( filename, "r" );
	int index;
	char buffer[ 512 ];
	char label[ 128 ];
	char data[ 128 ];

	if ( !f )
	{
		fprintf(stderr,
			"ERROR: Cannot open %s for read\n",
			filename );
		exit_early( "load_configuration_file" );
	}

	while ( get_line( buffer, f ) )
	{
		if ( !*buffer || *buffer == '#' ) continue;
		if ( !exists_character( buffer, ':' ) ) continue;

		piece( label, ':', buffer, 0 );
		piece( data, ':', buffer, 1 );

		if ( ( index = search_fn_array( label ) ) == -1 )
		{
			fprintf(stderr, "Invalid label: (%s)\n", label );
			exit_early( "load_configuration" );
		}
	
		/* Execute the function */
		/* -------------------- */
		fn_array[ index ].fn( table, data );
	}
	fclose( f );
} /* load_configuration_file() */

void set_column_display_off( TABLE *table, char *group_name, char *column_name )
{
	COLUMN *column;
	column = search_column( table, group_name, column_name );
	if ( !column ) exit_early( "set_column_display_off" );
	column->display_off = 1;
} /* set_column_display_off() */

TABLE *table_new()
{
	TABLE *t = (TABLE *)calloc( 1, sizeof( TABLE ) );
	return t;
} /* table_new() */

void set_width( TABLE *table, int width )
{
	table->width = width;
} /* set_width() */

void set_title( TABLE *table, char *title )
{
	table->title = strdup( title );
} /* set_title() */

void set_rows_per_page( TABLE *table, int rows_per_page )
{
	table->rows_per_page = rows_per_page;
} /* set_rows_per_page() */


void append_column_group( TABLE *table, char *name, char *heading )
{
	if ( table->number_column_groups == MAX_COLUMN_GROUPS )
	{
		fprintf(stderr,
		"ERROR in append_column_group(%s) max exceeded: %d\n",
			name,
			MAX_COLUMN_GROUPS );
		exit( 1 );
	}
	table->column_group[ table->number_column_groups++ ] = 
		column_group_new( name, heading );
} /* append_column_group() */

void set_column(	TABLE *table, 
			char *group_name, 
			char *column_name, 
			int datatype,
			char *heading,
			char justify_plus_minus,
			int width,
			int precision,
			char *default_value )
{
	COLUMN *column;
	COLUMN_GROUP *column_group;

	column_group = search_column_group( table, group_name );
	if ( !column_group ) exit_early( "set_column" );
	if ( column_group->number_columns == MAX_COLUMNS_PER_GROUP )
	{
		fprintf( stderr,
			 "ERROR: set_column(%s,%s,%s) exceedes %d\n",
			 group_name,
			 column_name,
			 heading,
			 MAX_COLUMNS_PER_GROUP );
		exit_early( "set_column" );
	}
	
	column = column_new( 	column_name, 
				datatype,
				heading, 
				justify_plus_minus, 
				width, 
				precision,
				default_value );
	column_group->column_array[ column_group->number_columns++ ] = column;
} /* set_column() */

int reset_get_column_group( TABLE *table )
{
	if ( !table->number_column_groups )
		return 0;
	else
	{
		table->current_column_group = 0;
		return 1;
	}
} /* reset_get_column_group() */

COLUMN_GROUP *get_next_column_group( TABLE *table )
{
	if ( table->current_column_group == table->number_column_groups )
		return (COLUMN_GROUP *)0;
	else
		return table->column_group[ table->current_column_group++ ];
} /* get_next_column_group() */

COLUMN_GROUP *search_column_group( TABLE *table, char *name )
{
	COLUMN_GROUP *a;

	reset_get_column_group( table );
	while( ( a = get_next_column_group( table ) ) )
		if ( strcmp( a->name, name ) == 0 )
			return a;
	fprintf(stderr,
		"ERROR: cannot find column group %s\n",
		name );
	exit( 1 );
} /* search_column_group() */

void append_key_column( TABLE *table, char *value )
{
	if ( table->number_key_column_array == MAX_ROWS )
	{
		fprintf(stderr,
			"Max rows of %d exceeded with %s\n",
			MAX_ROWS,
			value );
		exit_early( "append_key_column" );
	}
	table->key_column_array[ table->number_key_column_array++ ] =
		strdup( value );
} /* append_key_column() */

/* This assumes value is always a string */
/* ------------------------------------- */
int set_value_string(	TABLE *table, 
			char *group_name,
			char *column_name,
			char *key,
			char *value )
{
	COLUMN *column;
	int value_int;
	double value_double;

	column = search_column( table, group_name, column_name );
	if ( !column ) return 0;

	if ( column->datatype == DATATYPE_INTEGER )
	{
		value_int = atoi( value );
		set_value( table, group_name, column_name, key, &value_int );
	}
	else
	if ( column->datatype == DATATYPE_DOUBLE )
	{
		value_double = atof( value );
		set_value( table, group_name, column_name, key, &value_double );
	}
	else
	{
		set_value( table, group_name, column_name, key, value );
	}
	return 1;

} /* set_value_string() */

void set_value(	TABLE *table, 
		char *group_name,
		char *column_name,
		char *key,
		void *value )
{
	COLUMN *column;

	column = search_column( table, group_name, column_name );
	if ( !column ) exit_early( "set_value" );

	if ( strcmp( table->key_column_name, column_name ) == 0 )
	{
		append_key_column( table, value );

		/* No need to store value if only using it as a key */
		/* ------------------------------------------------ */
		if ( column->display_off ) return;
	}

	if ( column->datatype == DATATYPE_INTEGER )
	{
		dictionary_set(	column->value_dictionary, 
				key, 
				(int *)value, 
				sizeof( int ) );
	}
	else
	if ( column->datatype == DATATYPE_DOUBLE )
		dictionary_set(	column->value_dictionary, 
				key, 
				(double *)value, 
				sizeof( double ) );
	else
	if ( column->datatype == DATATYPE_STRING )
		dictionary_set(	column->value_dictionary, 
				key, 
				value, 
				strlen( value ) + 1 );
	else
		exit_early( "set_value" );

} /* set_value() */

void table_output( FILE *destination, TABLE *table )
{
	table_display( destination, table );
}

void table_display( FILE *destination, TABLE *table )
{
	COLUMN_GROUP *column_group;
	COLUMN *column;
	int spaces_between_column_groups = 0;
	int spaces_after_first_column_group = 0;
	int column_group_first_time;
	int row_number = 0;
	char **key_list = table->key_column_array;
	char *key;
	char old_key[ 128 ] = {0};

	/* Output the headings */
	/* ------------------- */
	if ( !table->heading_off )
	{
		spaces_between_column_groups = 
			get_spaces_between_column_groups(
					&spaces_after_first_column_group,
					table );
	
		output_title( destination, table->title, table->width );
		output_heading( destination, 
				table, 
				spaces_between_column_groups,
				spaces_after_first_column_group,
				table->suppress_output_on_any_empty_column );
	}

	/* Output the rows */
	/* --------------- */
	while( ( key = *key_list++ ) )
	{
		if ( !table->heading_off )
		{
			/* If another page is needed */
			/* ------------------------- */
			if ( row_number++ >= table->rows_per_page )
			{
				output_footer( destination, table );

				output_new_page( 
				   destination, 
				   table,
				   spaces_between_column_groups,
				   spaces_after_first_column_group,
				   table->suppress_output_on_any_empty_column );

				row_number = 1;
			}
	
			/* If grouping on length of the key */
			/* -------------------------------- */
			if ( table->group_key_length_skip )
			{
				if ( !*old_key )
				{
					strcpy( old_key, key );
				}
				else
				{
					if ( strlen( key ) != strlen( old_key ))
					{
					     int i;
					     for( i = 0; 
					     i < table->group_key_length_skip;
						     i++ )
					     {
						fprintf( destination, "\n" );
							row_number++;
					     }
					}
					strcpy( old_key, key );
				}
			}
		}

		column_group_first_time = 1;
		reset_get_column_group( table );
		while( ( column_group = get_next_column_group( table ) ) )
		{
			if ( table->suppress_output_on_any_empty_column
			&&   some_column_is_empty( column_group ) )
				continue;

			reset_get_column( column_group );
			while( ( column = get_next_column( column_group ) ) )
			{
				if ( !column->display_off )
					output_column( 	destination, 
							column, 
							key );
			}
			if ( !table->heading_off )
			{
				if ( column_group_first_time )
				{
					output_spaces_after_first_column_group(
					      destination,
					      spaces_after_first_column_group );
					column_group_first_time = 0;
				}
				if ( !at_last_column_group( table ) )
				{
					output_spaces_between_column_groups( 
						destination,
						spaces_between_column_groups );
				}
			}

		}
		fprintf( destination, "\n" );
	}

	if ( !table->heading_off )
	{
		output_footer( destination, table );
	}

} /* table_display() */

void output_new_page( 	FILE *destination, 
			TABLE *table,
			int spaces_between_column_groups,
			int spaces_after_first_column_group,
			int suppress_output_on_any_empty_column )
{
	char *title_to_output;

	title_to_output = (table->title_continued) ? 
			  table->title_continued : 
			  table->title;

	output_title( destination, title_to_output, table->width );
	output_heading( destination, 
			table, 
			spaces_between_column_groups,
			spaces_after_first_column_group,
			suppress_output_on_any_empty_column );
} /* output_new_page() */

void output_footer( 	FILE *destination, 
			TABLE *table )
{
	output_line( destination, table->width );
	if ( table->footer ) fprintf( destination, "%s\n", table->footer );
} /* output_footer() */

int get_spaces_between_column_groups( 	int *spaces_after_first_column_group,
					TABLE *table )
{
	COLUMN_GROUP *column_group;
	int sum_column_group_widths = 0;
	int number_column_groups = 0;
	int sum_column_widths;
	int return_value;

	*spaces_after_first_column_group = 1;
	reset_get_column_group( table );
	while( ( column_group = get_next_column_group( table ) ) )
	{
		if ( table->suppress_output_on_any_empty_column
		&&   some_column_is_empty( column_group ) )
			continue;

		sum_column_widths = 
			get_sum_column_widths( 	
				column_group, 
				table->suppress_output_on_any_empty_column );
		if ( sum_column_widths )
		{
			sum_column_group_widths += sum_column_widths;
			number_column_groups++;
		}
	}

	if ( number_column_groups < 1 ) return 0;

	return_value = ( (table->width - sum_column_group_widths) /
	       	          number_column_groups );

	*spaces_after_first_column_group = 
			( ( table->width - sum_column_group_widths) / 
			number_column_groups );
/*
fprintf( stderr, 
 "width = %d sum_column_group_widths = %d number_column_groups = %d ret = %d\n",
	 table->width,
	 sum_column_group_widths,
	 number_column_groups,
	 return_value );
fprintf( stderr, 
"width - sum_column_group_widths = %d spaces_after_first_column_group = %d\n\n",
	 table->width - sum_column_group_widths, 
	 *spaces_after_first_column_group );
*/
	return return_value;
} /* get_spaces_between_column_groups() */

void output_title( FILE *destination, char *title, int width )
{
	fprintf( destination, "%c", 12 );
	fprintf( destination, "%s\n", center( title, width ) );
} /* output_title() */

void output_heading(	FILE *destination, 
			TABLE *table, 
			int spaces_between_column_groups,
			int spaces_after_first_column_group,
			int suppress_output_on_any_empty_column )
{
	output_line( destination, table->width );
	output_column_group_heading( 	destination, 
					table,
					spaces_between_column_groups,
					spaces_after_first_column_group,
				  	suppress_output_on_any_empty_column );

	output_column_heading( 		destination, 
					table,
					spaces_between_column_groups,
					spaces_after_first_column_group,
				  	suppress_output_on_any_empty_column );
	output_line( destination, table->width );
	fprintf( destination, "\n" );
} /* output_heading() */

void output_column_group_heading( FILE *destination, 
				  TABLE *table,
				  int spaces_between_column_groups,
				  int spaces_after_first_column_group,
				  int suppress_output_on_any_empty_column )
{
	COLUMN_GROUP *column_group;
	int column_group_first_time = 1;

	reset_get_column_group( table );
	while( ( column_group = get_next_column_group( table ) ) )
	{
		if ( suppress_output_on_any_empty_column
		&&   some_column_is_empty( column_group ) )
			continue;

		fprintf( destination,
			 "%s",
			 center_rpad( 
				    column_group->heading,
			 	    get_sum_column_widths( column_group,
			            suppress_output_on_any_empty_column ) ) );

		if ( column_group_first_time )
		{
			output_spaces_after_first_column_group(
			      destination,
			      spaces_after_first_column_group );
			column_group_first_time = 0;
		}
		if ( !at_last_column_group( table ) )
			output_spaces_between_column_groups( 
					destination,
					spaces_between_column_groups );
	}
	fprintf( destination, "\n" );
} /* output_column_group_heading() */

void output_column_heading( 	FILE *destination, 
				TABLE *table,
				int spaces_between_column_groups,
				int spaces_after_first_column_group,
				int suppress_output_on_any_empty_column )
{
	COLUMN_GROUP *column_group;
	COLUMN *column;
	char format_string[ 64 ];
	int column_group_first_time = 1;

	reset_get_column_group( table );
	while( ( column_group = get_next_column_group( table ) ) )
	{ 
		if ( suppress_output_on_any_empty_column
		&&   some_column_is_empty( column_group ) )
			continue;

		reset_get_column( column_group );
		while( ( column = get_next_column( column_group ) ) )
		{
			if ( column->display_off ) continue;

			sprintf( format_string, 
			 "%c%c%ds",
			 '%',
			 column->justify_plus_minus,
			 get_column_width( column ) );

			fprintf( destination,
			 	 format_string,
			 	 column->heading );
		}
		if ( column_group_first_time )
		{
			output_spaces_after_first_column_group(
			      destination,
			      spaces_after_first_column_group );
			column_group_first_time = 0;
		}
		if ( !at_last_column_group( table ) )
			output_spaces_between_column_groups( 
					destination,
					spaces_between_column_groups );
	}
	fprintf( destination, "\n" );
} /* output_column_heading() */

void output_spaces_between_column_groups( FILE *destination,
					  int spaces_between_column_groups )
{
	if ( spaces_between_column_groups <= 0 ) return;
	while( spaces_between_column_groups-- ) fprintf( destination, " " );
}

void output_spaces_after_first_column_group( FILE *destination,
				       int spaces_after_first_column_group )
{
	if ( spaces_after_first_column_group <= 0 ) return;
	while( spaces_after_first_column_group-- ) fprintf( destination, " " );
}

void output_line( FILE *destination, int width )
{
	while( width-- ) fprintf( destination, "_" );
	fprintf( destination, "\n" );
} /* output_line() */

int get_sum_column_widths( COLUMN_GROUP *column_group,
			   int suppress_output_on_any_empty_column )
{
	COLUMN *column;
	int sum = 0;

	if ( suppress_output_on_any_empty_column
	&&   some_column_is_empty( column_group ) )
		return 0;

	reset_get_column( column_group );
	while( ( column = get_next_column( column_group ) ) )
		if ( !column->display_off )
			sum += get_column_width( column );
	return sum;
} /* get_sum_column_widths() */

int some_column_is_empty( COLUMN_GROUP *column_group )
{
	COLUMN *column;

	reset_get_column( column_group );
	while( ( column = get_next_column( column_group ) ) )
	{
		if ( !dictionary_len( column->value_dictionary ) )
		{
			return 1;
		}
		if ( dictionary_all_null_data( column->value_dictionary ) )
		{
fprintf( stderr, "found a set-all-blank column: %s.%s\n",
column_group->name, column->name );
			return 1;
		}
	}
	return 0;
} /* some_column_is_empty() */

int get_column_width( COLUMN *column )
{
	int str_len = strlen( column->heading );
	if ( column->width < str_len )
		return str_len + 1;
	else
		return column->width + 1;
} /* get_column_width() */


COLUMN *search_column( TABLE *table, char *group_name, char *column_name )
{
	COLUMN_GROUP *column_group;
	COLUMN *column;

	/* First check with group compare */
	/* ------------------------------ */
	reset_get_column_group( table );
	while( ( column_group = get_next_column_group( table ) ) )
	{
		if ( strcmp( column_group->name, group_name ) != 0 ) continue;
		reset_get_column( column_group );
		while( ( column = get_next_column( column_group ) ) )
			if ( strcmp( column->name, column_name ) == 0 )
				return column;
	}

	/* Second check for key without group compare */
	/* ------------------------------------------ */
	reset_get_column_group( table );
	while( ( column_group = get_next_column_group( table ) ) )
	{
		reset_get_column( column_group );
		while( ( column = get_next_column( column_group ) ) )
			if ( strcmp( 	table->key_column_name, 
					column_name ) == 0 )
			{
				return column;
			}
	}

	return (COLUMN *)0;
} /* search_column() */

void set_footer( TABLE *table, char *footer )
{
	table->footer = strdup( footer );
} /* set_footer() */

void set_title_continued( TABLE *table, char *title_continued )
{
	table->title_continued = strdup( title_continued );
} /* set_title_continued() */

/* Operations on COLUMN_GROUP */
/* -------------------------- */
COLUMN_GROUP *column_group_new( char *name, char *heading )
{
	COLUMN_GROUP *a = (COLUMN_GROUP *)calloc( 1, sizeof( COLUMN_GROUP ) );
	a->name = strdup( name );
	a->heading = strdup( heading );
	return a;
}

int exists_some_data( COLUMN_GROUP *column_group )
{
	COLUMN *column;
	reset_get_column( column_group );
	while( ( column = get_next_column( column_group ) ) )
		if ( dictionary_len( column->value_dictionary ) )
			return 1;
	return 0;
} /* exists_some_data() */


int reset_get_column( COLUMN_GROUP *column_group )
{
	if ( !column_group->number_columns )
		return 0;
	else
	{
		column_group->current_column = 0;
		return 1;
	}

} /* reset_get_column() */

COLUMN *get_next_column( COLUMN_GROUP *column_group )
{
	if ( column_group->current_column == column_group->number_columns )
		return (COLUMN *)0;
	else
		return 
		column_group->column_array[ column_group->current_column++ ];
} /* get_next_column() */

COLUMN *get_column( COLUMN_GROUP *column_group )
{
	return get_next_column( column_group );
}

/* Operations on COLUMN */
/* -------------------- */
COLUMN *column_new( 	char *column_name, 
			int datatype,
			char *heading, 
			char justify_plus_minus, 
			int width, 
			int precision,
			char *default_value )
{
	COLUMN *c = (COLUMN *)calloc( 1, sizeof( COLUMN ) );
	c->name = strdup( column_name );
	c->datatype = datatype;
	c->heading = strdup( heading );
	c->width = width;
	c->precision = precision;
	c->value_dictionary = dictionary_new();
	c->default_value = strdup( default_value );

	if ( !justify_plus_minus || justify_plus_minus == '+' )
		c->justify_plus_minus = ' ';
	else
		c->justify_plus_minus = justify_plus_minus;

	return c;
} /* column_new() */

void output_column( FILE *destination, COLUMN *column, char *key )
{
	char format_string[ 64 ];
	char format_code[3];
	char *value_ptr;
	int precision = column->precision;
	int datatype_were_using = column->datatype;

	value_ptr = dictionary_get( column->value_dictionary, key );
	if ( !value_ptr )
	{
		value_ptr = column->default_value;
		datatype_were_using = DATATYPE_STRING;
		precision = 0;
	}

	if ( datatype_were_using == DATATYPE_INTEGER )
	{
		format_code[ 0 ] = 'd';
		format_code[ 1 ] = '\0';
	}
	else
	if ( datatype_were_using == DATATYPE_DOUBLE )
	{
		format_code[ 0 ] = 'l';
		format_code[ 1 ] = 'f';
		format_code[ 2 ] = '\0';
	}
	else
	if ( datatype_were_using == DATATYPE_STRING )
	{
		format_code[ 0 ] = 's';
		format_code[ 1 ] = '\0';
	}

	if ( precision )
		sprintf( format_string, 
			 "%c%c%d.%d%s",
			 '%',
			 column->justify_plus_minus,
			 get_column_width( column ),
			 precision,
			 format_code );
	else
		sprintf( format_string, 
			 "%c%c%d%s",
			 '%',
			 column->justify_plus_minus,
			 get_column_width( column ),
			 format_code );

	if ( datatype_were_using == DATATYPE_INTEGER )
		fprintf( destination, format_string, *(int *)value_ptr );
	else
	if ( datatype_were_using == DATATYPE_DOUBLE )
		fprintf( destination, format_string, *(double *)value_ptr );
	else
	if ( datatype_were_using == DATATYPE_STRING )
		fprintf( destination, format_string, value_ptr );
	else
		exit_early( "output_column" );
} /* output_column() */

/* Misc functions */
/* -------------- */
void exit_early( char *from_where )
{
	fprintf( stderr, "Exiting early from %s\n", from_where );
	exit( 1 );
} /* exit_early() */

int search_fn_array( char *label )
{
	int i;

	for( i = 0; fn_array[ i ].fn_name; i++ )
		if( strcmp( fn_array[ i ].fn_name, label ) == 0 )
			return i;
	return -1;
} /* search_fn_array() */

int datatype_string2int( char *datatype )
{
	if ( strcmp( datatype, "integer" ) == 0 )
		return DATATYPE_INTEGER;
	else
	if ( strcmp( datatype, "string" ) == 0 )
		return DATATYPE_STRING;
	else
	if ( strcmp( datatype, "double" ) == 0 )
		return DATATYPE_DOUBLE;
	else
		exit_early( "datatype_string2int" );
	return 0;
} /* datatype_string2int() */
