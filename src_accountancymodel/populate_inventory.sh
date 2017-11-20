:
# $APPASERVER_HOME/src_accountancymodel/populate_inventory.sh
# -----------------------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------------------------

echo $0 $* 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application one2m_folder state" 1>&2
	exit 1
fi

application=$1
one2m_folder=$2
state=$3

export DATABASE=$application

table="inventory"

select="concat( inventory_name, ' [', retail_price, ',' , quantity_on_hand, ']' )"

if [ "$one2m_folder" = "inventory_sale" -a "$state" = "insert" ]
then
	where="quantity_on_hand >= 1"
else
	where="1 = 1"
fi

order="inventory_name"

echo "select $select from $table where $where order by $order;" | sql.e

exit 0
