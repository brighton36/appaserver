:

if [ $# -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

table_name=`get_table_name $application basin`

echo "	select concat( basin_code, ' ', basin ),basin
	from $table_name
	order by basin_code"				|
sql.e

exit 0

