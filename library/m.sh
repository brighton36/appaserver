:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make delete_folder_row
fi
