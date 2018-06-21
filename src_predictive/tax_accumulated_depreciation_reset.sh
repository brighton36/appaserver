#!/bin/bash
# ---------------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/tax_accumulated_depreciation_reset.sh
# ---------------------------------------------------------------------

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

echo "	update fixed_asset_purchase					\
	set tax_accumulated_depreciation = (				\
		select sum( recovery_amount )				\
		from tax_fixed_asset_recovery				\
		where	tax_fixed_asset_recovery.asset_name =		\
			fixed_asset_purchase.asset_name			\
		  and	tax_fixed_asset_recovery.serial_number =	\
			fixed_asset_purchase.serial_number )		\
		where disposal_date is null;"				|
sql.e

echo "	update prior_fixed_asset					\
	set tax_accumulated_depreciation = (				\
		select sum( recovery_amount )				\
		from tax_prior_fixed_asset_recovery			\
		where	tax_prior_fixed_asset_recovery.asset_name =	\
			prior_fixed_asset.asset_name			\
		  and	tax_prior_fixed_asset_recovery.serial_number =	\
			prior_fixed_asset.serial_number )		\
		where disposal_date is null;"				|
sql.e

echo "	update property_purchase					\
	set tax_accumulated_depreciation = (				\
		select sum( recovery_amount )				\
		from tax_property_recovery				\
		where	tax_property_recovery.property_street_address =	\
			property_purchase.property_street_address )	\
		where disposal_date is null;"				|
sql.e

echo "	update prior_property						\
	set tax_accumulated_depreciation = (				\
		select sum( recovery_amount )				\
		from tax_prior_property_recovery			\
		where							\
		  tax_prior_property_recovery.property_street_address =	\
			prior_property.property_street_address )	\
		where disposal_date is null;"				|
sql.e

exit 0
