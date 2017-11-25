#!/bin/sh
# ---------------------------------------------------------------
# src_autorepair/select_purchase_order_make_vendor_payment.sh
# ---------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

where="ifnull( amount_due, 0 ) > 0"

purchase_order=`get_table_name $application purchase_order`

purchase_order_select="	full_name,
			street_address,
			concat(	purchase_date_time,
				' [Due=',
				amount_due,
				']')"

echo "select ${purchase_order_select}
from ${purchase_order}
where $where
order by purchase_date_time desc;" |
sql.e '^'

exit 0
