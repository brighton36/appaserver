/* -------------------------------------------------------------------- */
/* src_accountancymodel/entity.h					*/
/* -------------------------------------------------------------------- */
/* This is the appaserver entity ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ENTITY_H
#define ENTITY_H

#include "boolean.h"
#include "hash_table.h"
#include "inventory.h"

/* Enumerated types */
/* ---------------- */
enum title_passage_rule{	title_passage_rule_null,
				FOB_shipping,
				FOB_destination };

enum inventory_cost_method{	inventory_not_set,
				inventory_fifo,
				inventory_average,
				inventory_lifo };

/* Constants */
/* --------- */
#define REFRESH_COMPLETELY	1

#define TITLE_PASSAGE_RULE_SHIPPED_DATE "FOB_shipping"
#define TITLE_PASSAGE_RULE_ARRIVED_DATE "FOB_destination"
#define TITLE_PASSAGE_RULE_NULL		""

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *unit;
	char *city;
	char *state_code;
	char *zip_code;
	char *phone_number;
	char *email_address;
	boolean sales_tax_exempt;
} ENTITY;

typedef struct
{
	ENTITY *entity;
	enum inventory_cost_method inventory_cost_method;
	LIST *purchase_order_list;
	LIST *customer_sale_list;
	INVENTORY *purchase_inventory;
	INVENTORY *sale_inventory;
	HASH_TABLE *inventory_purchase_hash_table;
	LIST *inventory_purchase_name_list;
	HASH_TABLE *inventory_sale_hash_table;
	LIST *inventory_sale_name_list;
	HASH_TABLE *transaction_hash_table;
	HASH_TABLE *journal_ledger_hash_table;
} ENTITY_SELF;

/* Operations */
/* ---------- */
LIST *entity_get_purchase_order_list(
					char *application_name,
					char *inventory_name );

LIST *entity_get_customer_sale_list(
					char *application_name,
					char *inventory_name );

ENTITY *entity_new(			char *full_name,
					char *street_address );

boolean entity_load(			char **unit,
					char **city,
					char **state_code,
					char **zip_code,
					char **phone_number,
					char **email_address,
					boolean *sales_tax_exempt,
					char *application_name,
					char *full_name,
					char *street_address );

ENTITY_SELF *entity_self_load(		char *application_name );

ENTITY_SELF *entity_self_new(		char *full_name,
					char *street_address );

LIST *entity_get_inventory_purchase_order_list(
					char *application_name,
					char *inventory_name );

LIST *entity_get_inventory_customer_sale_list(
					char *application_name,
					char *inventory_name );

enum inventory_cost_method entity_get_inventory_cost_method(
					char *inventory_cost_method_string );

void entity_propagate_purchase_order_ledger_accounts(
				char *application_name,
				char *fund_name,
				char *purchase_order_transaction_date_time );

void entity_propagate_inventory_ledger_accounts(
				char *application_name,
				char *fund_name,
				char *customer_sale_transaction_date_time );

void entity_propagate_customer_sale_ledger_accounts(
				char *application_name,
				char *fund_name,
				char *customer_sale_transaction_date_time );

ENTITY_SELF *entity_self_purchase_inventory_load(
				char *application_name,
				char *inventory_name );

ENTITY_SELF *entity_self_sale_inventory_load(
				char *application_name,
				char *inventory_name );

enum title_passage_rule
	entity_get_title_passage_rule(
				char *title_passage_rule_string );

char *entity_get_title_passage_rule_string(
				enum title_passage_rule title_passage_rule );

#endif

