/* -------------------------------------------------------------------- */
/* src_predictive/rental.c						*/
/* -------------------------------------------------------------------- */
/* This is the PredictiveBooks rental ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"
#include "rental.h"

LIST *rental_get_rental_property_string_list(	char *application_name,
						int tax_year )
{
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *select;

	select = "rental_property_street_address";

	sprintf( where,
		 "( property_acquired_year is null		"
		 "  or property_acquired_year >= %d ) and	"
		 "( property_disposal_year is null		"
		 "  or property_disposal_year <= %d )		",
		 tax_year,
		 tax_year );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=rental_property		"
		 "			where=\"%s\"			"
		 "			order=property_acquired_year	",
		 application_name,
		 select,
		 where );

	return pipe2list( sys_string );

} /* rental_get_rental_property_string_list() */

