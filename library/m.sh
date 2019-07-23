:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	touch mysql_remove_null.c
	make mysql_remove_null.e
fi
