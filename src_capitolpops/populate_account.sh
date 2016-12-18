#!/bin/bash
# -----------------------------------
# src_capitolpops/populate_account.sh
# -----------------------------------
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

table=$(get_table_name $application account)
select="account"

subclassification_where="1 = 1"

if [ "$one2m_folder" = "donation_program" ]
then
	subclassification_where="subclassification = 'donation' or subclassification = 'dues'"
fi

if [ "$one2m_folder" = "fixed_asset" ]
then
	subclassification_where="subclassification = 'fixed_asset'"
fi

if [ "$one2m_folder" = "inventory" ]
then
	subclassification_where="subclassification = 'inventory' or subclassification = 'cost_of_goods_sold'"
fi

if [ "$parameter_where" != "$where" ]
then
	where="$subclassification_where and $parameter_where"
else
	where="$subclassification_where"
fi

echo "select $select from $table where $where order by $select;"	|
sql.e

exit 0
