#!/bin/sh
# ---------------------------------------------
# datatype_bar_graph_4station_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 application station" 1>&2
	exit 1
fi

application=$1
station=$2

datatype=`get_table_name $application datatype`
station_datatype=`get_table_name $application station_datatype`

station_and_clause="$station_datatype.station = '$station'"
join_clause="$station_datatype.datatype = $datatype.datatype"

echo "		select $datatype.datatype, bar_graph_yn			\
		from 	$station_datatype,				\
			$datatype					\
		where  $join_clause					\
		  and $station_and_clause				\
		order by $station_datatype.datatype;"			|
sql.e									|
cat
