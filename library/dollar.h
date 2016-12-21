/* $APPASERVER_HOME/library/dollar.h			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */
/* This package converts a dollar double to dollar text.   */
/* ------------------------------------------------------- */

#ifndef DOLLAR_H
#define DOLLAR_H

#include "boolean.h"

/* Prototypes */
/* ---------- */
char *dollar_text(	char *destination,
			double dollar );

void dollar_part(	char *destination,
			int integer_part,
			boolean need_comma,
			boolean omit_dollars );

void pennies_part(	char *destination,
			int decimal_part );

#endif

