:
cd ../library
make
if [ "$?" -eq 0 ]
then
	cd -
	make output_prompt_insert_form
fi

