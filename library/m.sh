:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	touch insert_statement.c
	make insert_statement.e
fi
