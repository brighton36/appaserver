#!/bin/bash
# -----------------------------------------------------------------------
# $APPASERVER_HOME/src_accountancymodel/accumulated_depreciation_reset.sh
# -----------------------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)
database=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
else
	export DATABASE=$application
fi

echo "	update fixed_asset_purchase				\
	set accumulated_depreciation = (			\
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

exit 0
