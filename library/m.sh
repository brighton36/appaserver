:
make

if [ "$?" -eq 0 ]
then
	cd ../src_waterquality
	make load_turkey_point_file
fi
