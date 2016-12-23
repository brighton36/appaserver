#!/bin/bash
# ---------------------------------------------------------------------
# $APPASERVER_HOME/src_accountancymodel/populate_print_checks_entity.sh
# ---------------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------------------------------

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

select="full_name, concat( street_address, ' [', sum( ifnull( credit_amount, 0) ) - sum( ifnull( debit_amount, 0 ) ),']' )"

from="journal_ledger, account"

join_where="journal_ledger.account = account.account"

account_where="account.subclassification = 'current_liability' and journal_ledger.account <> 'uncleared_checks'"

group="full_name, street_address"

having="sum( ifnull( credit_amount, 0 ) ) - sum( ifnull( debit_amount, 0 ) ) > 0"

where="$join_where and $account_where"

echo "select $select from $from where $where group by $group having $having;" |
sql.e

exit 0
