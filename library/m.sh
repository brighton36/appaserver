:
make

if [ "$?" -eq 0 ]
then
	cd ../src_capitolpops
	make donor_receipt
fi
