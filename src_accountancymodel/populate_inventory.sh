:
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application one2m_folder" 1>&2
	exit 1
fi

application=$1
one2m_folder=$2

export DATABASE=$application

table="inventory"

select="concat( inventory_name, ' [', retail_price, ']' )"

if [ "$one2m_folder" = "inventory_sale" ]
then
	where="quantity_on_hand >= 1"
else
	where="1 = 1"
fi

order="inventory_name"

echo "select $select from $table where $where order by $order;" | sql.e

exit 0
