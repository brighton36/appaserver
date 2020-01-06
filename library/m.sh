:
make

if [ "$?" -eq 0 ]
then
	cd ../src_camp
	make generate_invoice
fi

