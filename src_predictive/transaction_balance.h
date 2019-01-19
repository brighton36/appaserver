/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/transaction_balance.h		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef TRANSACTION_BALANCE_H
#define TRANSACTION_BALANCE_H

#include "list.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define TRANSACTION_BALANCE_SELECT		\
	"transaction_date_time,bank_date,bank_description,full_name,street_address,transaction_amount,bank_amount,balance,bank_running_balance,sequence_number"

/* Structures */
/* ---------- */
typedef struct
{
	char *transaction_date_time;
	char *bank_date;
	char *bank_description;
	char *full_name;
	char *street_address;
	double transaction_amount;
	double bank_amount;
	double balance;
	double bank_running_balance;
	int sequence_number;
} TRANSACTION_BALANCE_ROW;

typedef struct
{
	double cash_ending_balance;
	char *begin_date;
	LIST *transaction_balance_row_list;
} TRANSACTION_BALANCE_INPUT;

typedef struct
{
	TRANSACTION_BALANCE_ROW *begin_transaction_balance;
	TRANSACTION_BALANCE_ROW *end_transaction_balance;
	int block_count;
	boolean is_inbalance;
} TRANSACTION_BALANCE_BLOCK;

typedef struct
{
	TRANSACTION_BALANCE_INPUT input;
	LIST *inbalance_block_list;
	LIST *outbalance_block_list;
	LIST *merged_block_list;
} TRANSACTION_BALANCE;

/* Operations */
/* ---------- */
TRANSACTION_BALANCE *transaction_balance_calloc(
					void );

TRANSACTION_BALANCE_BLOCK *transaction_balance_block_new(
					void );

TRANSACTION_BALANCE_ROW *transaction_balance_row_new(
					void );

TRANSACTION_BALANCE *transaction_balance_new(
					char *application_name,
					char *begin_date,
					double cash_ending_balance );

LIST *transaction_balance_fetch_row_list(
					char *application_name,
					char *begin_date );

TRANSACTION_BALANCE_ROW *transaction_balance_parse_row(
					char *input_buffer );

LIST *transaction_balance_get_inbalance_block_list(
					LIST *transaction_balance_row_list );

LIST *transaction_balance_get_outbalance_block_list(
					LIST *transaction_balance_row_list );

LIST *transaction_balance_get_merged_block_list(
					LIST *inbalance_block_list,
					LIST *outbalance_block_list );

#endif

