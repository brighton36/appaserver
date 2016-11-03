/* library/html_invoice.h						*/
/* -------------------------------------------------------------------- */
/* This is the appaserver html_invoice ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef HTML_INVOICE_H
#define HTML_INVOICE_H

#include "list.h"

/* Constants */
/* --------- */
#define HTML_INVOICE_EXTENSION \
	( (double)quantity * retail_price - discount_amount )

/* Structures */
/* ---------- */
typedef struct
{
	char *item_key;
	char *item;
	int quantity;
	double retail_price;
	double discount_amount;
} HTML_INVOICE_LINE_ITEM;

typedef struct
{
	char *invoice_key;
	char *name;
	char *street_address;
	char *suite_number;
	char *city;
	char *state;
	char *zip_code;
	char *customer_service_key;
	LIST *invoice_line_item_list;
	double sales_tax;
	double shipping_charge;
	double extension_total;
	boolean exists_discount_amount;
} HTML_INVOICE_CUSTOMER;

typedef struct
{
	char *name;
	char *street_address;
	char *suite_number;
	char *city;
	char *state;
	char *zip_code;
} HTML_INVOICE_COMPANY;

typedef struct
{
	char *invoice_date;
	char *line_item_key_heading;
	HTML_INVOICE_COMPANY invoice_company;
	HTML_INVOICE_CUSTOMER *invoice_customer;
	boolean omit_money;
} HTML_INVOICE;

/* Operations */
/* ---------- */
HTML_INVOICE *html_invoice_new(		char *invoice_date,
					char *company_name,
					char *company_street_address,
					char *company_suite_number,
					char *company_city,
					char *company_state,
					char *company_zip_code,
					char *line_item_key_heading );

HTML_INVOICE_CUSTOMER *html_invoice_customer_new(
					char *invoice_key,
					char *customer_name,
					char *customer_street_address,
					char *customer_suite_number,
					char *customer_city,
					char *customer_state,
					char *customer_zip_code,
					char *customer_service_key,
					double sales_tax,
					double shipping_charge );

void html_invoice_customer_free(	HTML_INVOICE_CUSTOMER *
						html_invoice_customer );

double html_invoice_append_line_item(	LIST *invoice_line_item_list,
					char *item_key,
					char *item,
					int quantity,
					double retail_price,
					double discount_amount );

HTML_INVOICE_LINE_ITEM *html_invoice_line_item_new(
					char *item_key,
					char *item,
					int quantity,
					double retail_price,
					double discount_amount );

void html_invoice_line_item_free(	HTML_INVOICE_LINE_ITEM *
						html_invoice_line_item );

void html_invoice_output_header(	FILE *output_stream );

void html_invoice_output_invoice_header(FILE *output_stream,
					char *invoice_date,
					char *line_item_key_heading,
					HTML_INVOICE_COMPANY *
						html_invoice_company,
	       				HTML_INVOICE_CUSTOMER *
						html_invoice_customer,
					boolean exists_discount_amount,
					char *title,
					boolean omit_money );

void html_invoice_output_invoice_line_items(
					FILE *output_stream,
					LIST *invoice_line_item_list,
					boolean exists_discount_amount,
					boolean omit_money );

void html_invoice_output_invoice_footer(
					FILE *output_stream,
					double extension_total,
					double sales_tax,
					double shipping_charge,
					char *line_item_key_heading,
					boolean exists_discount_amount );

void html_invoice_customer_free( 	HTML_INVOICE_CUSTOMER *
						invoice_customer );

void html_invoice_company_free(		HTML_INVOICE_COMPANY *
						invoice_company );

void html_invoice_free(			HTML_INVOICE *invoice );

boolean html_invoice_get_exists_discount_amount(
					LIST *invoice_line_item_list );

#endif

