:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application[:database]" 1>&2
	exit 1
fi

application=$1

database=$(echo $application | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
	application=$(echo $application | piece.e ':' 0)
fi

for folder in `folder_list.sh $application`
do

	if [ "$folder" = "null" ]
	then
		continue
	fi

	table=$(get_table_name $application $folder)

	timestamp_row=$(echo "describe $table;" | sql.e | grep timestamp)

	if [ "$timestamp_row" == "" ]
	then
		echo "alter table $table add column delta_time timestamp;"
	fi
done

exit 0
