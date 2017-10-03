:
make

if [ "$?" -eq 0 ]
then
	cd ../src_waterquality
	make google_map_station_operation
fi
