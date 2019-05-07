:
make

if [ "$?" -eq 0 ]
then
	cd ../src_hydrology
	make exceedance_curve_output
fi
