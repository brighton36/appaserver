:
# src_everglades/expected_count_list.sh
# -------------------------------------
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 station delimiter" 1>&2
	exit 1
fi

station=$1
delimiter=$2

echo "	select	expected_count,					\
		datatype,					\
		begin_measurement_date,				\
		begin_measurement_time				\
	from data_collection_frequency				\
	where station = '$station'				\
	order by	datatype,				\
			begin_measurement_date;"		|
sql "$delimiter"						|
cat

exit 0
