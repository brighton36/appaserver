/* load_spreadsheets.h */
/* ------------------- */

#ifndef LOAD_SPREADSHEET_H
#define LOAD_SPREADSHEET_H

/* Constants */
/* ========= */

/* SRF Spreadsheet */
/* --------------- */
#define SRF_NEST_NUMBER				1
#define SRF_DISCOVERY_DATE			3
#define SRF_TRANSECT_NUMBER			4
#define SRF_IN_TRANSECT_BOUNDARY_YN		5
#define SRF_BASIN_CODE				6
#define SRF_HABITAT_CODE			7
#define SRF_NEAR_POND_YN			8
#define SRF_LOCATION_NOTEPAD			9
#define SRF_UTM_EASTING				10
#define SRF_UTM_NORTHING			11
#define SRF_PRIMARY_RESEARCHER			12
#define SRF_SECONDARY_RESEARCHER1		13
#define SRF_SECONDARY_RESEARCHER2		14
#define SRF_PILOT				15
#define SRF_NEST_NOTEPAD			16
#define SRF_YN					17

/* Observation Spreadsheet */
/* ----------------------- */
#define OBSERVATION_DISCOVERY_YEAR		1
#define OBSERVATION_DATE			2
#define OBSERVATION_VIEW			3
#define OBSERVATION_NEST_NUMBER			5
#define OBSERVATION_MONITOR_NEST_YN		6
#define OBSERVATION_NEST_VISIT_NUMBER		7
#define OBSERVATION_TOTAL_EGGS			8
#define OBSERVATION_BANDED_EGGS			9
#define OBSERVATION_NOT_BANDED_EGGS		10
#define OBSERVATION_HATCHED_EGGS		11
#define OBSERVATION_FLOODED_EGGS		12
#define OBSERVATION_PREDATED_EGGS		13
#define OBSERVATION_CRUSHED_EGGS		14
#define OBSERVATION_OTHER_EGG_MORTALITY		15
#define OBSERVATION_REMOVED_VIABLE_EGGS		16
#define OBSERVATION_REMOVED_NON_VIABLE_EGGS	17
#define OBSERVATION_FEMALE_SIZE_CM		18
#define OBSERVATION_FEMALE_BEHAVIOR_CODE	19
#define OBSERVATION_NEST_HABITAT_CODE		20
#define OBSERVATION_MATERIALS_CODE		21
#define OBSERVATION_NEST_STATUS_CODE		22
#define OBSERVATION_HATCHLINGS_COUNT		23
#define OBSERVATION_WATER_DEPTH_1		24
#define OBSERVATION_WATER_DEPTH_2		25
#define OBSERVATION_WATER_DEPTH_3		26
#define OBSERVATION_WATER_DEPTH_4		27
#define OBSERVATION_WATER_DEPTH_AVERAGE_CM	28
#define OBSERVATION_NEST_HEIGHT_CM		29
#define OBSERVATION_NEST_HEIGHT_ABOVE_WATER_CM	30
#define OBSERVATION_NOTEPAD			31

#define UNKNOWN_RESEARCHER		"unknown"
#define NEST_PRIMARY_KEY		"nest_number"

#define INSERT_NEST			"nest_number,utm_easting,utm_northing,discovery_date,primary_researcher,transect_number,in_transect_boundary_yn,basin,habitat,near_pond_yn,nest_notepad,SRF_yn"
#define INSERT_TRANSECT			"transect_number"
#define INSERT_ALLIGATOR_CENSUS		"discovery_date,primary_researcher,pilot"
#define INSERT_PILOT			"pilot"
#define INSERT_SECONDARY_RESEARCHER	"researcher,discovery_date,primary_researcher"
#define INSERT_RESEARCHERS		"researcher"
#define INSERT_WATER_DEPTH		"nest_number,observation_date,measurement_number,water_depth_cm"

#define INSERT_NEST_OBSERVATION		"nest_number,observation_date,observation_view,nest_status,female_behavior,observation_notepad,nest_visit_number,hatchlings_count,hatched_eggs,flooded_eggs,predated_eggs,crushed_eggs,other_egg_mortality,removed_viable_eggs,removed_non_viable_eggs,water_depth_average_cm,nest_height_above_water_cm"

/* Prototypes */
/* ---------- */
void delete_nest(		char *application_name,
				char *input_filename );

int insert_nests(
				char *application_name,
				char *input_filename,
				char *output_medium );
int insert_observations(
				char *application_name,
				char *input_filename,
				char really_yn );


void delete_observation(	char *application_name,
				char *input_filename );

#endif
