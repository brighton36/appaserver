#!/bin/bash
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_accountancymodel/populate_tax_form_line.sh
# ---------------------------------------------------------------
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application one2m_folder where" 1>&2
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
