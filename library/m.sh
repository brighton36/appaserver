:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make output_prompt_process_form
fi

