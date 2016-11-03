:

if [ $# -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

table_name=`get_table_name $application observation_view`

echo "	select observation_view
	from $table_name
	order by observation_view"		|
sql.e

exit 0

