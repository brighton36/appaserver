:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	touch sql.c
	make sql.e
fi
