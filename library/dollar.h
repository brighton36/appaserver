/* $APPASERVER_HOME/library/dollar.h			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef DOLLAR_H
#define DOLLAR_H

#include "boolean.h"

/* Prototypes */
/* ---------- */
void dollar_part(	char *destination,
			int integer_part,
			boolean need_comma,
			boolean omit_dollars );

void pennies_part(	char *destination,
			int decimal_part );

char *dollar_text(
			char *destination,
			double dollar );

#endif

