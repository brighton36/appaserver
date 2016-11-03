#!/bin/sh
# ---------------------------------------------
# src_autorepair/select_customer_sale.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 application state [open|closed|all]" 1>&2
	exit 1
fi

application=$1
state=$2

if [ "$state" = "insert" ]
then
	third_parameter="open"
else
	third_parameter=$3
fi

if [ "$third_parameter" = "open" ]
then
	where="completed_date_time is null"
elif [ "$third_parameter" = "closed" ]
then
	where="completed_date_time is not null"
else
	where="1 = 1"
fi

customer_sale=`get_table_name $application customer_sale`

#customer_sale_select="entity_name,street_address, concat( sale_date_time, '[', vehicle_year, ',', vehicle_make, ',', vehicle_model, ',', vehicle_trim, ',', ifnull( invoice_amount, 0.00 ), ']')"

customer_sale_select="entity_name,street_address, concat( sale_date_time, '[', vehicle_make, ',', vehicle_model, ',', vehicle_trim, ',', vehicle_year, ',Due=', ifnull( amount_due, 0 ), ']')"

echo "select ${customer_sale_select}					\
from ${customer_sale}							\
where $where								\
order by sale_date_time desc;"						|
sql.e '^'

exit 0
