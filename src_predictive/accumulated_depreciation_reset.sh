#!/bin/bash
# -----------------------------------------------------------------------
# $APPASERVER_HOME/src_accountancymodel/accumulated_depreciation_reset.sh
# -----------------------------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

echo "	update fixed_asset_purchase				\
	set finance_accumulated_depreciation = (		\
		select sum( depreciation_amount )		\
		from depreciation				\
		where	depreciation.full_name =		\
			fixed_asset_purchase.full_name		\
		  and	depreciation.street_address =		\
			fixed_asset_purchase.street_address	\
		  and	depreciation.purchase_date_time =	\
			fixed_asset_purchase.purchase_date_time	\
		  and	depreciation.asset_name =		\
			fixed_asset_purchase.asset_name		\
		  and	depreciation.serial_number =		\
			fixed_asset_purchase.serial_number )	\
		where disposal_date is null;"			|
sql.e

echo "	update prior_fixed_asset					\
	set finance_accumulated_depreciation = (			\
		select sum( depreciation_amount )			\
		from prior_fixed_asset_depreciation			\
		where	prior_fixed_asset_depreciation.asset_name =	\
			prior_fixed_asset.asset_name			\
		  and	prior_fixed_asset_depreciation.serial_number =	\
			prior_fixed_asset.serial_number )		\
		where disposal_date is null;"				|
sql.e

exit 0
