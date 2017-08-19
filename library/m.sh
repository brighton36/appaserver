:
make

if [ "$?" -eq 0 ]
then
	cd ../src_accountancymodel
	make post_change_inventory_sale propagate_inventory_sale_layers
fi
