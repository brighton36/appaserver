#!/bin/bash
# ----------------------------------------------------------
# $APPASERVER_HOME/src_accountancymodel/depreciate_select.sh
# ----------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

folder_attribute_exists.sh $application purchase_order fund
results=$?

if [ "$results" -eq 0 ]
then
	# fund does exist
	# ---------------
	fund_select="fund"
else
	# fund doesn't exist
	# ------------------
	fund_select="''"
fi

select="fixed_asset_purchase.full_name,fixed_asset_purchase.street_address,${fund_select},fixed_asset_purchase.purchase_date_time,asset_name,serial_number,estimated_useful_life_years,estimated_useful_life_units,estimated_residual_value,declining_balance_n,depreciation_method,accumulated_depreciation,arrived_date_time,extension"

from="purchase_order,fixed_asset_purchase"

join_where="purchase_order.full_name = fixed_asset_purchase.full_name and purchase_order.street_address = fixed_asset_purchase.street_address and purchase_order.purchase_date_time = fixed_asset_purchase.purchase_date_time"

where="ifnull(extension,0) <> ifnull(accumulated_depreciation,0) and disposal_date is null and $join_where"

echo "select $select from $from where $where;"			|
sql.e								|
sort

exit 0
