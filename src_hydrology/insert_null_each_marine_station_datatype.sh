:
# /opt/html/src_hydrology/insert_null_each_marine_station_datatype.sh
# -------------------------------------------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 entity begin_date end_date" 1>&1
	exit 1
fi

entity=$1
begin_date=$2
end_date=$3

station_datatype=`get_table_name $entity station_datatype`
station=`get_table_name $entity station`

for station_datatype in `						\
	echo "	select $station.station, $station_datatype.datatype	\
		from $station,$station_datatype				\
		where $station.station = $station_datatype.station	\
		  and $station.station_type = 'marine'			\
		order by $station.station, $station_datatype.datatype;"	|
	mysql.sh '|' 							|
	cat`
do
	station=`echo $station_datatype | piece.e '|' 0`
	datatype=`echo $station_datatype | piece.e '|' 1`
	measurement_null_value_insert	$entity 			\
					$station 			\
					$datatype 			\
					$begin_date 			\
					0000 				\
					$end_date 			\
					2359 				\
					n 				\
					y
done

exit 0
