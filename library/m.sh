:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make output_edit_table_form post_choose_isa_drop_down
fi
