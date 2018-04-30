:

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must .set_project first." 1>&2
	exit 1
fi

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 ignored" 1>&2
	exit 1
fi

application=$1

application=$APPASERVER_DATABASE

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
