/* insert_table.h							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver insert_table ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* This is not in the public domain.					*/
/* -------------------------------------------------------------------- */


#ifndef INSERT_TABLE_H
#define INSERT_TABLE_H

#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "widget.h"
#include "widget_list.h"

typedef struct
{
	char *customer;
	char *application;
	int insert_rows_number;
	DICTIONARY *post_dictionary;
	FOLDER *folder;
	WIDGET_LIST *widget_list;
} INSERT_TABLE;

/* Operations */
/* ---------- */
INSERT_TABLE *insert_table_new( 	char *customer,
					char *application,
					char *folder,
					int insert_rows_number,
					DICTIONARY *post_dictionary,
					char *decoded_post_dictionary_string );

LIST *insert_table_get_widget_list(	INSERT_TABLE *insert_table );
void insert_table_set_input_data_dictionary(	INSERT_TABLE *insert_table,
						DICTIONARY *d );
LIST *insert_table_fetch_data_dictionary_list(	char *customer,
						char *application,
						FOLDER *folder );
LIST *insert_table_build_widget_list(		INSERT_TABLE *insert_table );
void insert_table_output_headings(		INSERT_TABLE *insert_table );
void insert_table_output_table(			INSERT_TABLE *insert_table );
void insert_table_output_row( 			LIST *widget_list, 
						DICTIONARY *data_dictionary,
						int row );
char *insert_table_get_current_value(		DICTIONARY *data_dictionary,
						char *name );
int insert_table_output_empty_rows( 		INSERT_TABLE *insert_table, 
						int starting_row );
#endif

