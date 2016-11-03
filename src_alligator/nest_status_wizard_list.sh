:

if [ $# -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

table_name=`get_table_name $application nest_status`

echo "	select concat( nest_status_code, ' ', nest_status ), nest_status
	from $table_name
	order by nest_status_code"					|
sql.e

exit 0

