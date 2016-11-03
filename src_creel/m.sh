:
#cd $APPASERVER_HOME/library
#make

if [ "$?" -eq 0 ]
then
	cd -
	make output_measurement_spreadsheet_file
fi

exit 0
