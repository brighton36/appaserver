:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	make trim_length.e
fi
