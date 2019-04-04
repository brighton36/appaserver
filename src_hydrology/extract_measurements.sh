:
# src_hydrology/extract_measurements.sh
# -------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 7 ]
then
	echo \
"Usage: $0 entity station datatype begin_date begin_time end_date end_time" 1>&2
	exit 1
fi

entity=$1
station=$2
datatype=$3
begin_date=$4
begin_time=$5
end_date=$6
end_time=$7

table_name=`get_table_name $entity measurement`

date_where=`	date_time_between_where.sh	$begin_date		\
						$begin_time		\
						$end_date		\
						$end_time`

echo "select 	station,						\
		datatype, 						\
		measurement_date,					\
		measurement_time, 					\
		measurement_value					\
      from $table_name					        	\
      where station = '$station'					\
	and datatype = '$datatype'					\
	and ${date_where}						\
      order by station, datatype, measurement_date, measurement_time;" 	|
sql.e ','								|
cat
