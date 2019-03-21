#!/bin/sh
# ---------------------------------------------
# station_datatype_list_all.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

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

if [ "$#" -eq 0 ]
then
	echo "Usage: $0 ignored [station]" 1>&2
	exit 1
fi

if [ "$#" -eq 2 ]
then
	station_where="station = '$2'"
else
	station_where="1 = 1"
fi

station_datatype=`get_table_name $application station_datatype`
datatype=`get_table_name $application datatype`

where="${station_datatype}.datatype = ${datatype}.datatype and $station_where"

select="	$station_datatype.station,				\
		$datatype.datatype,					\
		$datatype.units,					\
		$datatype.bar_graph_yn,					\
		$datatype.scale_graph_to_zero_yn,			\
		$datatype.aggregation_sum_yn,				\
		$datatype.ysi_load_heading,				\
		$datatype.exo_load_heading,				\
		$datatype.set_negative_values_to_zero_yn,		\
		$datatype.calibrated_yn"

echo "	select $select							\
	from $datatype,$station_datatype				\
	where $where							\
	order by $station_datatype.station, $datatype.datatype;"	|
sql '|'									|
cat

exit 0
