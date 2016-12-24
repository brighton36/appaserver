:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	make make_checks.e
fi
