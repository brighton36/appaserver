:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	touch csvdump_fork.c
	make csvdump_fork.e
fi
