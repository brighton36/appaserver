:
make

if [ "$?" -eq 0 ]
then
	cd ../src_predictive
	make post_change_activity_work
fi
