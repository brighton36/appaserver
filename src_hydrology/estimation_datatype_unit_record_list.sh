:
# estimation_datatype_unit_record_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -lt 3 ]
then
	echo "Usage: $0 application station datatype" 1>&2
	exit 1
fi

application=$1
station=$2
datatype=$3

datatype_table=`get_table_name $application datatype`
station_datatype=`get_table_name $application station_datatype`

# Note: selecting 'n' for aggregation_sum_yn and 'n' to scale_graph_to_zero_yn
# ----------------------------------------------------------------------------
echo "select 	$datatype_table.units,					\
		$station_datatype.datatype,				\
		$station_datatype.station,				\
		'n',							\
		'n'							\
      from $datatype_table,$station_datatype 				\
      where $station_datatype.station = '$station'			\
        and $datatype_table.units is not null				\
	and $station_datatype.datatype = $datatype_table.datatype	\
	and $datatype_table.datatype = '$datatype'			\
	order by units,$station_datatype.datatype;"			|
sql.e '|'

# Output "new_value" datatype
# ---------------------------
echo "select 	$datatype_table.units,					\
		'new_value',						\
		$station_datatype.station,				\
		'n',							\
		'n'							\
      from $datatype_table,$station_datatype 				\
      where $station_datatype.station = '$station'			\
        and $datatype_table.units is not null				\
	and $station_datatype.datatype = $datatype_table.datatype	\
	and $datatype_table.datatype = '$datatype'			\
	order by units;"						|
sql.e '|'

exit 0
