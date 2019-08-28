#!/bin/sh
# ---------------------------------------------
# station_datatype_aggregate_average_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

enp_station=`get_table_name $application enp_station`
station_datatype=`get_table_name $application station_datatype`
datatype=`get_table_name $application datatype`

join_where="$enp_station.station = $station_datatype.station and $station_datatype.datatype = $datatype.datatype"

datatype_where="ifnull(aggregation_sum_yn,'n') = 'n'"

echo "	select $enp_station.station, $station_datatype.datatype		\
	from $enp_station, $station_datatype, $datatype			\
	where $join_where and $datatype_where				\
	order by $enp_station.station,$station_datatype.datatype;"	|
sql '^'									|
cat

exit 0

