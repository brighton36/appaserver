:
# $APPASERVER_HOME/src_predictive/populate_inventory.sh
# -----------------------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

echo $0 $* 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 ignored inventory_category" 1>&2
	exit 1
fi

inventory_category=$2

table="inventory"

select="concat( inventory_name, '|', inventory_name, ' [', retail_price, ',' , quantity_on_hand, ']' )"

if [	"${inventory_category}" = "" -o			\
	"${inventory_category}" = "inventory_category" ]
then
	where="1 = 1"
else
	where="inventory_category = '${inventory_category}'"
fi

order="inventory_name"

echo "select $select from $table where $where order by $order;" | sql.e

exit 0
