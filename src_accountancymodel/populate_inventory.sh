:
# $APPASERVER_HOME/src_accountancymodel/populate_inventory.sh
# -----------------------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------------------------

echo $0 $* 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application inventory_category" 1>&2
	exit 1
fi

application=$1
inventory_category=$2

export DATABASE=$application

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
