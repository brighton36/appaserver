:
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application one2m_folder" 1>&2
	exit 1
fi

application=$1
one2m_folder=$2

export DATABASE=$application

table="specific_inventory_purchase"

select="inventory_name, concat( serial_number, ' [', retail_price, ']' )"

if [ "$one2m_folder" = "specific_inventory_sale" ]
then
	where="not exists ( select inventory_name, serial_number from specific_inventory_sale where specific_inventory_purchase.inventory_name = specific_inventory_sale.inventory_name and specific_inventory_purchase.serial_number = specific_inventory_sale.serial_number )"
else
	where="1 = 1"
fi


order="inventory_name, serial_number"

echo "select $select from $table where $where order by $order;" | sql.e

exit 0
