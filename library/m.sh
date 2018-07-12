:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	touch output_edit_table_form.c
	make output_edit_table_form
fi
