:
cd ../library
make
if [ "$?" -eq 0 ]
then
	cd -
	make time_ticker.e
fi
