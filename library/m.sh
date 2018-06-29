:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	touch date_convert.c && make date_convert.e
fi
