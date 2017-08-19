:
make

if [ "$?" -eq 0 ]
then
	cd ../src_accountancymodel
	make post_change_inventory_sale
fi
