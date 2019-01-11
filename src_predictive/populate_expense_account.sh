#!/bin/bash
# -----------------------------------------------------------
# $APPASERVER_HOME/src_predictive/populate_expense_account.sh
# -----------------------------------------------------------

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

select="account"
table="account,subclassification"
where="account.subclassification = subclassification.subclassification and element = 'expense'"

echo "select $select from $table where $where order by $select;"	|
sql.e

exit 0
