:
make

if [ "$?" -eq 0 ]
then
	cd ../src_appaserver
	make post_prompt_insert_form
fi
