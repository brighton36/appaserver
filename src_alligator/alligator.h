/* ---------------------------------------------------	*/
/* alligator.h						*/
/* ---------------------------------------------------	*/
/* This is the library for the alligator project.	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef ALLIGATOR_H
#define ALLIGATOR_H

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */

typedef struct
{
	LIST *alligator_census_list;
	LIST *nest_list;
} ALLIGATOR;

typedef struct
{
	char *discovery_date;
	char *discovery_year;
	char *discovery_date_american;
	char *primary_researcher;
	char *pilot;
	LIST *secondary_researcher_list;
} ALLIGATOR_CENSUS;

typedef struct
{
	ALLIGATOR_CENSUS *alligator_census;
	int nest_number;
	char *nest_number_key;
	char utm_easting[ 16 ];
	char utm_northing[ 16 ];
	char transect_number[ 16 ];
	char monitor_nest_yn;
	char total_eggs_string[ 16 ];
	char banded_eggs_string[ 16 ];
	char not_banded_eggs_string[ 16 ];
	char female_size_cm_string[ 16 ];
	char *materials;
	char *materials_code;
	char *basin;
	char *basin_code;
	char *habitat;
	char *habitat_code;
	char nest_height_cm_string[ 16 ];
	char in_transect_boundary_yn;
	char near_pond_yn;
	char *nest_notepad;
	LIST *observation_list;
} NEST;

typedef struct
{
	int water_depth_cm;
} WATER_DEPTH;

typedef struct
{
	char *observation_date;
	char nest_visit_number[ 16 ];
	char *observation_view;
	char *nest_status;
	char *female_behavior;
	char hatchlings_count_string[ 16 ];
	char hatched_eggs_string[ 16 ];
	char flooded_eggs_string[ 16 ];
	char predated_eggs_string[ 16 ];
	char crushed_eggs_string[ 16 ];
	char other_egg_mortality_string[ 16 ];
	char removed_viable_eggs_string[ 16 ];
	char removed_non_viable_eggs_string[ 16 ];
	char water_depth_average_cm_string[ 16 ];
	char nest_height_above_water_cm_string[ 16 ];
	char observation_notepad[ 1024 ];
	LIST *water_depth_list;
} OBSERVATION;

typedef struct
{
	double transect_number;
	double utm_northing;
	int east_easting;
	int east_northing;
	int west_easting;
	int west_northing;
} TRANSECT;

typedef struct
{
	int cell_number;
	double utm_northing;
	double utm_easting;
	char *basin;
} CELL;

/* Prototypes */
/* ---------- */
CELL *alligator_cell_new(	int cell_number,
				double utm_northing,
				double utm_easting,
				char *basin );

char *alligator_get_basin_should_be(	int *cell_number,
					char *application_name,
					double utm_northing,
					double utm_easting );

TRANSECT *alligator_seek_transect(	LIST *transect_list,
					int transect_number );

TRANSECT *alligator_transect_new(	double transect_number );

LIST *alligator_get_transect_list(	char *application_name );

LIST *alligator_get_cell_list(		char *application_name );

int alligator_get_transect_number_should_be(
				int *parameter_distance,
				char *application_name,
				int nest_utm_northing );

char alligator_get_in_transect_boundary_should_be_yn(
				double *distance,
				char *application_name,
				int transect_number,
				int nest_utm_northing,
				int nest_utm_easting );

char *alligator_get_discovery_date_american(
				char *discovery_date_international );

char *alligator_get_discovery_year(
				char *discovery_date_international );

char *alligator_get_materials_code(	char *application_name,
					char *materials_name );

ALLIGATOR_CENSUS *alligator_census_fetch(
				char *discovery_date,
				char *primary_reseacher,
				LIST *alligator_census_list );

LIST *alligator_get_secondary_researcher_list(
				char *application_name,
				char *discovery_date,
				char *primary_researcher );

LIST *alligator_get_census_list(char *application_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				char *begin_discovery_date,
				char *end_discovery_date,
				boolean with_secondary_researchers );

char *alligator_get_discovery_primary_where_clause(
				char *application_name,
				char *folder_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				char *begin_discovery_date,
				char *end_discovery_date );

ALLIGATOR *alligator_new(	char *application_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				char *begin_discovery_date,
				char *end_discovery_date,
				boolean with_secondary_researchers,
				boolean with_nest_list,
				boolean with_observation_list );

void alligator_get_secondary_researchers(
				char **secondary_researcher_1,
				char **secondary_researcher_2,
				LIST *secondary_researcher_list );

char *alligator_get_pilot(
				char *application_name,
				char *discovery_date,
				char *primary_researcher );

LIST *alligator_get_alligator_census_record_list(
				char *application_name );

LIST *alligator_get_nest_list(	char *application_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				boolean with_observation_list,
				LIST *alligator_census_list );

char *alligator_get_basin(
				char *application_name,
				char *basin_code );

char *alligator_get_basin_code(	char *application_name,
				char *basin_name );

char *alligator_get_habitat_code(char *application_name,
				 char *habitat_name );

char *alligator_get_habitat(
				char *application_name,
				char *habitat_code );

char *alligator_get_female_behavior(
				char *application_name,
				char *female_behavior_code );

char *alligator_get_female_behavior_code(
				char *application_name,
				char *female_behavior );

char *alligator_get_nest_status(
				char *application_name,
				char *nest_status_code );

char *alligator_get_nest_status_code(
				char *application_name,
				char *nest_status );

NEST *alligator_nest_new(	char *nest_number_key );

ALLIGATOR_CENSUS *alligator_census_new(
				char *application_name,
				char *discovery_date,
				char *primary_researcher,
				char *pilot,
				boolean with_secondary_researchers );

OBSERVATION *alligator_observation_new(
				char *observation_date );

char *alligator_get_nest_key(	char *discovery_date_international,
				char *discovery_year,
				char *nest_number );

char *alligator_get_materials(
				char *application_name,
				char *materials_code );

LIST *alligator_get_observation_list(
				char *application_name,
				char *nest_number,
				char *discovery_date_list_string,
				char *primary_researcher_list_string );

LIST *alligator_get_observation_record_list(
				char *application_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string );

char *alligator_get_transect_number_string(
				char *application_name,
				int utm_northing );

double alligator_get_transect_northing_at_nest_easting(
				TRANSECT *transect,
				int nest_utm_easting );

WATER_DEPTH *alligator_water_depth_new(
				int water_depth_cm );

LIST *alligator_get_water_depth_record_list(
				char *application_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string );

LIST *alligator_get_water_depth_list(
				char *application_name,
				char *nest_number,
				char *observation_date,
				char *discovery_date_list_string,
				char *primary_researcher_list_string );

#endif
