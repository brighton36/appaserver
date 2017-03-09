:
make

if [ "$?" -eq 0 ]
then
	cd ../src_hydrology
	make load_ysi_data
fi
