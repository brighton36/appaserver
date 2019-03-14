/* src_hydrology/datatype.h				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef DATATYPE_H
#define DATATYPE_H

#include <string.h>
#include <ctype.h>
#include "aggregate_statistic.h"
#include "appaserver_library.h"
#include "boolean.h"
#include "list.h"
#include "timlib.h"
#include "piece.h"
#include "units.h"

/* Structures */
/* ---------- */
typedef struct
{
	char *datatype_alias;
	char *datatype_name;
} DATATYPE_ALIAS;

typedef struct
{
	char *datatype_name;
	UNITS *units;
	boolean bar_chart;
	boolean scale_graph_to_zero;
	boolean aggregation_sum;
	boolean set_negative_values_to_zero;
	boolean calibrated;
	char *ysi_load_heading;
	char *exo_load_heading;
	int column_piece;
} DATATYPE;

/* Prototypes */
/* ---------- */
DATATYPE *datatype_new_datatype(
			char *datatype_name,
			char *units_name );

LIST *datatype_get_datatype_list(
			char *application_name,
			char *station,
			char plot_for_station_check_yn,
			enum aggregate_statistic );

LIST *datatype_list_get(
			char *application_name );

LIST *datatype_get_list(
			char *application_name );

char *datatype_get_units_string(
			boolean *bar_graph,
			char *application_name,
			char *datatype_name );

DATATYPE *datatype_unit_record2datatype(
			char *record );

DATATYPE *datatype_record2datatype(
			char *record );

boolean datatype_list_exists(
			LIST *datatype_list,
			char *datatype_name );

LIST *datatype_with_station_name_list_get_datatype_list(
			char *application_name,
			LIST *station_name_list );

LIST *datatype_with_station_name_get_datatype_list(
			char *application_name,
			char *station_name );

char *datatype_list_display(
			LIST *datatype_list );

LIST *datatype_with_station_name_list_get_datatype_bar_graph_list(
			char *application_name,
			LIST *station_name_list );

boolean datatype_bar_chart(
			char *application_name,
			char *datatype_name );

DATATYPE *datatype_list_seek(
			LIST *datatype_list,
			char *datatype_name );

boolean datatype_get_bypass_data_collection_frequency(
				char *application_name,
				char *station,
				char *datatype );

boolean datatype_bypass_data_collection_frequency(
				char *application_name,
				char *station,
				char *datatype );

DATATYPE *datatype_list_ysi_load_heading_seek(
				LIST *datatype_list,
				char *two_line_datatype_heading );

DATATYPE *datatype_list_exo_load_heading_seek(
				LIST *datatype_list,
				char *two_line_datatype_heading );

LIST *datatype_list_get_unique_unit_list(
				LIST *datatype_list );

LIST *datatype_get_datatypes_for_unit(
				LIST *datatype_list,
				char *unit );

DATATYPE *datatype_calloc(	void );

LIST *datatype_fetch_datatype_alias_list(
				char *application_name );

DATATYPE_ALIAS *datatype_alias_new(
				void );

DATATYPE_ALIAS *datatype_alias_seek(
				LIST *datatype_alias_list,
				char *datatype_alias_name );

DATATYPE *datatype_parse_new(
				char *application_name,
				LIST *input_datatype_list,
				char *column_heading );

DATATYPE *datatype_fetch_new(
				char *application_name,
				char *datatype_name,
				char *units_name,
				LIST *input_datatype_list );

void datatype_free(		DATATYPE *datatype );

#endif
