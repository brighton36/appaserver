:
make

if [ "$?" -eq 0 ]
then
	cd ../src_hydrology
	make output_current_vs_historical output_measurement_googlecharts
fi
