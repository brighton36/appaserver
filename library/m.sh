:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make generic_statistics_report
fi
