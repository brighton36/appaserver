#!/bin/bash
# -------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/bank_upload_ledger_pending.sh
# -------------------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

select="transaction_date_time,full_name,street_address,debit_amount,credit_amount"

order="transaction_date_time"

table="journal_ledger,account"

journal_ledger_where="	journal_ledger.account = account.account and	\
			account.hard_coded_account_key = 'cash_key'"

subquery_where="not exists (
		select 1						\
		from bank_upload_transaction				\
		where journal_ledger.full_name =			\
			bank_upload_transaction.full_name and		\
		      journal_ledger.street_address =			\
			bank_upload_transaction.street_address and	\
		      journal_ledger.transaction_date_time =		\
			bank_upload_transaction.transaction_date_time )"

development_where="transaction_date_time >= '2018-01-01'"

where="$journal_ledger_where and $subquery_where and $development_where"

echo "select $select from $table where $where order by $order;"		|
sql.e									|
cat

exit 0
