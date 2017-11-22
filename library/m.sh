:
make

if [ "$?" -eq 0 ]
then
	cd ../src_autorepair
	make generate_invoice
fi
