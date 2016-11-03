:
# utility/quick_select_star.sh
# ----------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application folder" 1>&2
	exit 1
fi

application=$1
folder=$2

table_name=$(get_table_name $application folder_attribute)
select_list=$(echo "select attribute from $table_name where folder = '$folder';" | sql.e | sed 's/$/,/')

select_list=$(echo $select_list | sed 's/,$//')

output_file=$folder.dat
echo "#$select_list" > $output_file

get_folder_data application=$application select="$select_list" folder=$folder >> $output_file

echo "Created $output_file"

exit 0
