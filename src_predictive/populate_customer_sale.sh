#!/bin/bash
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_accountancymodel/populate_tax_form_line.sh
# ---------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 ignored one2m_folder where" 1>&2
	exit 1
fi

echo $0 $* 1>&2

one2m_folder=$2
parameter_where=$3

if [ "$one2m_folder" = "activity_work" ]
then
	where="$parameter_where and completed_date_time is not null"
else
	where="$parameter_where"
fi

table=$(get_table_name $application customer_sale)
select="full_name,street_address,sale_date_time"

echo "select $select from $table where $where order by $select;"	|
sql.e

exit 0
