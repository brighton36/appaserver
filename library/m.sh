:
make

if [ "$?" -eq 0 ]
then
	cd ../src_waterquality
	touch output_results_exception_free.c
	make output_results_exception_free
fi
