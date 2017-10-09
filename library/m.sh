:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	make minutes_ticker.e
fi
