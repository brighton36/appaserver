/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_camp/camp.c					*/
/* -------------------------------------------------------------------- */
/* This is the PredictiveBooks CAMP ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include "camp.h"

TRANSACTION *camp_enrollment_transaction(
				char *application_name,
				char *fund_name,
				double enrollment_cost )
{
	return (TRANSACTION *)0;

} /* camp_enrollment_transaction() */

ENROLLMENT *camp_enrollment_fetch(
				char *application_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address )
{
	return (ENROLLMENT *)0;

} /* camp_enrollment_fetch() */

ENROLLMENT *camp_enrollment_parse(
				char *input_buffer )
{
	return (ENROLLMENT *)0;

} /* camp_enrollment_parse() */

char *camp_enrollment_select( void )
{
	return (char *)0;
}

LIST *camp_enrollment_journal_ledger_list(
				double *transaction_amount,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double enrollment_cost )
{
	return (LIST *)0;

} /* camp_enrollment_journal_ledger_list() */

