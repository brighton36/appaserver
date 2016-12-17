#!/bin/sh
# ---------------------------------------------
# src_autorepair/select_invoice_lineitems.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 4 ]
then
	echo "Usage: $0 application full_name street_address sale_date_time" 1>&2
	exit 1
fi

application=$1
full_name=$2
street_address=$3
sale_date_time=$4

inventory_sale=`get_table_name $application inventory_sale`
service_sale=`get_table_name $application service_sale`

inventory_sale_select="inventory_name,quantity,retail_price,discount_amount"
service_sale_select="service_name,'1',retail_price,discount_amount"

echo "select ${inventory_sale_select}					\
from ${inventory_sale}							\
where full_name = '$full_name'					\
and street_address = '$street_address'					\
and sale_date_time = '$sale_date_time';" 				|
sql.e '^'

echo "select ${service_sale_select}					\
from ${service_sale}							\
where full_name = '$full_name'					\
and street_address = '$street_address'					\
and sale_date_time = '$sale_date_time';" 				|
sql.e '^'

exit 0
