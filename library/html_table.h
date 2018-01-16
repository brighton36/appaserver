/* html_table.h 							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver html_table ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef HTML_TABLE_H
#define HTML_TABLE_H

#include "list.h"

/* Objects */
/* ------- */
typedef struct
{
	char *title;
	char *sub_title;
	char *sub_sub_title;
	LIST *heading_list;
	LIST *data_list;
	LIST *justify_list;
	int number_left_justified_columns;
	int number_right_justified_columns;
	int background_shaded;
} HTML_TABLE;

/* Operations */
/* ---------- */
HTML_TABLE *new_html_table(		char *title,
					char *sub_title );

HTML_TABLE *html_table_new(		char *title,
					char *sub_title,
					char *sub_sub_title );

void html_table_output( 		HTML_TABLE *d );
void html_table_set_data_heading( 	HTML_TABLE *d, 
					char *heading );
void html_table_set_data( 		LIST *data_list, char *data );
void html_table_set_record_data(	HTML_TABLE *d,
					char *record_data,
					char delimiter );
void html_table_close( 			void );
void html_table_set_number_left_justified_columns(
					HTML_TABLE *d, int i );
void html_table_set_number_right_justified_columns(
					HTML_TABLE *d, int i );
void html_table_reset_data_heading( 	HTML_TABLE *d );

void html_table_set_heading( 		HTML_TABLE *d,
					char *heading );

void html_table_set_heading_list( 	HTML_TABLE *d,
					LIST *heading_list );

void html_table_set_background_shaded(	HTML_TABLE *d );
void html_table_set_background_unshaded(HTML_TABLE *d );
char *html_table_get_alignment(		int column_number,
					int number_left_justified_columns,
					int number_right_justified_columns );
void html_table_output_table_heading(	char *title,
					char *sub_title );
void html_table_output_table_heading2(	char *title,
					char *sub_title,
					char *sub_sub_title );
void html_table_output_data_heading( 	LIST *heading_list,
					int number_left_justified_columns,
					int number_right_justified_columns,
					LIST *justify_list );
void html_table_output_data( 		LIST *data_list,
					int number_left_justified_columns,
					int number_right_justified_columns,
					int background_shaded,
					LIST *justify_list );
#endif
