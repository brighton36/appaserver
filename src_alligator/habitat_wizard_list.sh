:

if [ $# -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

table_name=`get_table_name $application habitat`

echo "	select concat( habitat_code, ' ', habitat ), habitat
	from $table_name
	order by habitat_code"			|
sql.e						|
grep "^[0-9]"					|
sort -n

exit 0

