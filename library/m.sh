:
make

if [ "$?" -eq 0 ]
then
	cd ../utility
	touch binary_count.c
	make binary_count.e
	touch keys_match_sum.c
	make keys_match_sum.e
fi
