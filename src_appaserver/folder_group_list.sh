:
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 entity application folder_group" 1>&2
	exit 1
fi

get_related_record_list.sh $1 $2 $3 '1:m' | piece.e '|' 0
exit 0
