/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_creel/creel.h			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef CREEL_H
#define CREEL_H

/* Includes */
/* -------- */
#include "list.h"
#include "hash_table.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *family;
	char *genus;
	char *species;
	int kept_count;
	int released_count;
} CREEL_CATCHES;

typedef struct
{
	int interview_number;
	char *interview_time;
	char *trip_origin_location;
	char *family;
	char *genus;
	char *species_preferred;
	char *fishing_area;
	char *fishing_party_composition;
	int trip_hours;
	int hours_fishing;
	int number_of_people_fishing;
	char *permit_code;
	char *recreational_angler_reside;
	LIST *catches_list;
} CREEL_FISHING_TRIPS;

typedef struct
{
	char *fishing_purpose;
	char *census_date;
	char *interview_location;
	char *researcher;
	LIST *fishing_trips_list;
} CREEL_CENSUS;

typedef struct
{
	LIST *creel_census_list;
	HASH_TABLE *creel_census_hash_table;
	HASH_TABLE *fishing_trips_hash_table;
	HASH_TABLE *catches_hash_table;
} CREEL;

/* Prototypes */
/* ---------- */
CREEL *creel_new(			char *application_name,
					char *begin_census_date,
					char *end_census_date,
					char *fishing_purpose );

HASH_TABLE *creel_get_census_hash_table(LIST **creel_census_list,
					char *application_name,
					char *begin_census_date,
					char *end_census_date,
					char *fishing_purpose );

HASH_TABLE *creel_get_fishing_trips_hash_table(
					char *application_name,
					char *begin_census_date,
					char *end_census_date,
					char *fishing_purpose );

HASH_TABLE *creel_get_catches_hash_table(
					char *application_name,
					char *begin_census_date,
					char *end_census_date,
					char *fishing_purpose );

CREEL_CENSUS *creel_census_new(		char *fishing_purpose,
					char *census_date,
					char *interview_location );

CREEL_FISHING_TRIPS *creel_fishing_trips_new(
					int interview_number );

CREEL_CATCHES *creel_catches_new(	char *family,
					char *genus,
					char *species );

char *creel_census_get_key(		char *fishing_purpose,
					char *census_date,
					char *interview_location );

char *creel_fishing_trips_get_key(	char *fishing_purpose,
					char *census_date,
					char *interview_location,
					int interview_number );

char *creel_catches_get_key(		char *fishing_purpose,
					char *census_date,
					char *interview_location,
					int interview_number,
					char *family,
					char *genus,
					char *species );

CREEL_CENSUS *creel_census_fetch(	HASH_TABLE *creel_census_hash_table,
					char *fishing_purpose,
					char *census_date,
					char *interview_location );

CREEL_FISHING_TRIPS *creel_fishing_trips_fetch(
					HASH_TABLE *fishing_trips_hash_table,
					char *fishing_purpose,
					char *census_date,
					char *interview_location,
					int interview_number );

CREEL_CATCHES *creel_catches_fetch(	HASH_TABLE *catches_hash_table,
					char *fishing_purpose,
					char *census_date,
					char *interview_location,
					int interview_number,
					char *family,
					char *genus,
					char *species );

void creel_append_fishing_trips(	HASH_TABLE *creel_census_hash_table,
					HASH_TABLE *fishing_trips_hash_table );

void creel_append_catches(		HASH_TABLE *fishing_trips_hash_table,
					HASH_TABLE *catches_hash_table );

#endif
