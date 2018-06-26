#!/bin/bash
# -------------------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/finance_accumulated_depreciation_reset.sh
# -------------------------------------------------------------------------

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
		from fixed_asset_depreciation			\
		where	fixed_asset_depreciation.asset_name =	\
			fixed_asset_purchase.asset_name		\
		  and	fixed_asset_depreciation.serial_number =\
			fixed_asset_purchase.serial_number )	\
		where disposal_date is null;"			|
sql.e 2>&1 | grep -v "doesn't exist"

echo "	update prior_fixed_asset					\
	set finance_accumulated_depreciation = (			\
		select sum( depreciation_amount )			\
		from prior_fixed_asset_depreciation			\
		where	prior_fixed_asset_depreciation.asset_name =	\
			prior_fixed_asset.asset_name			\
		  and	prior_fixed_asset_depreciation.serial_number =	\
			prior_fixed_asset.serial_number )		\
		where disposal_date is null;"				|
sql.e 2>&1 | grep -v "doesn't exist"

echo "	update property_purchase					\
	set finance_accumulated_depreciation = (			\
		select sum( depreciation_amount )			\
		from property_depreciation				\
		where	property_depreciation.property_street_address =	\
			property_purchase.property_street_address )	\
		where disposal_date is null;"				|
sql.e 2>&1 | grep -v "doesn't exist"

echo "	update prior_property						     \
	set finance_accumulated_depreciation = (			     \
		select sum( depreciation_amount )			     \
		from prior_property_depreciation			     \
		where	prior_property_depreciation.property_street_address =\
			prior_property.property_street_address )	     \
		where disposal_date is null;"				     |
sql.e 2>&1 | grep -v "doesn't exist"

exit 0
