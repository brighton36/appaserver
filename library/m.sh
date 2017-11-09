:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make drop_column get_folder_data
fi
