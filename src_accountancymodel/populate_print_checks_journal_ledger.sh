#!/bin/bash
# -----------------------------------------------------------------------------
# $APPASERVER_HOME/src_accountancymodel/populate_print_checks_journal_ledger.sh
# -----------------------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# -----------------------------------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)
database=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
else
	export DATABASE=$application
fi

select_account="account"
table_account="account"
where_account="subclassification = 'current_liability'"

echo "select $select_account from $table_account where $where_account"	|
sql.e									|
while read account
do
	account_escaped=`echo $account | escape_character.e "'"`

	if [ "$account_escaped" = "uncleared_checks" ]
	then
		continue
	fi

	select="max(transaction_date_time)"
	where="account = '$account_escaped'"
	table=journal_ledger

	transaction_date_time=`\
		echo "select $select from $table where $where;" | sql.e`

	select="balance"
	where="account = '$account_escaped' and transaction_date_time = '$transaction_date_time'"
	table=journal_ledger

	balance=`\
		echo "select $select from $table where $where;" | sql.e`

	if [ "$balance" = "0.00" ]
	then
		continue
	fi

	select="full_name,street_address,'',concat(account,' [',balance,']')"
	where="transaction_date_time = '$transaction_date_time' and account = '$account_escaped'"

	echo "select $select from $table where $where;" | sql.e

done

exit 0
