:
# -----------------------------------------------------------------
# $APPASERVER_HOME/src_accountancymodel/depreciate_entity_select.sh
# -----------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 ignored fund" 1>&2
	exit 1
fi

fund=$2

folder_attribute_exists.sh $application purchase_order fund
results=$?

if [ "$results" -eq 0 ]
then
	# fund does exist
	# ---------------
	fund_where="fund = '$fund'"
else
	# fund doesn't exist
	# ------------------
	fund_where="1 = 1"
fi

select="fixed_asset_purchase.full_name,fixed_asset_purchase.street_address"

join_where="purchase_order.full_name = fixed_asset_purchase.full_name and purchase_order.street_address = fixed_asset_purchase.street_address and purchase_order.purchase_date_time = fixed_asset_purchase.purchase_date_time"

where="ifnull(extension,0) <> ifnull(accumulated_depreciation,0) and disposal_date is null and $join_where and $fund_where"

folder="purchase_order,fixed_asset_purchase"

echo "select $select from $folder where $where;"		|
sql.e								|
sort -u

exit 0
