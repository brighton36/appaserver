:
# utility/quick_insert_star.sh
# ----------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application folder" 1>&2
	exit 1
fi

application=$1
folder=$2

input_file=$folder.dat
table=$(get_table_name $application $folder)

field_list=$(head -1 $input_file | sed 's/#//')

cat $input_file								|
grep -v '^#'								|
insert_statement.e table=$table field="$field_list" del="^"		|
cat


exit 0
