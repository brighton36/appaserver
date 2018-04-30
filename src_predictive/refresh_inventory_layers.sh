#!/bin/bash
# -----------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/refresh_inventory_layers.sh
# -----------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

if [ "$#" -eq 0 ]
then
	echo "Usage: $0 ignored [inventory]" 1>&2
	exit 1
fi

if [ "$#" -eq 2 ]
then
	inventory=$2
	propagate_inventory_sale_layers $application '' '' '' "$inventory" '' n
	exit 0
fi

echo "select inventory_name from inventory;"		|
sql.e							|
while read inventory
do
	propagate_inventory_sale_layers $application '' '' '' "$inventory" '' n
done

exit $?
