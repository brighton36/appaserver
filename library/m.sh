:
make

if [ "$?" -eq 0 ]
then
	cd ../src_hydrology
	make custom_chart
fi
