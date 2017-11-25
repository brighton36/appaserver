#!/bin/sh
# -------------------------------------------------
# src_autorepair/select_open_fixed_service_work.sh
# -------------------------------------------------
#
# Freely available software: see Appaserver.org
# -------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application login_name" 1>&2
	exit 1
fi

application=$1
login_name=$2

join_where="	fixed_service_sale.full_name =
			customer_sale.full_name and
		fixed_service_sale.street_address =
			customer_sale.street_address and
		fixed_service_sale.sale_date_time =
			customer_sale.sale_date_time and
		fixed_service_work.full_name =
			fixed_service_sale.full_name and
		fixed_service_work.street_address =
			fixed_service_sale.street_address and
		fixed_service_work.sale_date_time =
			fixed_service_sale.sale_date_time and
		fixed_service_work.service_name =
			fixed_service_sale.service_name"

where="	completed_date_time is null and
	end_work_date_time is null and
	login_name = '$login_name' and
	$join_where"

select="	fixed_service_sale.full_name,
		fixed_service_sale.street_address,
		fixed_service_sale.sale_date_time,
		fixed_service_sale.service_name,
		concat( substr( begin_work_date_time, 1, 16 ),
			' [',
			vehicle_make,
			',',
			vehicle_model,
			',',
			vehicle_trim,
			',',
			vehicle_year,
			']' )"

from="customer_sale,fixed_service_sale,fixed_service_work"

echo "	select ${select}
	from ${from}
	where $where
	order by fixed_service_work.begin_work_date_time desc;" |
sql.e '^' $application

exit 0
