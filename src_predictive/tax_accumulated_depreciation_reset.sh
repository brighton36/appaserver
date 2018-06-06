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
		from tax_fixed_recovery					\
		where	tax_fixed_recovery.full_name =			\
			fixed_asset_purchase.full_name			\
		  and	tax_fixed_recovery.street_address =		\
			fixed_asset_purchase.street_address		\
		  and	tax_fixed_recovery.purchase_date_time =		\
			fixed_asset_purchase.purchase_date_time		\
		  and	tax_fixed_recovery.asset_name =			\
			fixed_asset_purchase.asset_name			\
		  and	tax_fixed_recovery.serial_number =		\
			fixed_asset_purchase.serial_number )		\
		where disposal_date is null;"				|
sql.e

echo "	update prior_fixed_asset					\
	set finance_accumulated_depreciation = (			\
		select sum( recovery_amount )				\
		from tax_prior_fixed_asset_recovery			\
		where	tax_prior_fixed_asset_recovery.asset_name =	\
			prior_fixed_asset.asset_name			\
		  and	tax_prior_fixed_asset_recovery.serial_number =	\
			prior_fixed_asset.serial_number )		\
		where disposal_date is null;"				|
sql.e

exit 0
