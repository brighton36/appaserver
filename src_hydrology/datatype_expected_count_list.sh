:
# src_everglades/datatype_expected_count_list.sh
# ----------------------------------------------
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 station datatype delimiter" 1>&2
	exit 1
fi

station=$1
datatype=$2
delimiter=$3

select="begin_measurement_date,begin_measurement_time,expected_count"

echo "	select	$select						\
	from data_collection_frequency				\
	where station = '$station'				\
	  and datatype = '$datatype'				\
	order by $select;"					|
sql "$delimiter"						|
cat

exit 0
