/* -------------------------------------------------------------------- */
/* src_accountancymodel/rental.h					*/
/* -------------------------------------------------------------------- */
/* This is the appaserver rental ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef RENTAL_H
#define RENTAL_H

#include "boolean.h"
#include "customer.h"
#include "purchase.h"
#include "hash_table.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */

/* Operations */
/* ---------- */
LIST *rental_get_rental_property_string_list(	char *application_name,
						int tax_year );

#endif

