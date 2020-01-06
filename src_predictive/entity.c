/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/entity.c				*/
/* -------------------------------------------------------------------- */
/* This is the PredictiveBooks entity ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"
#include "purchase.h"
#include "customer.h"
#include "ledger.h"
#include "entity.h"

enum payroll_pay_period entity_get_payroll_pay_period(
				char *payroll_pay_period_string )
{
	if ( strcasecmp( payroll_pay_period_string, "weekly" ) == 0 )
		return pay_period_weekly;
	else
	if ( strcasecmp( payroll_pay_period_string, "biweekly" ) == 0 )
		return pay_period_biweekly;
	else
	if ( strcasecmp( payroll_pay_period_string, "semimonthly" ) == 0 )
		return pay_period_semimonthly;
	else
	if ( strcasecmp( payroll_pay_period_string, "monthly" ) == 0 )
		return pay_period_monthly;
	else
		return pay_period_not_set;

} /* entity_get_payroll_pay_period() */

enum inventory_cost_method entity_get_inventory_cost_method(
				char *inventory_cost_method_string )
{
	if ( strcasecmp( inventory_cost_method_string, "fifo" ) == 0 )
		return inventory_fifo;
	else
	if ( strcasecmp( inventory_cost_method_string, "lifo" ) == 0 )
		return inventory_lifo;
	else
	if ( strcasecmp( inventory_cost_method_string, "average" ) == 0 )
		return inventory_average;
	else
	if ( strcasecmp(	inventory_cost_method_string,
				"moving_average" ) == 0 )
		return inventory_average;
	else
		return inventory_not_set;

} /* entity_get_inventory_cost_method() */

ENTITY *entity_calloc( void )
{
	ENTITY *e;

	if ( ! ( e = calloc( 1, sizeof( ENTITY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return e;

} /* entity_calloc() */

ENTITY *entity_new(	char *full_name,
			char *street_address )
{
	ENTITY *e;

	e = entity_calloc();

	e->full_name = full_name;
	e->street_address = street_address;
	return e;

} /* entity_new() */

ENTITY_SELF *entity_self_new(	char *full_name,
				char *street_address )
{
	ENTITY_SELF *s;

	if ( ! ( s = calloc( 1, sizeof( ENTITY_SELF ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	s->entity = entity_new( full_name, street_address );

	return s;

} /* entity_self_new() */

char *entity_self_get_select( char *application_name )
{
	char *select;
	boolean inventory_cost_method_exists;

	inventory_cost_method_exists =
		attribute_exists(
			application_name,
			"self"
				/* folder_name */,
			"inventory_cost_method"
				/* attribute_name */ );

	if ( inventory_cost_method_exists )
	{
		select =
"full_name, street_address, inventory_cost_method, payroll_pay_period, payroll_beginning_day, social_security_combined_tax_rate, social_security_payroll_ceiling, medicare_combined_tax_rate, medicare_additional_withholding_rate, medicare_additional_gross_pay_floor, federal_unemployment_wage_base, federal_unemployment_tax_minimum_rate, federal_unemployment_tax_standard_rate, federal_withholding_allowance_period_value, federal_nonresident_withholding_income_premium, state_unemployment_wage_base, state_unemployment_tax_rate, federal_unemployment_threshold_rate, state_withholding_allowance_period_value, state_itemized_allowance_period_value, state_sales_tax_rate";
	}
	else
	{
		select =
"full_name, street_address, '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''";
	}

	return select;

} /* entity_self_get_select() */

ENTITY_SELF *entity_self_load(	char *application_name )
{
	ENTITY_SELF *self;
	char full_name[ 128 ];
	char street_address[ 128 ];
	char piece_buffer[ 128 ];
	char sys_string[ 1024 ];
	char *select;
	char *results;

	select = entity_self_get_select( application_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=self		",
		 application_name,
		 select );

	results = pipe2string( sys_string );

	if ( !results ) return (ENTITY_SELF *)0;

	piece( full_name, FOLDER_DATA_DELIMITER, results, 0 );
	piece( street_address, FOLDER_DATA_DELIMITER, results, 1 );

	self = entity_self_new(
			strdup( full_name ),
			strdup( street_address ) );

	if ( !entity_load(	&self->entity->city,
				&self->entity->state_code,
				&self->entity->zip_code,
				&self->entity->phone_number,
				&self->entity->email_address,
				application_name,
				self->entity->full_name,
				self->entity->street_address ) )
	{
		return (ENTITY_SELF *)0;
	}

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );
	self->inventory_cost_method =
		entity_get_inventory_cost_method(
			piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 3 );
	self->payroll_pay_period =
		entity_get_payroll_pay_period(
			piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 4 );
	self->payroll_beginning_day = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 5 );
	self->social_security_combined_tax_rate = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 6 );
	self->social_security_payroll_ceiling = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 7 );
	self->medicare_combined_tax_rate = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 8 );
	self->medicare_additional_withholding_rate = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 9 );
	self->medicare_additional_gross_pay_floor = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 10 );
	self->federal_unemployment_wage_base = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 11 );
	self->federal_unemployment_tax_minimum_rate = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 12 );
	self->federal_unemployment_tax_standard_rate = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 13 );
	self->federal_withholding_allowance_period_value = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 14 );
	self->federal_nonresident_withholding_income_premium =
		atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 15 );
	self->state_unemployment_wage_base = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 16 );
	self->state_unemployment_tax_rate = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 17 );
	self->federal_unemployment_threshold_rate = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 18 );
	self->state_withholding_allowance_period_value = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 19 );
	self->state_itemized_allowance_period_value = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 20 );
	self->state_sales_tax_rate = atof( piece_buffer );

	return self;

} /* entity_self_load() */

boolean entity_load(	char **city,
			char **state_code,
			char **zip_code,
			char **phone_number,
			char **email_address,
			char *application_name,
			char *full_name,
			char *street_address )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *select;
	char *results;
	char buffer[ 128 ];

	select =
"city,state_code,zip_code,phone_number,email_address";

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' 		",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=entity			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	if ( city )
	{
		piece( buffer, FOLDER_DATA_DELIMITER, results, 0 );
		*city = strdup( buffer );
	}

	if ( state_code )
	{
		piece( buffer, FOLDER_DATA_DELIMITER, results, 1 );
		*state_code = strdup( buffer );
	}

	if ( zip_code )
	{
		piece( buffer, FOLDER_DATA_DELIMITER, results, 2 );
		*zip_code = strdup( buffer );
	}

	if ( phone_number )
	{
		piece( buffer, FOLDER_DATA_DELIMITER, results, 3 );
		*phone_number = strdup( buffer );
	}

	if ( email_address )
	{
		piece( buffer, FOLDER_DATA_DELIMITER, results, 4 );
		*email_address = strdup( buffer );
	}

	free( results );

	return 1;

} /* entity_load() */

void entity_propagate_purchase_order_ledger_accounts(
				char *application_name,
				char *fund_name,
				char *purchase_order_transaction_date_time )
{
	LIST *inventory_account_name_list;
	char *sales_tax_expense_account = {0};
	char *freight_in_expense_account = {0};
	char *account_payable_account = {0};
	char *specific_inventory_account = {0};
	char *cost_of_goods_sold_account = {0};

	inventory_account_name_list =
		ledger_get_inventory_account_name_list(
			application_name );

	ledger_get_purchase_order_account_names(
		&sales_tax_expense_account,
		&freight_in_expense_account,
		&account_payable_account,
		application_name,
		fund_name );

	if ( list_length( inventory_account_name_list ) )
	{
		ledger_propagate_account_name_list(
			application_name,
			purchase_order_transaction_date_time,
			inventory_account_name_list );
	}

	if ( sales_tax_expense_account )
	{
		ledger_propagate(
			application_name,
			purchase_order_transaction_date_time,
			sales_tax_expense_account );
	}

	if ( freight_in_expense_account )
	{
		ledger_propagate(
			application_name,
			purchase_order_transaction_date_time,
			freight_in_expense_account );
	}

	if ( account_payable_account )
	{
		ledger_propagate(
			application_name,
			purchase_order_transaction_date_time,
			account_payable_account );
	}

} /* entity_propagate_purchase_order_ledger_accounts() */

ENTITY_SELF *entity_self_inventory_load(
			char *application_name,
			char *inventory_name )
{
	ENTITY_SELF *entity_self;

	if ( ! ( entity_self = entity_self_load( application_name ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot fetch from SELF.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	entity_self->sale_inventory =
		inventory_load_new(
			application_name,
			inventory_name );

	entity_self->inventory_purchase_hash_table =
		inventory_get_arrived_inventory_purchase_hash_table(
			&entity_self->
				sale_inventory->
				inventory_purchase_list,
			&entity_self->inventory_purchase_name_list,
			application_name,
			inventory_name );

	entity_self->inventory_sale_hash_table =
		inventory_get_completed_inventory_sale_hash_table(
			&entity_self->
				sale_inventory->
				inventory_sale_list,
			&entity_self->inventory_sale_name_list,
			application_name,
			inventory_name );

	entity_self->transaction_hash_table =
		ledger_get_transaction_hash_table(
			application_name,
			inventory_name );

	entity_self->journal_ledger_hash_table =
		ledger_get_journal_ledger_hash_table(
			application_name,
			inventory_name );

	entity_self->purchase_order_list =
		purchase_get_inventory_purchase_order_list(
			application_name,
			inventory_name,
			entity_self->inventory_purchase_hash_table,
			entity_self->inventory_purchase_name_list,
			entity_self->transaction_hash_table,
			entity_self->journal_ledger_hash_table );

	entity_self->inventory_list =
		entity_get_inventory_list(
			application_name );

	entity_self->customer_sale_list =
		customer_get_inventory_customer_sale_list(
			application_name,
			inventory_name,
			entity_self->inventory_sale_hash_table,
			entity_self->inventory_sale_name_list,
			entity_self->transaction_hash_table,
			entity_self->journal_ledger_hash_table,
			entity_self->inventory_list );

	return entity_self;

} /* entity_self_inventory_load() */

char *entity_get_title_passage_rule_string(
				enum title_passage_rule title_passage_rule )
{
	if ( title_passage_rule == title_passage_rule_null )
		return TITLE_PASSAGE_RULE_NULL;
	else
	if ( title_passage_rule == FOB_shipping )
		return TITLE_PASSAGE_RULE_SHIPPED_DATE;
	else
	if ( title_passage_rule == FOB_destination )
		return TITLE_PASSAGE_RULE_ARRIVED_DATE;
	else
		return TITLE_PASSAGE_RULE_NULL;

} /* entity_get_title_passage_rule_string() */

enum title_passage_rule entity_get_title_passage_rule(
				char *title_passage_rule_string )
{
	if ( !title_passage_rule_string )
	{
		return title_passage_rule_null;
	}
	else
	if ( timlib_strcmp(
			title_passage_rule_string,
			TITLE_PASSAGE_RULE_SHIPPED_DATE ) == 0 )
	{
		return FOB_shipping;
	}
	else
	if ( timlib_strcmp(
			title_passage_rule_string,
			TITLE_PASSAGE_RULE_ARRIVED_DATE ) == 0 )
	{
		return FOB_destination;
	}
	else
	{
		return title_passage_rule_null;
	}

} /* entity_get_title_passage_rule() */

LIST *entity_get_inventory_list(
			char *application_name )
{
	INVENTORY *inventory;
	LIST *inventory_list;
	char *select;
	char sys_string[ 512 ];
	char input_buffer[ 512 ];
	char piece_buffer[ 128 ];
	FILE *input_pipe;

	select =
"inventory_name,inventory_account,cost_of_goods_sold_account";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=inventory	",
		 application_name,
		 select );

	input_pipe = popen( sys_string, "r" );

	inventory_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		inventory =
			inventory_new(
				strdup( piece_buffer ) );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		if ( !*piece_buffer )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: empty inventory_account for inventory = (%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 inventory->inventory_name );
			pclose( input_pipe );
			exit( 1 );
		}

		inventory->inventory_account_name = strdup( piece_buffer );

		piece(	piece_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		if ( !*piece_buffer )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: empty cost_of_goods_sold_account_name for inventory = (%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 inventory->inventory_name );
			pclose( input_pipe );
			exit( 1 );
		}

		inventory->cost_of_goods_sold_account_name =
			strdup( piece_buffer );

		list_append_pointer( inventory_list, inventory );
	}

	pclose( input_pipe );
	return inventory_list;

} /* entity_get_inventory_list() */

ENTITY *entity_get_sales_tax_payable_entity(
				char *application_name )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char *results;

	select = "full_name,street_address";

	folder = "sales_tax_payable_entity";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		",
		 application_name,
		 select,
		 folder );

	results = pipe2string( sys_string );

	if ( !results ) return (ENTITY *)0;

	piece( full_name, FOLDER_DATA_DELIMITER, results, 0 );
	piece( street_address, FOLDER_DATA_DELIMITER, results, 1 );

	return entity_new(	strdup( full_name ),
				strdup( street_address ) );

} /* entity_get_sales_tax_payable_entity() */

char *entity_get_payroll_pay_period_string(
				enum payroll_pay_period
					payroll_pay_period )
{
	if ( payroll_pay_period == pay_period_not_set )
		return "weekly";
	else
	if ( payroll_pay_period == pay_period_weekly )
		return "weekly";
	else
	if ( payroll_pay_period == pay_period_biweekly )
		return "biweekly";
	else
	if ( payroll_pay_period == pay_period_semimonthly )
		return "semimonthly";
	else
	if ( payroll_pay_period == pay_period_monthly )
		return "monthly";

	fprintf( stderr,
"ERROR in %s/%s()/%d: unrecognized payroll_pay_period = %d.\n",
		 __FILE__,
		 __FUNCTION__,
		 __LINE__,
		 payroll_pay_period );

	exit( 1 );

} /* entity_get_payroll_pay_period_string() */

ENTITY *entity_seek(		LIST *entity_list,
				char *full_name,
				char *street_address )
{
	ENTITY *entity;

	if ( !entity_list )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: entity_list is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( entity_list ) ) return (ENTITY *)0;

	do {
		entity = list_get_pointer( entity_list );

		if ( timlib_strcmp( entity->full_name, full_name ) == 0
		&&   timlib_strcmp(	entity->street_address,
					street_address ) == 0 )
		{
			return entity;
		}

	} while( list_next( entity_list ) );

	return (ENTITY *)0;

} /* entity_seek() */

ENTITY *entity_get_or_set(	LIST *entity_list,
				char *full_name,
				char *street_address,
				boolean with_strdup )
{
	ENTITY *entity;

	if ( ! ( entity =
			entity_seek(
				entity_list,
				full_name,
				street_address ) ) )
	{

		if ( with_strdup )
		{
			entity = entity_new(	strdup( full_name ),
						strdup( street_address ) );
		}
		else
		{
			entity = entity_new( full_name, street_address );
		}

		list_append_pointer( entity_list, entity );
	}

	return entity;

} /* entity_get_or_set() */

char *entity_list_display( LIST *entity_list )
{
	ENTITY *entity;
	char buffer[ 65536 ];
	char *ptr = buffer;

	*ptr = '\0';

	if ( list_rewind( entity_list ) )
	{
		do {
			entity = list_get_pointer( entity_list );

			ptr += sprintf(	ptr,
					"Entity: %s/%s, sum_balance = %.2lf\n",
					entity->full_name,
					entity->street_address,
					entity->sum_balance );

		} while( list_next( entity_list ) );
	}

	return strdup( buffer );

} /* entity_list_display() */

boolean entity_list_exists(	LIST *entity_list,
				char *full_name,
				char *street_address )
{
	if ( !entity_list )
		return 0;
	else
		return (boolean)entity_seek(
					entity_list,
					full_name,
					street_address );

} /* entity_list_exists() */

boolean entity_location_fetch(	char **city,
				char **state_code,
				char **zip_code,
				char *application_name,
				char *full_name,
				char *street_address )
{
	return entity_load(	city,
				state_code,
				zip_code,
				(char **)0 /* phone_number */,
				(char **)0 /* email_address */,
				application_name,
				full_name,
				street_address );

} /* entity_location_fetch() */


