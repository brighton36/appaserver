#!/bin/bash
# ---------------------------------
# station_utm_stage_list.sh
# ---------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application measurement_date" 1>&2
	exit 1
fi

application=$1
measurement_date=$2

DATABASE=$application

function select_measurement ()
{
	station=$1

	select="station.station,utm_x,utm_y,measurement_value"

	join="station.station = measurement.station"
	not_null="utm_x is not null and utm_y is not null"
	measurement_where="measurement_date = '$measurement_date'"
	datatype_where="datatype = 'stage'"
	station_where="station.station = '$station'"

	where="	$join and
		$not_null and
		$measurement_where and
		$datatype_where and
		$station_where"

	echo "	select $select
		from station,measurement
		where $where
		order by measurement_time;"	|
	sql.e $DATABASE				|
	tail -1
}

station_list_all.sh $application		|
while read station
do
	select_measurement "$station"
done

exit 0
