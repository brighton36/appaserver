:
# utility/fix_orphans.sh
# ----------------------

if [ "$#" != 3 ]
then
	echo "Usage: $0 application max_record_count delete_really_yn" 1>&2
	exit 1
fi

application=$1
max_record_count=$2
delete_really_yn=$3

database=$(echo $application | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
	application=$(echo $application | piece.e ':' 0)
else
	export DATABASE=$application
fi

for folder_name in `echo "select folder from folder;" | sql.e`
do
	#echo "folder_name = $folder_name"

	if [ "$folder_name" = "null" ]
	then
		continue
	fi

	table_name=`get_table_name $application $folder_name`
	record_count=`echo "select count(*) from $table_name;" | sql.e 2>&1`

	results=`strncmp.e "$record_count" ERROR 5`

	if [ "$results" -eq 0 ]
	then
		echo "ERROR: table $table_name does not exist." 1>&2
		continue
	fi

	if [ "$max_record_count" -gt 0 ]
	then
		if [ "$record_count" -ge "$max_record_count" ]
		then
			continue
		fi
	fi

	fix_orphans	$application		\
			fix_orphans		\
			$folder_name		\
			y			\
			$delete_really_yn	\
			stdout
done

exit 0
