:
make

if [ "$?" -eq 0 ]
then
	cd ../src_predictive
	make load_bank_spreadsheet
fi
