:
# src_hydrology/datatype_record_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

# ---------------------------------------------------------------
# This select the same columns as station_datatype_record_list.sh
# ---------------------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

datatype=`get_table_name $application datatype`

echo "select 	$datatype.datatype,					\
		$datatype.units,					\
		$datatype.bar_graph_yn,					\
		$datatype.scale_graph_to_zero_yn,			\
		$datatype.aggregation_sum_yn,				\
		'',							\
		'',							\
		$datatype.set_negative_values_to_zero_yn,		\
		$datatype.calibrated_yn					\
      from $datatype							\
      order by datatype;"						|
sql.e '|'								|
cat

exit 0
