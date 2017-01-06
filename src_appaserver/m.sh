:
cd ../library
make
if [ "$?" -eq 0 ]
then
	cd -
	make generic_statistics_report
fi

