/* ---------------------------------------------------	*/
/* src_benthic/benthic_species.h			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef BENTHIC_SPECIES_H
#define BENTHIC_SPECIES_H

#include "list.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define BENTHIC_SPECIES_SCIENTIFIC_NAME_TEMPLATE	\
	"concat( %s.family_name, ' ', %s.genus_name, ' ', %s.species_name )"

/* Structures */
/* ---------- */
typedef struct
{
	char *scientific_name;
	char *common_name;
	char *faunal_group;
} BENTHIC_SPECIES;

/* Prototypes */
/* ---------- */
BENTHIC_SPECIES *benthic_species_new(
			char *scientific_name );

LIST *benthic_species_with_faunal_group_get_species_list(
			char *application_name,
			char *faunal_group );

LIST *benthic_species_get_record_list(
			char *application_name );

LIST *benthic_species_get_vegetation_species_name_list(
			char *application_name,
			boolean total );

LIST *benthic_species_get_species_name_list(
			char *application_name,
			char *faunal_group );

LIST *benthic_species_get_common_name_list(
			char *application_name,
			char *faunal_group );

char *benthic_species_get_scientific_name_expression(
			char *table_name );

#endif
