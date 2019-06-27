:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	make mysqldump_fork.e
fi
