/* prompt_table.h							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver prompt_table ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* This is not in the public domain.					*/
/* -------------------------------------------------------------------- */


#ifndef PROMPT_TABLE_H
#define PROMPT_TABLE_H

#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "widget.h"
#include "widget_list.h"

typedef struct
{
	char *customer;
	char *application;
	FOLDER *folder;
	WIDGET_LIST *widget_list;
} PROMPT_TABLE;

/* Operations */
/* ---------- */
PROMPT_TABLE *prompt_table_new( 		char *customer,
						char *application,
						char *folder_name );
LIST *prompt_table_get_widget_list(		PROMPT_TABLE *prompt_table );
LIST *prompt_table_build_widget_list(	PROMPT_TABLE *prompt_table );
void prompt_table_output_table(			PROMPT_TABLE *prompt_table );
#endif

