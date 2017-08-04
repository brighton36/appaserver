#!/bin/sh
# ----------------------------------------------------------------
# src_autorepair/select_customer_sale_collect_customer_payment.sh
# ----------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# ----------------------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

where="ifnull( amount_due, 0 ) > 0 and uncollectible_writeoff_date is null"

customer_sale=`get_table_name $application customer_sale`

customer_sale_select="full_name,street_address, concat( sale_date_time, '[', vehicle_make, ',', vehicle_model, ',', vehicle_trim, ',', vehicle_year, ',Due=', ifnull( amount_due, 0 ), ']')"

echo "select ${customer_sale_select}					\
from ${customer_sale}							\
where $where								\
order by sale_date_time desc;"						|
sql.e '^'

exit 0
