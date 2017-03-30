:
make

if [ "$?" -eq 0 ]
then
	cd ../src_hydrology
	make post_generic_measurement_load
fi
