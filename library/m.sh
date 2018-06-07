:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	make date_ticker.e date.e
fi
