#!/bin/bash
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/populate_tax_form_line.sh
# ---------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

if [ "$#" -eq 0 ]
then
	echo "Usage: $0 ignored [where]" 1>&2
	exit 1
fi

echo $0 $* 1>&2

if [ $# -eq 2 -a "$2" != "where" ]
then
	where=$2
else
	where="1 = 1"
fi

table=$(get_table_name $application tax_form_line)
select="tax_form,concat( tax_form_line, ' [', tax_form_description, ']' )"

echo "select $select from $table where $where order by $select;"	|
sql.e

exit 0
