:

if [ $# -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

table_name=`get_table_name $application transect`

echo "	select transect_number
	from $table_name
	order by transect_number"		|
sql.e

exit 0

