:
make

if [ "$?" -eq 0 ]
then
	cd ../src_hydrology
	make generic_measurement_load
fi
