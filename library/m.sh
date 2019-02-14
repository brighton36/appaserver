:
make

if [ "$?" -eq 0 ]
then
	cd ../src_waterquality
	touch output_results_exception.c
	make output_results_exception
fi
