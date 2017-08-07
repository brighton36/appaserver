:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	make pad_missing_times.e
fi
