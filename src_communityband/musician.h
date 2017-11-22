/* src_capitolpops/musician.h						*/
/* -------------------------------------------------------------------- */
/* This is the appaserver musician ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef MUSICIAN_H
#define MUSICIAN_H

#include "list.h"

#define APPLICATION	"capitolpops"

typedef struct
{
	char *full_name;
	char *primary_instrument;
	char *email_address;
	char *street_address;
	char *city;
	char *state_code;
	char *zip_code;
	char *home_phone;
	char *cell_phone;
	char *work_phone;
	char *substitute_concert_only_yn;
	char *contact_information_private_yn; 
} MUSICIAN;

MUSICIAN *musician_new( 
				char *full_name );

LIST *musician_get_band_member_list(
				char **email_address,
				char *member_full_name );

LIST *musician_get_substitute_musician_list(
				char **email_address,
				char *member_full_name );

MUSICIAN *musician_fetch(	char *musician_full_name );

LIST *musician_get_substitution_substitute_musician_list(
				char *member_primary_instrument );

LIST *musician_get_instrument_band_member_email_address_list(
				char *primary_instrument );

LIST *musician_get_list(	void );

#endif
