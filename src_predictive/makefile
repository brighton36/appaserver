# src_accountancymodel/makefile
# -----------------------------

CC=gcc
#CFLAGS=-g
#CFLAGS=-O3 -s -Wall -W
CFLAGS=-O3 -s -Wall ${APPASERVER_UNUSED_PARAMETER_FLAG}

LIBRARY=${APPASERVER_HOME}/library

STANDARD_LIBRARY=							\
		${LIBRARY}/date.o 					\
		${LIBRARY}/julian.o 					\
		${LIBRARY}/dictionary.o 				\
		${LIBRARY}/list.o 					\
		${LIBRARY}/list_usage.o 				\
		${LIBRARY}/timlib.o 					\
		${LIBRARY}/piece.o 					\
		${LIBRARY}/column.o 					\
		${LIBRARY}/date_convert.o 				\
		${LIBRARY}/hashtbl.o

EXECUTABLES=	trial_balance						\
		tax_form_report						\
		income_statement					\
		balance_sheet						\
		program_profit						\
		ledger_propagate					\
		ledger_unbalance					\
		propagate_purchase_order_accounts			\
		propagate_customer_sale_accounts			\
		ledger_debit_credit_audit				\
		close_nominal_accounts					\
		pay_liabilities_process					\
		populate_print_checks_entity				\
		load_bank_spreadsheet					\
		inventory_balance_detail				\
		propagate_inventory_purchase_layers			\
		propagate_inventory_sale_layers				\
		depreciate_fixed_assets					\
		depreciate_prior_fixed_assets				\
		post_change_supply_purchase				\
		post_change_inventory_purchase				\
		post_change_fixed_asset_purchase			\
		post_change_fixed_asset					\
		post_change_inventory_sale				\
		post_change_fixed_service_sale				\
		post_change_hourly_service_sale				\
		post_change_fixed_service_work				\
		post_change_hourly_service_work				\
		post_change_purchase_order				\
		post_change_customer_sale				\
		post_change_customer_payment				\
		post_change_prior_fixed_asset				\
		post_change_account_balance				\
		post_change_vendor_payment				\
		post_change_prepaid_asset_purchase			\
		post_change_prepaid_asset_accrual			\
		post_change_bank_upload					\
		update_sale_completed					\
		post_reoccurring_transaction				\
		post_employee_work_period				\
		collect_customer_payment				\
		make_vendor_payment					\
		post_change_self					\
		post_change_employee_work_day				\
		inventory_cost_method_propagate				\
		load_receipt_scan					\
		shortcut_service_work

EXTERNAL_LIBRARY=							\
		${STANDARD_LIBRARY}					\
		${LIBRARY}/date_aggregate.o 				\
		${LIBRARY}/aggregate_level.o 				\
		${LIBRARY}/aggregate_statistic.o 			\
		${LIBRARY}/html_table.o 				\
		${LIBRARY}/prompt_recursive.o 				\
		${LIBRARY}/decode_html_post.o				\
		${LIBRARY}/appaserver_parameter_file.o			\
		${LIBRARY}/appaserver_library.o				\
		${LIBRARY}/appaserver_error.o				\
		${LIBRARY}/application_constants.o			\
		${LIBRARY}/process_generic_output.o			\
		${LIBRARY}/name_arg.o					\
		${LIBRARY}/basename.o					\
		${LIBRARY}/semaphore.o					\
		${LIBRARY}/fopen_path.o					\
		${LIBRARY}/environ.o					\
		${LIBRARY}/document.o					\
		${LIBRARY}/application.o				\
		${LIBRARY}/appaserver_user.o				\
		${LIBRARY}/process_parameter_list.o			\
		${LIBRARY}/populate_helper_process.o			\
		${LIBRARY}/appaserver_link_file.o			\
		${LIBRARY}/related_folder.o				\
		${LIBRARY}/folder.o					\
		${LIBRARY}/role.o					\
		${LIBRARY}/appaserver.o					\
		${LIBRARY}/dictionary_appaserver.o			\
		${LIBRARY}/query.o					\
		${LIBRARY}/latex.o					\
		${LIBRARY}/process.o					\
		${LIBRARY}/attribute.o					\
		${LIBRARY}/form.o					\
		${LIBRARY}/element.o					\
		${LIBRARY}/operation.o					\
		${LIBRARY}/role_folder.o				\
		${LIBRARY}/session.o					\
		${LIBRARY}/frameset.o					\
		${LIBRARY}/insert_database.o				\
		${LIBRARY}/update_database.o				\
		${LIBRARY}/operation_list.o				\
		${LIBRARY}/validation_form.o				\
		${LIBRARY}/post2dictionary.o				\
		${LIBRARY}/dictionary2file.o

SOURCE_LIBRARY= 							\
		ledger.o						\
		purchase.o						\
		customer.o						\
		inventory.o						\
		depreciation.o						\
		accrual.o						\
		employee.o						\
		entity.o

all: 	${SOURCE_LIBRARY} ${EXECUTABLES}

touch:
	/usr/bin/touch *.c

clean:
	/bin/rm -f ${SOURCE_LIBRARY} ${EXECUTABLES}			\
		financial_position					\
		statement_of_activities					\
		${CGI_HOME}/post_trial_balance

post_trial_balance: post_trial_balance.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_trial_balance.c					\
		-o post_trial_balance
	touch ${CGI_HOME}/post_trial_balance
	/bin/rm -f ${CGI_HOME}/post_trial_balance
	ln ${APPASERVER_LINK_FLAGS}					\
	${APPASERVER_HOME}/src_accountancymodel/post_trial_balance	\
	${CGI_HOME}

post_reoccurring_transaction: post_reoccurring_transaction.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_reoccurring_transaction.c				\
		-o post_reoccurring_transaction

post_employee_work_period: post_employee_work_period.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_employee_work_period.c				\
		-o post_employee_work_period

shortcut_service_work: shortcut_service_work.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		shortcut_service_work.c					\
		-o shortcut_service_work

ledger_propagate: ledger_propagate.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		ledger_propagate.c					\
		-o ledger_propagate

load_receipt_scan: load_receipt_scan.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		load_receipt_scan.c					\
		-o load_receipt_scan

ledger_unbalance: ledger_unbalance.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		ledger_unbalance.c					\
		-o ledger_unbalance

propagate_purchase_order_accounts: propagate_purchase_order_accounts.c	\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		propagate_purchase_order_accounts.c			\
		-o propagate_purchase_order_accounts

propagate_customer_sale_accounts: propagate_customer_sale_accounts.c	\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		propagate_customer_sale_accounts.c			\
		-o propagate_customer_sale_accounts

ledger_debit_credit_audit: ledger_debit_credit_audit.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		ledger_debit_credit_audit.c				\
		-o ledger_debit_credit_audit

depreciate_fixed_assets: depreciate_fixed_assets.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		depreciate_fixed_assets.c				\
		-o depreciate_fixed_assets

depreciate_prior_fixed_assets: depreciate_prior_fixed_assets.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		depreciate_prior_fixed_assets.c				\
		-o depreciate_prior_fixed_assets

trial_balance: trial_balance.c						\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		trial_balance.c						\
		-o trial_balance

tax_form_report: tax_form_report.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY} tax.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		tax_form_report.c					\
		tax.o							\
		-o tax_form_report

program_profit: program_profit.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		program_profit.c					\
		-o program_profit

income_statement: income_statement.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		income_statement.c					\
		-o income_statement
	touch statement_of_activities
	/bin/rm -f statement_of_activities
	ln income_statement statement_of_activities

balance_sheet: balance_sheet.c						\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		balance_sheet.c						\
		-o balance_sheet
	touch financial_position
	/bin/rm -f financial_position
	ln balance_sheet financial_position

close_nominal_accounts: close_nominal_accounts.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		close_nominal_accounts.c				\
		-o close_nominal_accounts

pay_liabilities_process: pay_liabilities_process.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY} pay_liabilities.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		pay_liabilities_process.c				\
		pay_liabilities.o					\
		-o pay_liabilities_process

populate_print_checks_entity: populate_print_checks_entity.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY} pay_liabilities.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		populate_print_checks_entity.c				\
		pay_liabilities.o					\
		-o populate_print_checks_entity

load_bank_spreadsheet: load_bank_spreadsheet.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		load_bank_spreadsheet.c					\
		-o load_bank_spreadsheet

post_change_prepaid_asset_accrual: post_change_prepaid_asset_accrual.c	\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_prepaid_asset_accrual.c			\
		-o post_change_prepaid_asset_accrual

post_change_bank_upload: post_change_bank_upload.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_bank_upload.c				\
		-o post_change_bank_upload

post_change_inventory_purchase: post_change_inventory_purchase.c	\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_inventory_purchase.c			\
		-o post_change_inventory_purchase

post_change_supply_purchase: post_change_supply_purchase.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_supply_purchase.c				\
		-o post_change_supply_purchase

post_change_fixed_service_sale:						\
	post_change_fixed_service_sale.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_fixed_service_sale.c			\
		-o post_change_fixed_service_sale

post_change_hourly_service_sale:					\
	post_change_hourly_service_sale.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_hourly_service_sale.c			\
		-o post_change_hourly_service_sale

post_change_fixed_service_work:						\
	post_change_fixed_service_work.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_fixed_service_work.c			\
		-o post_change_fixed_service_work

post_change_hourly_service_work:					\
	post_change_hourly_service_work.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_hourly_service_work.c			\
		-o post_change_hourly_service_work

post_change_specific_inventory_purchase:				\
	post_change_specific_inventory_purchase.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_specific_inventory_purchase.c		\
		-o post_change_specific_inventory_purchase

post_change_service_purchase: post_change_service_purchase.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_service_purchase.c				\
		-o post_change_service_purchase

post_change_fixed_asset_purchase:					\
		post_change_fixed_asset_purchase.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_fixed_asset_purchase.c			\
		-o post_change_fixed_asset_purchase

post_change_prepaid_asset_purchase:					\
		post_change_prepaid_asset_purchase.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_prepaid_asset_purchase.c			\
		-o post_change_prepaid_asset_purchase

post_change_fixed_asset:						\
		post_change_fixed_asset.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_fixed_asset.c				\
		-o post_change_fixed_asset

propagate_inventory_purchase_layers:					\
	propagate_inventory_purchase_layers.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		propagate_inventory_purchase_layers.c			\
		-o propagate_inventory_purchase_layers

propagate_inventory_sale_layers:					\
	propagate_inventory_sale_layers.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		propagate_inventory_sale_layers.c			\
		-o propagate_inventory_sale_layers

inventory_balance_detail:						\
	inventory_balance_detail.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		inventory_balance_detail.c				\
		-o inventory_balance_detail

post_change_inventory_sale: post_change_inventory_sale.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_inventory_sale.c				\
		-o post_change_inventory_sale

post_change_specific_inventory_sale:					\
	post_change_specific_inventory_sale.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_specific_inventory_sale.c			\
		-o post_change_specific_inventory_sale

post_change_purchase_order: post_change_purchase_order.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_purchase_order.c				\
		-o post_change_purchase_order

post_change_vendor_payment: post_change_vendor_payment.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_vendor_payment.c				\
		-o post_change_vendor_payment

post_change_customer_payment: post_change_customer_payment.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY} subsidiary_transaction.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		subsidiary_transaction.o				\
		post_change_customer_payment.c				\
		-o post_change_customer_payment

post_change_prior_fixed_asset: post_change_prior_fixed_asset.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY} subsidiary_transaction.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		subsidiary_transaction.o				\
		post_change_prior_fixed_asset.c				\
		-o post_change_prior_fixed_asset

post_change_account_balance: post_change_account_balance.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY} investment.o
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		investment.o						\
		post_change_account_balance.c				\
		-o post_change_account_balance

post_change_customer_sale: post_change_customer_sale.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_customer_sale.c				\
		-o post_change_customer_sale

synchronize_customer_sale_transaction:					\
	synchronize_customer_sale_transaction.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		synchronize_customer_sale_transaction.c			\
		-o synchronize_customer_sale_transaction

update_sale_completed: update_sale_completed.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		update_sale_completed.c					\
		-o update_sale_completed
	touch update_sale_not_completed
	/bin/rm -f update_sale_not_completed
	ln update_sale_completed update_sale_not_completed

collect_customer_payment: collect_customer_payment.c			\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		collect_customer_payment.c				\
		-o collect_customer_payment

make_vendor_payment: make_vendor_payment.c				\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		make_vendor_payment.c					\
		-o make_vendor_payment

post_change_self: post_change_self.c					\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_self.c					\
		-o post_change_self

post_change_employee_work_day: post_change_employee_work_day.c		\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		post_change_employee_work_day.c				\
		-o post_change_employee_work_day

inventory_cost_method_propagate: inventory_cost_method_propagate.c	\
	${EXTERNAL_LIBRARY} ${SOURCE_LIBRARY}
	${CC}	${CFLAGS} -I${LIBRARY}					\
		${EXTERNAL_LIBRARY}					\
		${SOURCE_LIBRARY}					\
		inventory_cost_method_propagate.c			\
		-o inventory_cost_method_propagate

ledger.o: ledger.c ledger.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c ledger.c

inventory.o: inventory.c inventory.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c inventory.c

purchase.o: purchase.c purchase.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c purchase.c

customer.o: customer.c customer.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c customer.c

depreciation.o: depreciation.c depreciation.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c depreciation.c

accrual.o: accrual.c accrual.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c accrual.c

entity.o: entity.c entity.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c entity.c

employee.o: employee.c employee.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c employee.c

pay_liabilities.o: pay_liabilities.c pay_liabilities.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c pay_liabilities.c

tax.o: tax.c tax.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c tax.c

subsidiary_transaction.o: subsidiary_transaction.c			\
		subsidiary_transaction.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c subsidiary_transaction.c

investment.o: investment.c						\
		investment.h
	${CC}	${CFLAGS} -I${LIBRARY}					\
		-c investment.c
