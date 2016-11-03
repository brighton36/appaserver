:

cd ../library
make

if [ "$?" -eq 0 ]
then
	cd -
	make output_wet_dry_annual
fi
