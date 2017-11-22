#!/bin/bash
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application where state" 1>&2
	exit 1
fi

echo $0 $* 1>&2

application=$(echo $1 | piece.e ':' 0)
database=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
else
	export DATABASE=$application
fi

parameter_where=$2
state=$3

table=subclassification
select="subclassification"

# --------------------------------------------------------------------
# This doesn't fully work. We want to force donation inserts through
# <Insert> <Donation> <Donation>, not <Insert> <Ledger> <Transaction>.
#
# However, this prevents new donation revenue accounts from being
# inserted via <Insert> <Ledger> <Account>. If you need to insert a
# new donation revenue account, then comment this out.
# --------------------------------------------------------------------
if [ "$state" = "insert" ]
then
	where="$parameter_where and subclassification != 'donation'"
else
	where="$parameter_where"
fi

echo "select $select from $table where $where order by $select;"	|
sql.e

exit 0
