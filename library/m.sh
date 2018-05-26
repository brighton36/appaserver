:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make upgrade_appaserver_database
fi
