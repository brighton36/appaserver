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

select="concat( account.account, '|', chart_account_number, '---', account.account )"

subclassification_join="account.subclassification = subclassification.subclassification"

element_join="subclassification.element = element.element"

subset_where="element.element = 'expense'"

where="${element_join} and ${subclassification_join} and ${subset_where}"

from="element,account,subclassification"

echo "select $select from $from where $where order by $select;"		|
sql.e									|
grep -v '^$'								|
cat

exit 0
