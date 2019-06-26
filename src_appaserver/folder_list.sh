:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 entity" 1>&2
	exit 1
fi

get_folder_data application=$1 folder=folder select=folder	|
grep -v '^null$'						|
cat

exit 0
