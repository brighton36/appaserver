/* prompt_table.c							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver prompt_table ADT.				*/
/*									*/
/* Copyright 1999 by Appahost						*/
/* -------------------------------------------------------------------- */


#include <stdio.h>
#include <string.h>
#include "prompt_table.h"
#include "attribute.h"
#include "appaserver_library.h"
#include "folder.h"
#include "widget.h"
#include "widget_list.h"

PROMPT_TABLE *prompt_table_new( char *customer,
				char *application,
				char *folder_name )
{
	PROMPT_TABLE *prompt_table;

	prompt_table = (PROMPT_TABLE *) calloc( 1, sizeof( PROMPT_TABLE ) );

	prompt_table->customer = strdup( customer );
	prompt_table->application = strdup( application );

	/* Do administrative work */
	/* ---------------------- */
	prompt_table->folder = folder_new( 	customer,
						application,
						folder_name,
						(DICTIONARY *)0 );

	prompt_table->widget_list =
		widget_list_new(	prompt_table->customer,
					prompt_table->application,
					prompt_table->folder->name,
					prompt_table->
						folder->
						attribute_hash_table,
					folder_get_attribute_name_list( 
						prompt_table->folder ),
					folder_get_related_1tom_folder_list(
						prompt_table->customer,
						prompt_table->application,
						prompt_table->folder ),
					(DICTIONARY *)0,
					(char *)0,	/* decoded_post_*/
							/* dictionary_  */
							/* string	*/
					1 		/* output_pairs */ );
	return prompt_table;
} /* prompt_table_new() */

void prompt_table_output_table( PROMPT_TABLE *prompt_table )
{
	widget_list_output_row( prompt_table->widget_list,
				(DICTIONARY *)0, 
				0 /* row */ );
} /* prompt_table_output_table() */

