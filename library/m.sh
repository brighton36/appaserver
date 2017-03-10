:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	touch menu.c
	make menu.e
fi
