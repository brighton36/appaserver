#!/bin/sh
# ---------------------------------------------
# station_datatype_stage_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

station_table=`get_table_name $application station`
station_datatype=`get_table_name $application station_datatype`

datatype_and_clause="and $station_datatype.datatype like 'stage%'"

echo "	select $station_table.station, $station_datatype.datatype	\
	from $station_table, $station_datatype				\
	where $station_table.station = $station_datatype.station	\
	$datatype_and_clause						\
	order by $station_table.station, $station_datatype.datatype;"	|
sql '^'									|
cat

exit 0

