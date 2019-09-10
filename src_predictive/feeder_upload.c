/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_upload.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include "timlib.h"
#include "sed.h"
#include "piece.h"
#include "feeder_upload.h"

LIST *feeder_upload_get_possible_description_list(
				char *bank_description_file,
				char *fund_name,
				char *bank_date,
				double bank_amount,
				double bank_running_balance,
				int check_number )
{
	LIST *possible_description_list = list_new();
	char *trimmed_bank_description;

	list_append_pointer(
		possible_description_list,
		/* ----------------------- */
		/* Returns strdup() memory */
		/* ----------------------- */
		feeder_upload_get_description_embedded(
			bank_description_file,
			fund_name,
			bank_amount,
			bank_running_balance ) );

	list_append_pointer(
		possible_description_list,
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			feeder_upload_get_description_bank_amount(
				bank_description_file,
				bank_amount ) ) );

	if ( check_number )
	{
		list_append_pointer(
			possible_description_list,
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				feeder_upload_get_check_portion(
					check_number ) ) );
	}

#ifdef NOT_DEFINED
Trimming MM/DD from the input file.

	/* If they append the MM/DD to the row. */
	/* ------------------------------------ */
	if ( ( trimmed_bank_description =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			feeder_upload_trim_bank_date_description(
				bank_description_file ) ) )
	{
		list_append_pointer(
			possible_description_list,
			strdup( trimmed_bank_description ) );
	}
#endif

	return possible_description_list;

} /* feeder_upload_get_possible_description_list() */

/* Returns static memory */
/* --------------------- */
char *feeder_upload_get_description_bank_amount(
			char *bank_description_file,
			double bank_amount )
{
	static char bank_description_bank_amount[ 1024 ];
	char *bank_amount_portion;

	bank_amount_portion =
		/* Returns static memory */
		/* --------------------- */
		feeder_upload_get_bank_amount_portion(
			bank_amount );

	sprintf( bank_description_bank_amount,
		 "%s%s",
		 bank_description_file,
		 bank_amount_portion );

	return bank_description_bank_amount;

} /* feeder_upload_get_description_bank_amount() */

/* Returns strdup() memory */
/* ----------------------- */
char *feeder_upload_get_description_embedded(
			char *bank_description_file,
			char *fund_name,
			double bank_amount,
			double bank_running_balance )
{
	char *bank_amount_portion;
	char bank_description_embedded[ 1024 ];
	char fund_portion[ 512 ];
	char running_balance_portion[ 512 ];

	*fund_portion = '\0';

	/* Returns static memory */
	/* --------------------- */
	bank_description_file =
		feeder_upload_trim_bank_date_description(
			bank_description_file );

	if ( fund_name && *fund_name && strcmp( fund_name, "fund" ) != 0 )
	{
		sprintf( fund_portion, " %s", fund_name );
	}

	sprintf( running_balance_portion, " %.2lf", bank_running_balance );

	bank_amount_portion =
		/* Returns static memory */
		/* --------------------- */
		feeder_upload_get_bank_amount_portion(
			bank_amount );

	sprintf( bank_description_embedded,
		 "%s%s%s%s",
		 bank_description_file,
	 	 fund_portion,
		 bank_amount_portion,
		 running_balance_portion );

	return strdup( bank_description_embedded );

} /* feeder_upload_get_description_embedded() */

/* Returns static memory */
/* --------------------- */
char *feeder_upload_get_bank_amount_portion(
				double bank_amount )
{
	static char bank_amount_portion[ 128 ];

	if ( bank_amount - (double)(int)bank_amount == 0.0 )
	{
		sprintf( bank_amount_portion, " %d", (int)bank_amount );
	}
	else
	{
		sprintf( bank_amount_portion, " %.2lf", bank_amount );

		/* They trim off the pennies if on the dime. */
		/* ----------------------------------------- */
		if ( * (bank_amount_portion +
			strlen( bank_amount_portion ) -
			1 ) == '0' )
		{
			*(bank_amount_portion +
			  strlen( bank_amount_portion ) -
			  1) = '\0';
		}
	}

	return bank_amount_portion;

} /* feeder_upload_get_bank_amount_portion() */

/* Returns static memory */
/* --------------------- */
char *feeder_upload_get_fund_portion(
				char *fund_name )
{
	static char fund_portion[ 128 ];

	*fund_portion = '\0';

	if ( fund_name && *fund_name && strcmp( fund_name, "fund" ) != 0 )
	{
		sprintf( fund_portion, " %s", fund_name );
	}

	return fund_portion;

} /* feeder_upload_get_fund_portion() */

/* Returns static memory */
/* --------------------- */
char *feeder_upload_get_check_portion(
				int check_number )
{
	static char check_portion[ 128 ];

	*check_portion = '\0';

	if ( check_number )
	{
		sprintf( check_portion, "CHECK %d", check_number );
	}

	return check_portion;

} /* feeder_upload_get_check_portion() */

char *feeder_upload_get_like_where(	char *where,
					char *bank_date,
					char *bank_description )
{
	char description_buffer[ 512 ];

	sprintf( where,
		 "bank_date = '%s' and			"
		 "bank_description like '%s%c'		",
		 bank_date,
		 escape_character(	description_buffer,
					bank_description,
					'\'' ),
		 '%' );

	return where;

} /* feeder_upload_get_like_where() */

/* Returns static memory */
/* --------------------- */
char *feeder_upload_trim_bank_date_description(
				char *bank_description_file )
{
	static char sans_bank_date_description[ 512 ];
	char *replace;
	char *regular_expression;
	char buffer[ 512 ];
	SED *sed;

	regular_expression = "[ ][0-9][0-9]/[0-9][0-9]$";
	replace = "";

	sed = sed_new( regular_expression, replace );

	timlib_strcpy(	sans_bank_date_description,
			bank_description_file,
			512 );

	/* Why do I need to append the EOL character for sed() to work? */
	/* ------------------------------------------------------------ */
	sprintf( sans_bank_date_description +
		 strlen( sans_bank_date_description ),
		 "$" );

	if ( sed_will_replace( sans_bank_date_description, sed ) )
	{
		sed_search_replace( sans_bank_date_description, sed );
	}
	else
	{
		trim_right( sans_bank_date_description, 1 );
	}

	sed_free( sed );

	return timlib_rtrim( sans_bank_date_description );

} /* feeder_upload_trim_bank_date_description() */

