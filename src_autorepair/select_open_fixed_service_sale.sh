#!/bin/sh
# -------------------------------------------------
# src_autorepair/select_open_fixed_service_sale.sh
# -------------------------------------------------
#
# Freely available software: see Appaserver.org
# -------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

join_where="	fixed_service_sale.full_name =
			customer_sale.full_name and
		fixed_service_sale.street_address =
			customer_sale.street_address and
		fixed_service_sale.sale_date_time =
			customer_sale.sale_date_time"
		
where="completed_date_time is null and $join_where"

select="	fixed_service_sale.full_name,
		fixed_service_sale.street_address,
		fixed_service_sale.sale_date_time,
		concat(	service_name,
			' [',
			vehicle_make,
			',',
			vehicle_model,
			',',
			vehicle_trim,
			',',
			vehicle_year,
			']' )"

from="customer_sale, fixed_service_sale"

echo "	select ${select}
	from ${from}
	where $where
	order by fixed_service_sale.sale_date_time desc;" |
sql.e '^' $application

exit 0
