:
make

if [ "$?" -eq 0 ]
then
	cd ../src_creel
	make load_guide_angler_submission
fi
