:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make execute_select_statement
fi
