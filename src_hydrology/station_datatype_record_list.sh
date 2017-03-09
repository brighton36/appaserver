:
# src_hydrology/station_datatype_record_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

# -------------------------------------------------------
# This select the same columns as datatype_record_list.sh
# -------------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application station" 1>&2
	exit 1
fi

application=$1
station=$2

datatype=`get_table_name $application datatype`
station_datatype=`get_table_name $application station_datatype`

join_where="${station_datatype}.datatype = ${datatype}.datatype"
where="${join_where} and ${station_datatype}.station = '$station'"

echo "select 	$datatype.datatype,					\
		$datatype.bar_graph_yn,					\
		$datatype.scale_graph_to_zero_yn,			\
		$datatype.aggregation_sum_yn,				\
		$datatype.ysi_load_heading,				\
		$datatype.exo_load_heading,				\
		$datatype.set_negative_values_to_zero_yn		\
      from $datatype,${station_datatype}				\
      where $where							\
      order by datatype;"						|
sql.e '|'								|
cat

exit 0
