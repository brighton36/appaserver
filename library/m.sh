:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make post_edit_table_form
fi
