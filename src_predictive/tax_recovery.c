/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/tax_recovery.c			*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "appaserver_library.h"
#include "ledger.h"
#include "tax_recovery.h"

boolean tax_recovery_parse(
				int *service_month,
				int *service_year,
				int *sale_month,
				int *sale_year,
				char *service_placement_date_string,
				char *sale_date_string )
{
	DATE *service_placement_date;
	DATE *sale_date = {0};

	*service_month = 0;
	*service_year = 0;
	*sale_month = 0;
	*sale_year = 0;

	if ( ! ( service_placement_date =
			date_yyyy_mm_dd_new(
				service_placement_date_string ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: invalid service_placement_date = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 service_placement_date_string );

		return 0;
	}

	*service_month = date_get_month( service_placement_date );
	*service_year = date_get_year( service_placement_date );
	date_free( service_placement_date );

	if ( sale_date_string && *sale_date_string )
	{
		if ( ! ( sale_date =
				date_yyyy_mm_dd_new(
					sale_date_string ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: invalid sale_date = (%s)\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	sale_date_string );

			return 0;
		}

		*sale_month = date_get_month( sale_date );
		*sale_year = date_get_year( sale_date );
		date_free( sale_date );
	}

	return 1;

} /* tax_recovery_parse() */

double tax_recovery_calculate_recovery_amount(
				double *recovery_percent,
				double cost_basis,
				char *service_placement_date_string,
				char *sale_date_string,
				double recovery_period_years,
				int current_year )
{
	double recovery_amount = 0.0;
	int service_month = 0;
	int service_year = 0;
	int sale_month = 0;
	int sale_year = 0;
	int recovery_period_months;
	int recovery_period_semi_months;
	double percent_per_year;
	double percent_per_semi_month;
	unsigned int recovery_months_as_of_december;
	unsigned int recovery_months_extra_year;
	double applicable_rate = 0.0;

	if ( !tax_recovery_parse(
				&service_month,
				&service_year,
				&sale_month,
				&sale_year,
				service_placement_date_string,
				sale_date_string ) )
	{
		return 0.0;
	}

	recovery_period_months = (int)(recovery_period_years * 12.0);
	recovery_period_semi_months = recovery_period_months * 2;
	percent_per_year = 1.0 / recovery_period_years;
	percent_per_semi_month = 1.0 / (double)recovery_period_semi_months;

	recovery_months_as_of_december =
		( current_year * 12 - service_year * 12 ) +
		( ( 12 - service_month ) + 1 );

	recovery_months_extra_year =
		recovery_period_months + 12;

	if ( sale_year == current_year
	&&   recovery_months_as_of_december <= recovery_period_months )
	{
		applicable_rate =
			(double)( ( ( sale_month - 1 ) * 2 ) + 1 ) *
			percent_per_semi_month;
	}
	else
	if ( sale_year )
	{
		return 0.0;
	}
	else
	if ( recovery_months_as_of_december <= 12 )
	{
		applicable_rate =
			(double)( ( ( 12 - service_month ) * 2 ) + 1 ) *
			percent_per_semi_month;
	}
	else
	if ( recovery_months_as_of_december > 12
	&&   recovery_months_as_of_december <= recovery_period_months )
	{
		applicable_rate = percent_per_year;
	}
	else
	if ( recovery_months_as_of_december > recovery_period_months
	&&   recovery_months_as_of_december <= recovery_months_extra_year )
	{
		if ( timlib_double_is_integer( recovery_period_years ) )
		{
			applicable_rate =
				(double)
				( ( service_month * 2 ) - 1 ) *
				percent_per_semi_month;
		}
		else
		/* ------------------ */
		/* If extra half-year */
		/* ------------------ */
		if ( service_month <= 6 )
		{
			applicable_rate =
			(double)( 12 + ( service_month * 2 ) - 1 ) *
			percent_per_semi_month;
		}
		else
		{
			applicable_rate =
			(double)( ( service_month - 6 ) * 2 - 1 ) *
			percent_per_semi_month;
		}
	}

	recovery_amount = cost_basis * applicable_rate;

	if ( recovery_percent )
	{
		*recovery_percent = applicable_rate * 100.0;
	}

	return recovery_amount;

} /* tax_recovery_calculate_recovery_amount() */

char *tax_recovery_get_order_by( void )
{
	return "asset_name,serial_number";
}

char *tax_recovery_get_select( void )
{
	char *select;

	select =
"asset_name,serial_number,tax_service_placement_date,tax_cost_basis,tax_recovery_period,disposal_date";

	return select;
}

char *tax_recovery_get_filter_where( char *minimum_disposal_date )
{
	char filter_where[ 512 ];

	sprintf( filter_where,
		 "(disposal_date is null or disposal_date >= %s) and	"
		 "ifnull(tax_cost_basis,0.0) <> 0.0) and		"
		 "tax_recovery_period is not null and			"
		 "tax_accumulated_depreciation < tax_cost_basis		",
		 minimum_disposal_date );

	return strdup( filter_where );

} /* tax_recovery_get_filter_where() */

FILE *tax_recovery_get_fixed_asset_input_pipe(
					char *application_name,
					int recovery_year )
{
	char *select;
	char *order_by;
	char *filter_where;
	char sys_string[ 1024 ];
	char minimum_disposal_date[ 16 ];

	select = tax_recovery_get_select();

	order_by = tax_recovery_get_order_by();

	sprintf( minimum_disposal_date, "%d-01-01", recovery_year );

	filter_where =
		tax_recovery_get_filter_where(
			minimum_disposal_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=\"%s\"			",
		 application_name,
		 select,
		 "fixed_asset_purchase",
		 filter_where,
		 order_by );

	return popen( sys_string, "r" );

} /* tax_recovery_get_fixed_asset_input_pipe() */

int tax_recovery_fetch_max_recovery_year(
			char *application_name,
			char *folder_name )
{
	char sys_string[ 1024 ];
	int max_recovery_year;

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			",
		 application_name,
		 "max(recovery_year)",
		 folder_name );

	max_recovery_year = atoi( pipe2string( sys_string ) );

	return max_recovery_year;

} /* tax_recovery_fetch_max_recovery_year() */

TAX_RECOVERY *tax_recovery_new(	void )
{
	TAX_RECOVERY *t;

	if ( ! ( t = calloc( 1, sizeof( TAX_RECOVERY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return t;

} /* tax_recovery_new() */

TAX_RECOVERY *tax_recovery_input_buffer_parse(
				char *input_buffer )
{
	TAX_RECOVERY *t;
	char piece_buffer[ 512 ];

	t = tax_recovery_new();

/*
"asset_name,serial_number,tax_service_placement_date,tax_cost_basis,tax_recovery_period,disposal_date";
*/
	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	t->asset_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	t->serial_number = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	t->tax_service_placement_date = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	t->tax_cost_basis = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	t->tax_recovery_period = strdup( piece_buffer );

	t->tax_recovery_period_years = atof( t->tax_recovery_period );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	t->disposal_date = strdup( piece_buffer );

	return t;

} /* tax_recovery_input_buffer_parse() */

