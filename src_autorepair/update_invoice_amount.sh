#!/bin/bash
# ---------------------------------------------
# src_autorepair/update_invoice_amount.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 8 ]
then
       echo "Usage: $0 application date time customer make model trim year" 1>&2
	exit 1
fi

application=$1
order_date=$2
order_time=$3
customer_name=$4
vehicle_make=$5
vehicle_model=$6
vehicle_trim=$7
vehicle_year=$8

customer_order=`get_table_name $application customer_order`
part_order=`get_table_name $application part_order`
service_order=`get_table_name $application service_order`

part_expression="retail_price * quantity - ifnull( discount_amount, 0 )"
service_expression="retail_price - ifnull( discount_amount, 0 )"

part_order_sum=`echo "	select sum( $part_expression )			\
			from $part_order				\
			where order_date = '$order_date'		\
	  		and order_time = '$order_time'			\
	  		and customer_name = '$customer_name'		\
			and vehicle_make = '$vehicle_make'		\
			and vehicle_model = '$vehicle_model'		\
			and vehicle_trim = '$vehicle_trim'		\
			and vehicle_year = $vehicle_year;"		|
		sql.e`

if [ "$part_order_sum" = "" ]
then
	part_order_sum=0
fi

service_order_sum=`echo "select sum( $service_expression )		\
			from $service_order				\
			where order_date = '$order_date'		\
	  		and order_time = '$order_time'			\
	  		and customer_name = '$customer_name'		\
			and vehicle_make = '$vehicle_make'		\
			and vehicle_model = '$vehicle_model'		\
			and vehicle_trim = '$vehicle_trim'		\
			and vehicle_year = $vehicle_year;"		|
		sql.e`

if [ "$service_order_sum" = "" ]
then
	service_order_sum=0
fi

sales_tax=`echo "	select sales_tax				\
			from $customer_order				\
			where order_date = '$order_date'		\
	  		and order_time = '$order_time'			\
	  		and customer_name = '$customer_name'		\
			and vehicle_make = '$vehicle_make'		\
			and vehicle_model = '$vehicle_model'		\
			and vehicle_trim = '$vehicle_trim'		\
			and vehicle_year = $vehicle_year;"		|
		sql.e`

if [ "$sales_tax" = "" ]
then
	sales_tax=0
fi

invoice_amount=`echo "	$part_order_sum +		\
			$service_order_sum +		\
			$sales_tax"			|
		bc -l`

echo "	update $customer_order				\
	set invoice_amount = '$invoice_amount'		\
	where order_date = '$order_date'		\
	and order_time = '$order_time'			\
	and customer_name = '$customer_name'		\
	and vehicle_make = '$vehicle_make'		\
	and vehicle_model = '$vehicle_model'		\
	and vehicle_trim = '$vehicle_trim'		\
	and vehicle_year = $vehicle_year;"		|
sql.e

exit 0
