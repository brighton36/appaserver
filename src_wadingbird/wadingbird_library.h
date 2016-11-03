/* src_wadingbird/wadingbird_library.h */
/* ----------------------------------- */

#ifndef WADINGBIRD_LIBRARY_H
#define WADINGBIRD_LIBRARY_H

/* Constants */
/* --------- */
#define TABLET_GREAT_EGRET_COUNT_PIECE			1
#define TABLET_WHITE_IBIS_COUNT_PIECE			2
#define TABLET_GREAT_BLUE_HERON_COUNT_PIECE		3
#define TABLET_SMALL_WHITE_HERON_COUNT_PIECE		4
#define TABLET_SMALL_DARK_HERON_COUNT_PIECE		5
#define TABLET_WOOD_STORK_COUNT_PIECE			6
#define TABLET_ROSEATE_SPOONBILL_COUNT_PIECE		7
#define TABLET_GLOSSY_IBIS_COUNT_PIECE			8
#define TABLET_GREAT_WHITE_HERON_COUNT_PIECE		9
#define TABLET_LEFT_RIGHT_CODE_PIECE			10
#define TABLET_TRANSECT_PIECE				11
#define TABLET_HYDROPATTERN_PIECE			12
#define TABLET_INITIAL_TAKEOFF_DATE_PIECE		13
#define TABLET_LATITUDE_PIECE				34
#define TABLET_LONGITUDE_PIECE				35
#define TABLET_CELL_NUMBER_PIECE			40

#define GREAT_EGRET_SPECIES_CODE			1
#define GREAT_WHITE_HERON_SPECIES_CODE			5
#define GREAT_BLUE_HERON_SPECIES_CODE			6
#define WHITE_IBIS_SPECIES_CODE				9
#define GLOSSY_IBIS_SPECIES_CODE			10
#define WOOD_STORK_SPECIES_CODE				11
#define SMALL_WHITE_HERON_SPECIES_CODE			14
#define SMALL_DARK_HERON_SPECIES_CODE			16
#define ROSEATE_SPOONBILL_SPECIES_CODE			20

#define SRF_FIELD_LIST "initial_takeoff_date,non_conventional_yn,preferred_hydropattern_seat"
#define BIRD_OBSERVATION_FIELD_LIST "initial_takeoff_date,utm_easting,utm_northing,common_name,left_right,bird_count,bird_estimation,cell_number"
#define CELL_HYDROPATTERN_FIELD_LIST "initial_takeoff_date,cell_number,left_right,hydropattern"
#define PREFERRED_HYDROPATTERN_FIELD_LIST "initial_takeoff_date,left_right"

#define BIRD_OBSERVATION_JUSTIFY_COMMA_LIST	"left,right,right,left,left,right,right"
#define CELL_HYDROPATTERN_JUSTIFY_COMMA_LIST	"left,right,left,left"

/* Structures */
/* ---------- */
typedef struct
{
	LIST *cell_list;
	char *preferred_hydropattern_seat;
} HYDROPATTERN_MERGE;

typedef struct
{
	int cell_number;
	char *hydropattern;
	int hydropattern_numeric_code;
	char preferred_hydropattern_yn;
	int bird_count;
	char *left_right;
	int utm_easting;
	int utm_northing;
} SRF_CELL;

typedef struct
{
	int cell_number;
	int transect_number;
} WADINGBIRD_CELL;

typedef struct
{
	int transect_number;
	boolean cells_increase;
	LIST *left_cell_list;
	LIST *right_cell_list;
} TRANSECT;

typedef struct
{
	char *initial_takeoff_date;
	LIST *transect_list;
	char *preferred_hydropattern_seat;
} SRF_CENSUS;

/* Prototypes */
/* ---------- */
char *wadingbird_get_initial_takeoff_date_international(
				char *initial_takeoff_date_american );

void wadingbird_get_species(	char **common_name,
				char *application_name,
				int species_code );

boolean wadingbird_get_hydropattern(
				char **hydropattern,
				char *hydropattern_numeric_code,
				char *application_name );

DICTIONARY *wadingbird_get_hydropattern_code_dictionary(
				char *application_name );

SRF_CENSUS *wadingbird_srf_census_new(
				char *application_name,
				char *input_filename );

TRANSECT *wadingbird_transect_new(
				char *application_name,
				int transect_number,
				char *input_filename );

SRF_CELL *wadingbird_srf_cell_new(
				int cell_number );

WADINGBIRD_CELL *wadingbird_cell_new(
				int cell_number );

LIST *wadingbird_get_transect_list(
				char *application_name,
				char *input_filename );

LIST *wadingbird_get_cell_list( char *application_name );

LIST *wadingbird_given_transect_get_cell_list(
				char *application_name,
				int transect_number,
				boolean cells_increase );

boolean wadingbird_exists_researcher(
				char *input_filename,
				char *left_right_code );

FILE *wadingbird_get_tablet_input_pipe(
				char *input_filename );

char *wadingbird_get_tablet_input_sys_string(
				char *input_filename );

char *wadingbird_get_tablet_initial_takeoff_date_international(
				char *input_filename );

SRF_CELL *wadingbird_get_cell(	LIST *transect_list,
				int cell_number,
				char *left_right );

HYDROPATTERN_MERGE *wadingbird_get_hydropattern_merge(
				char *application_name,
				char *initial_takeoff_date );

HYDROPATTERN_MERGE *wadingbird_hydropattern_merge_new(
				void );

LIST *wadingbird_get_hydropattern_merge_cell_list(
				char *application_name,
				char *initial_takeoff_date,
				char *preferred_hydropattern_seat );

SRF_CELL *wadingbird_merge_seek_cell(
				LIST *cell_list,
				int cell_number );

int wadingbird_get_bird_estimation(
				char *application_name,
				int cell_number,
				int bird_count,
				LIST *transects_with_one_observer_list );

int wadingbird_given_cell_number_get_transect_number(
				char *application_name,
				int cell_number );

boolean wadingbird_exists_cell_hydropattern_for_seat(
				char *application_name,
				char *initial_takeoff_date,
				char *preferred_seat_position );

int wadingbird_get_max_bird_count(
				LIST *cell_list );
#endif
