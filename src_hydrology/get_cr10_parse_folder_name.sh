:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1
table_name=`get_table_name $application application_constants`
echo "	select application_constant_value			\
	from $table_name					\
	where application_constant = 'cr10_parse_folder_name';" |
sql.e

exit 0

