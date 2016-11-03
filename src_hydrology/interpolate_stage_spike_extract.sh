:
# /opt/html/src/hydrology/interpolate_stage_spike_extract.sh
# ----------------------------------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 station start_date end_date" 1>&2
	exit 1
fi

station=$1
start_date=$2
end_date=$3

echo "select 	station,						\
		datatype, 						\
		measurement_date,					\
		measurement_time, 					\
		measurement_value					\
      from measurement					        	\
      where station = '$station'					\
	and datatype = 'stage'						\
	and measurement_date between '$start_date' and '$end_date'	\
      order by station, datatype, measurement_date, measurement_time;" 	|
mysql.sh ','								|
sed 's/,/|/4'
