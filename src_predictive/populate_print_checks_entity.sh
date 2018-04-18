#!/bin/bash
# ---------------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/populate_print_checks_entity.sh
# ---------------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application fund filter" 1>&2

	echo "filter = {both,taxes,not_taxes}" 1>&2
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

fund=$2
filter=$3

select="full_name, concat( street_address, ' [', sum( ifnull( credit_amount, 0) ) - sum( ifnull( debit_amount, 0 ) ),']' )"

from="journal_ledger, account"

join_where="journal_ledger.account = account.account"

if [ "$fund" != "" -a "$fund" != "fund" ]
then
	fund_where="account.fund = '$fund'"
else
	fund_where="1 = 1"
fi

group="full_name, street_address"

having="sum( ifnull( credit_amount, 0 ) ) - sum( ifnull( debit_amount, 0 ) ) > 0"

function select_both()
{
	account_where="	account.subclassification = 'current_liability' and
			journal_ledger.account <> 'uncleared_checks'"

	where="$join_where and $account_where and $fund_where"

	echo "	select $select
		from $from
		where $where
		group by $group
		having $having;" |
	sql.e
}

function select_taxes()
{
	account_where="account.account = 'sales_tax_payable'"

	where="$join_where and $account_where and $fund_where"

	echo "	select $select
		from $from
		where $where
		group by $group
		having $having;" |
	sql.e
}

function select_not_taxes()
{
	account_where="	account.subclassification = 'current_liability' and
			journal_ledger.account <> 'uncleared_checks' and
			account.account not in
			(select account from liability_account_entity)"

	where="$join_where and $account_where and $fund_where"

	echo "	select $select
		from $from
		where $where
		group by $group
		having $having;" |
	sql.e

}

if [ "$filter" = "both" ]
then
	select_both
elif [ "$filter" = "taxes" ]
then
	select_taxes
elif [ "$filter" = "not_taxes" ]
then
	select_not_taxes
fi

exit 0
