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

station_utm_file="/tmp/station_utm.dat"
station_stage_file="/tmp/station_stage.dat"

`get_appaserver_home.e`/src_$application/station_utm_list.sh	\
	$application						|
sort								|
cat > $station_utm_file

`get_appaserver_home.e`/src_$application/station_stage_list.sh	\
	$application $measurement_date				|
sort								|
cat > $station_stage_file

join -t'^' $station_utm_file $station_stage_file 2>/dev/null

exit 0
