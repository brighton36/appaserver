#!/bin/bash

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

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 ignored where state" 1>&2
	exit 1
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
