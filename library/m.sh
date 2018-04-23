:
make

if [ "$?" -eq 0 ]
then
	cd ../src_waterquality
	make load_sfwmd_file
fi
