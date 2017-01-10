:
# src_hydrology/extract_measurements.sh
# -------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 5 ]
then
	echo \
"Usage: $0 entity station datatype begin_date end_date" 1>&2
	exit 1
fi

entity=$1
station=$2
datatype=$3
begin_date=$4
end_date=$5

table_name=`get_table_name $entity measurement`

echo "select 	station,						\
		datatype, 						\
		measurement_date,					\
		measurement_time, 					\
		measurement_value					\
      from $table_name					        	\
      where station = '$station'					\
	and datatype = '$datatype'					\
	and measurement_date between '$begin_date' and '$end_date'	\
      order by station, datatype, measurement_date, measurement_time;" 	|
sql.e ','								|
cat
