/* ---------------------------------------------------	*/
/* src_benthic_spss/benthic_spss.h			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef BENTHIC_SPSS_H
#define BENTHIC_SPSS_H

#include "benthic_species.h"
#include "sampling_point.h"
#include "list.h"
#include "hash_table.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define SPSS_KEY_SIZE					256

#define BENTHIC_SPSS_COLLECTION_NUMBER_PIECE		0
#define BENTHIC_SPSS_LOCATION_NUMBER_PIECE		1
#define BENTHIC_SPSS_COLLECTION_NAME_PIECE		2
#define BENTHIC_SPSS_LOCATION_PIECE			3
#define BENTHIC_SPSS_REGION_CODE_PIECE			4
#define BENTHIC_SPSS_PINK_CODE_PIECE			5
#define BENTHIC_SPSS_SITE_NUMBER_PIECE			6
#define BENTHIC_SPSS_SWEEP_NUMBER_PIECE			7
#define BENTHIC_SPSS_NO_SUM_SCIENTIFIC_NAME_PIECE	8
#define BENTHIC_SPSS_NO_SUM_COUNT_PIECE			9

/* Structures */
/* ---------- */
typedef struct
{
	BENTHIC_SPECIES *root_benthic_species;
	int count_per_square_meter;
} SPSS_SPECIES_COUNT;

typedef struct
{
	char *sampling_point_key;
	SAMPLING_POINT *root_sampling_point;
	int sweep_number;
	LIST *species_count_list;
} SPSS_SAMPLING_POINT;

typedef struct
{
	char *faunal_group_name;
	LIST *benthic_species_list;
	int faunal_group_row_total;
} SPSS_FAUNAL_GROUP;

typedef struct
{
	SPSS_SAMPLING_POINT *spss_sampling_point;
	LIST *faunal_group_list;
	FILE *input_file;
/*
	HASH_TABLE *spss_species_count_hash_table;
*/
} BENTHIC_SPSS;

/* Prototypes */
/* ---------- */
BENTHIC_SPSS *benthic_spss_new(
				char *application_name,
				LIST *faunal_group_name_list,
				char *select_output_filename );

BENTHIC_SPSS *benthic_spss_spss_ready_counts_new(
				char *application_name,
				LIST *faunal_group_name_list );

LIST *benthic_spss_get_faunal_group_list(
				char *application_name,
				LIST *faunal_group_name_list );

char *benthic_spss_get_sampling_point_key(
				char *collection_name,
				char *location,
				int site_number,
				int sweep_number,
				char sum_sweeps_yn );

LIST *benthic_spss_get_faunal_group_record_list(
				char *application_name );


BENTHIC_SPECIES *benthic_spss_benthic_species_seek(
				LIST *faunal_group_list,
				char *scientific_name );

SPSS_SPECIES_COUNT *benthic_spss_species_count_new(
				BENTHIC_SPECIES *root_benthic_species,
				int count_per_square_meter );

SPSS_SAMPLING_POINT *benthic_spss_sampling_point_seek(
				LIST *spss_sampling_point_list,
				char *sampling_point_key );

SPSS_SPECIES_COUNT *benthic_spss_species_count_seek(
				LIST *species_count_list,
				char *scientific_name );

SPSS_SAMPLING_POINT *benthic_spss_sampling_point_new(
				char *sampling_point_key,
				char *collection_number,
				char *location_number,
				char *collection_name,
				char *location,
				char *region_code,
				char *pink_assessment_area,
				char *site_number,
				char *sweep_number,
				LIST *faunal_group_list,
				char *scientific_name,
				char *count,
				boolean no_animals_found );

SPSS_FAUNAL_GROUP *benthic_spss_faunal_group_new(
				char *faunal_group_name );

char *benthic_spss_get_heading_string(
				LIST *faunal_group_list,
				char sum_sweeps_yn );

char *benthic_spss_get_common_name_label_string(
				LIST *faunal_group_list );

char *benthic_spss_get_scientific_name_label_string(
				LIST *faunal_group_list );

char *benthic_spss_get_sampling_point_output_line(
				SPSS_SAMPLING_POINT *spss_sampling_point,
				LIST *faunal_group_list,
				char sum_sweeps_yn );

char *benthic_spss_sampling_point_display_count_list(
				LIST *species_count_list,
				LIST *faunal_group_list );

void benthic_spss_calculate_faunal_group_row_total(
				LIST *faunal_group_list,
				LIST *species_count_list );

char *benthic_spss_sampling_point_display(
				SPSS_SAMPLING_POINT *spss_sampling_point );

int benthic_spss_get_season_number(
				char *collection_name );

SPSS_SAMPLING_POINT *benthic_spss_get_spss_sampling_point(
				int *all_done,
				FILE *input_file,
				LIST *faunal_group_list,
				char sum_sweeps_yn );

boolean benthic_spss_parse_input_buffer(
				char *collection_number,
				char *location_number,
				char *collection_name,
				char *location,
				char *region_code,
				char *pink_code,
				char *site_number,
				char *sweep_number,
				char *scientific_name,
				char *count,
				char *input_buffer,
				char sum_sweeps_yn );

void benthic_spss_append_species_count(
				LIST *species_count_list,
				LIST *faunal_group_list,
				char *scientific_name,
				char *count,
				boolean no_animals_found );

void benthic_spss_append_species_count(
				LIST *species_count_list,
				LIST *faunal_group_list,
				char *scientific_name,
				char *count,
				boolean no_animals_found );

void benthic_spss_sampling_point_free(
				SPSS_SAMPLING_POINT *spss_sampling_point );

void benthic_reset_old_sampling_point_key(
				void );

#endif
