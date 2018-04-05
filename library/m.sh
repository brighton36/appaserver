:
make

if [ "$?" -eq 0 ]
then
	cd ../src_vegetation
	make update_cell_stage
fi
