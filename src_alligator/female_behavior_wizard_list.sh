:

if [ $# -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

table_name=`get_table_name $application female_behavior`

echo "	select	concat( female_behavior_code, ' ', female_behavior ),
     		female_behavior
	from $table_name
	order by female_behavior_code"		|
sql.e

exit 0

