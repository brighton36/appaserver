:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make post_login_cloudacus get_folder_data
fi
