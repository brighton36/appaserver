:
make

if [ "$?" -eq 0 ]
then
	cd ../src_hydrology
	make output_measurement_sets_googlechart
fi
