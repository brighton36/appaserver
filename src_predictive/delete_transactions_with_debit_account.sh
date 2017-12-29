#!/bin/bash
# ----------------------------------------------------
# src_ledger/delete_transactions_with_debit_account.sh
# ----------------------------------------------------
#
# Freely available software: see Appaserver.org
# ----------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application transaction_date account_list" 1>&2
	exit 1
fi

application=$1
transaction_date=$2
account_list=$3

transaction=`get_table_name $application transaction`
journal_ledger=`get_table_name $application journal_ledger`

where="	transaction_date = '${transaction_date}' and			\
	credit_amount is not null and					\
	account in (`single_quotes_around.e ${account_list}`)"

sql_statement="	select transaction_date,transaction_time		\
		from $journal_ledger					\
		where $where;"

for transaction_date_time in `echo "$sql_statement" | sql.e | sort -u`
do
	transaction_date=`echo "${transaction_date_time}" | piece.e '^' 0`
	transaction_time=`echo "${transaction_date_time}" | piece.e '^' 1`

	where="	transaction_date = '${transaction_date}' and		\
		transaction_time = '${transaction_time}'"

	echo "delete from ${journal_ledger} where $where;" | sql.e
	echo "delete from ${transaction} where $where;" | sql.e

done

exit 0

