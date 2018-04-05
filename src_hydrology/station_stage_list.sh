#!/bin/bash
# ---------------------------------
# station_stage_list.sh
# ---------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application measurement_date" 1>&2
	exit 1
fi

application=$1
measurement_date=$2

function select_measurement ()
{
	station=$1

	select="station.station,measurement_value"

	join="station.station = measurement.station"
	measurement_where="measurement_date = '$measurement_date'"
	datatype_where="datatype = 'stage'"
	station_where="station.station = '$station'"

	where="	$join and
		$measurement_where and
		$datatype_where and
		$station_where"

	echo "	select $select
		from station,measurement
		where $where
		order by measurement_time;"	|
	`get_appaserver_home.e`/src_appaserver/sql.e $application	  |
	tail -1					|
	cat
}

`get_appaserver_home.e`/src_$application/station_list_all.sh $application |
while read station
do
	select_measurement "$station"
done

exit 0
