:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	make keys_match_sum.e
fi
