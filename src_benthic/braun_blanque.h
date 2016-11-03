/* ---------------------------------------------------	*/
/* src_benthic/braun_blanque.h				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef BRAUN_BLANQUE_H
#define BRAUN_BLANQUE_H

/* Includes */
/* -------- */
#include "list.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define BRAUN_BLANQUE_STRING_SIZE	128
#define BRAUN_BLANQUE_SUBSTRATE_KEY	"substrate"
#define BRAUN_BLANQUE_SEAGRASS		"seagrass"

/* Structures */
/* ---------- */

typedef struct
{
	char *substrate;
	char *substrate_code;
} SUBSTRATE;

typedef struct
{
	char *vegetation_group_name;
	char *heading_label;
} VEGETATION_GROUP;

typedef struct
{
	char *vegetation_name;
	char is_height_yn;
	char *heading_label;
	VEGETATION_GROUP *vegetation_group;
} VEGETATION;

typedef struct
{
	double value;
} VALUE;

typedef struct
{
	double value_sum;
	int count;
} VEGETATION_GROUP_VALUE;

typedef struct
{
	int collection_number;
	int location_number;
	char collection_name[ BRAUN_BLANQUE_STRING_SIZE	];
	char location[ BRAUN_BLANQUE_STRING_SIZE ];
	char actual_latitude[ BRAUN_BLANQUE_STRING_SIZE ];
	char actual_longitude[ BRAUN_BLANQUE_STRING_SIZE ];
	char region_code[ BRAUN_BLANQUE_STRING_SIZE ];
	int pink_assessment_area;
	int site_number;
	int quad;
	VALUE **value_array;
	VEGETATION_GROUP_VALUE **group_value_array;
	LIST *substrate_list;
	int season;
	double max_height;
	double average_height;
} OUTPUT_RECORD;

typedef struct
{
	int collection_number;
	int location_number;
	char collection_name[ BRAUN_BLANQUE_STRING_SIZE	];
	char location[ BRAUN_BLANQUE_STRING_SIZE ];
	char actual_latitude[ BRAUN_BLANQUE_STRING_SIZE ];
	char actual_longitude[ BRAUN_BLANQUE_STRING_SIZE ];
	char region_code[ BRAUN_BLANQUE_STRING_SIZE ];
	int pink_assessment_area;
	int site_number;
	int quad;
	char vegetation_name[ BRAUN_BLANQUE_STRING_SIZE ];
	char is_height_yn;
	double value;
	int season;
} INPUT_RECORD;

typedef struct
{
	INPUT_RECORD *input_record;
	OUTPUT_RECORD *output_record;
	LIST *vegetation_list;
	LIST *master_substrate_list;
	LIST *vegetation_group_list;
	VEGETATION **vegetation_array;
} BRAUN_BLANQUE;

/* Prototypes */
/* ---------- */
BRAUN_BLANQUE *braun_blanque_new(
				char *application_name );

VEGETATION *braun_blanque_vegetation_new(
				char *vegetation_name,
				char is_height_yn,
				char *heading_label,
				VEGETATION_GROUP *group );

LIST *braun_blanque_get_vegetation_list(
				char *application_name,
				LIST *vegetation_group_list );

LIST *braun_blanque_get_total_vegetation_list(
				char *application_name );

LIST *braun_blanque_get_seagrass_vegetation_list(
				char *application_name,
				LIST *vegetation_group_list );

LIST *braun_blanque_get_non_seagrass_vegetation_list(
				char *application_name,
				LIST *vegetation_group_list );

LIST *braun_blanque_get_substrate_list(
					char *application_name );

LIST *braun_blanque_get_vegetation_group_list(
					char *application_name );

char *braun_blanque_group_heading_label_list_display(
				LIST *vegetation_group_list );

char *braun_blanque_heading_label_list_display(
				LIST *vegetation_list );

LIST *braun_blanque_get_database_vegetation_list(
				char *application_name,
				LIST *static_vegetation_list );

void braun_blanque_append_vegetation_list(
				LIST *vegetation_list,
				char *vegetation_name,
				char is_height_yn,
				char *heading_label,
				VEGETATION_GROUP *group );

VEGETATION *braun_blanque_append_vegetation_seek(
				LIST *vegetation_list,
				char *vegetation_name,
				char is_height_yn );

boolean braun_blanque_vegetation_exists(
				LIST *vegetation_list,
				char *vegetation_name,
				char is_height_yn );

boolean braun_blanque_set_input_buffer(
				FILE *output_file,
				OUTPUT_RECORD *output_record,
				INPUT_RECORD *input_record,
				char *input_buffer,
				LIST *vegetation_list,
				LIST *vegetation_group_list,
				LIST *master_substrate_list,
				VEGETATION **vegetation_array,
				boolean avg_quads_boolean );

OUTPUT_RECORD *braun_blanque_output_record_new(
				int length_vegetation_list,
				int length_vegetation_group_list );

INPUT_RECORD *braun_blanque_input_record_new(
				void );

char *braun_blanque_get_key(	char *collection_name,
				char *location,
				int site_number,
				int quad );

void braun_blanque_set_group_value_array(
				VEGETATION_GROUP_VALUE **group_value_array,
				char *vegetation_group_name,
				double value_double,
				LIST *vegetation_group_list );

void braun_blanque_set_value_array(
				VALUE **value_array,
				char *vegetation_name,
				double value_double,
				LIST *vegetation_list,
				char is_height_yn );

int braun_blanque_get_output_record_value_index(
				char *vegetation_name,
				LIST *vegetation_list,
				char is_height_yn );

VEGETATION_GROUP_VALUE **braun_blanque_group_value_array_new(
				int length_vegetation_group_list );

VALUE **braun_blanque_value_array_new(
				int length_vegetation_list );

VALUE *braun_blanque_value_new(	double value_double );

void braun_blanque_set_output_anchor(
				OUTPUT_RECORD *output_record,
				char *collection_name,
				char *location,
				int site_number,
				int quad );

void braun_blanque_output_record(
				FILE *output_file,
				OUTPUT_RECORD *output_record,
				int length_vegetation_list,
				int length_vegetation_group_list,
				VEGETATION **vegetation_array,
				boolean avg_quads_boolean );

void braun_blanque_clear_group_value_array(
				VEGETATION_GROUP_VALUE **group_value_array,
				int length_vegetation_group_list );

void braun_blanque_clear_value_array(
				VALUE **value_array,
				int length_vegetation_list );

boolean braun_blanque_is_height_yn_match(
				char is_height_yn1,
				char is_height_yn2 );

SUBSTRATE *braun_blanque_substrate_new(
				char *substrate_name,
				char *substrate_code );

VEGETATION_GROUP *braun_blanque_vegetation_group_new(
				char *vegetation_group_name,
				char *heading_label );

char *braun_blanque_or_sequence_get_where_clause(
				char *application_name,
				LIST *collection_list,
				LIST *project_list );

LIST *braun_blanque_clear_substrate_list(
				LIST *substrate_list );

SUBSTRATE *braun_blanque_substrate_seek(
				LIST *master_substrate_list,
				char *substrate_name );

char *braun_blanque_get_substrate_display(
				LIST *substrate_list );

void braun_blanque_set_output_record(
				OUTPUT_RECORD *output_record,
				INPUT_RECORD *input_record,
				LIST *vegetation_list,
				LIST *vegetation_group_list );

VEGETATION_GROUP *braun_blanque_vegetation_group_seek(
				char *vegetation_group_name,
				LIST *vegetation_group_list );

int braun_blanque_get_vegetation_group_index(
				char *vegetation_group_name,
				LIST *vegetation_group_list );

VEGETATION_GROUP_VALUE *braun_blanque_vegetation_group_value_new(
				void );

char *braun_blanque_vegetation_list_display(
				LIST *vegetation_list );

char *braun_blanque_vegetation_group_display(
				VEGETATION_GROUP *vegetation_group );

VEGETATION *braun_blanque_get_vegetation_seek(
				LIST *vegetation_list,
				char *vegetation_name );

char *braun_blanque_get_vegetation_group_name(
				LIST *vegetation_list,
				char *vegetation_name );

double braun_blanque_get_max_avg_height(
				double *average_height,
				VALUE **value_array,
				LIST *vegetation_list,
				char *seagrass_group );

VEGETATION **braun_blanque_vegetation_array_new(
				LIST *vegetation_list );

void braun_blanque_clear_aggregate_values(
				OUTPUT_RECORD *output_record );

#endif

