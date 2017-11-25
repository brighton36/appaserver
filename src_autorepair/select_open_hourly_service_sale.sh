#!/bin/sh
# -------------------------------------------------
# src_autorepair/select_open_hourly_service_sale.sh
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

join_where="	hourly_service_sale.full_name =
			customer_sale.full_name and
		hourly_service_sale.street_address =
			customer_sale.street_address and
		hourly_service_sale.sale_date_time =
			customer_sale.sale_date_time"
		
where="completed_date_time is null and $join_where"

select="	hourly_service_sale.full_name,
		hourly_service_sale.street_address,
		hourly_service_sale.sale_date_time,
		service_name,
		concat(	description,
			' [',
			mechanic_full_name,
			',',
			vehicle_make,
			',',
			vehicle_model,
			',',
			vehicle_trim,
			',',
			vehicle_year,
			']' )"

from="customer_sale, hourly_service_sale"

echo "	select ${select}
	from ${from}
	where $where
	order by hourly_service_sale.sale_date_time desc;" |
sql.e '^' $application

exit 0
