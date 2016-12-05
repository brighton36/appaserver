:
make

if [ "$?" -eq 0 ]
then
	cd ../src_capitolpops
	make post_change_dues_payment
fi
