#!/bin/bash
# -----------------------------------------------------------------
# $APPASERVER_HOME/src_accountancymodel/refresh_inventory_layers.sh
# -----------------------------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application inventory" 1>&2
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

inventory=$2

propagate_inventory_sale_layers $application '' '' '' "$inventory" '' n

exit $?
