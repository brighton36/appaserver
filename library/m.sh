:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	touch mysqldump_fork_count_drop.c
	make mysqldump_fork_count_drop.e
fi
