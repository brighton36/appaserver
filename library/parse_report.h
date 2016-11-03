/* parse_report.h 					*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PARSE_REPORT_H
#define PARSE_REPORT_H

#include "list.h"
#include "boolean.h"

typedef struct
{
	char heading[ 512 ];
	int beginning_offset;
	char *data;
} CELL;
 
typedef struct
{
	LIST *cell_list;
	LIST *heading_string_list;
} PARSE_REPORT;

/* Prototypes */
/* ---------- */
void parse_report_skip_heading(		void );

void parse_report_populate_cell_data(
					LIST *cell_list,
					char *buffer );

void parse_report_populate_heading_row(	LIST *cell_list,
					char *buffer,
					boolean with_append );

LIST *parse_report_load_heading_string_list(
					char *buffer );

void parse_report_display_cell_list(	LIST *column_list,
					char delimiter );

CELL *parse_report_new_cell(		void );

PARSE_REPORT *parse_report_new_parse_report(
					void );

LIST *parse_report_get_cell_list(
					LIST *heading_string_list,
					char *buffer );

void parse_report_populate_heading(	LIST *cell_list,
					LIST *heading_string_list );

#endif
