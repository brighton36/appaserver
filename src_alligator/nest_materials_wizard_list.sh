:

if [ $# -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

table_name=`get_table_name $application materials`

echo "	select concat( materials_code, ' ', materials ), materials
	from $table_name
	order by materials_code"					|
sql.e

exit 0

