:
# src_hydrology/datatype_unit_record_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 4 ]
then
	echo "Usage: $0 application station plot_for_station_check_yn aggregate_statistic [datatype]" 1>&2
	exit 1
fi

application=$1
station=$2
plot_for_station_check_yn=$3
aggregate_statistic=$4

datatype=`get_table_name $application datatype`
station_datatype=`get_table_name $application station_datatype`

exclude_conductivity_if_salinity_process="cat"

ignore_volts_process="cat"
ignore_watts_process="cat"

# If single datatype
# ------------------
if [ "$#" -eq 5 -a "$5" != "datatype" ]
then
	datatype_unique_process="grep |$5|"
	chart_datatype_and="and 1 = 1"
else
	datatype_unique_process="cat"

	if [ "$plot_for_station_check_yn" = "" \
	-o   "$plot_for_station_check_yn" = "plot_for_station_check_yn" ]
	then
		chart_datatype_and="and 1 = 1"
		ignore_volts_process="grep -v ^volts"
		ignore_watts_process="grep -v ^watts"
	elif [ "$plot_for_station_check_yn" = "y" ]
	then
		chart_datatype_and="and $station_datatype.plot_for_station_check_yn = 'y'"
	else
		exclude_conductivity_if_salinity_process="exclude_conductivity_if_salinity | 1 2"
		chart_datatype_and="and $station_datatype.validation_required_yn = 'y'"
		ignore_volts_process="grep -v ^volts"
		ignore_watts_process="grep -v ^watts"
	fi
fi

if [ "$aggregate_statistic" = "non_zero_percent" ]
then
	units_select="'percent'"
else
	units_select="$datatype.units"
fi

echo "select 	$units_select,						\
		$station_datatype.datatype,				\
		$station_datatype.station,				\
		$datatype.bar_graph_yn,					\
		$datatype.scale_graph_to_zero_yn,			\
		$datatype.aggregation_sum_yn,				\
		$datatype.set_negative_values_to_zero_yn		\
      from $datatype,$station_datatype 					\
      where ${station_datatype}.station = '$station'			\
        and $datatype.units is not null					\
	$chart_datatype_and						\
	and $station_datatype.datatype = $datatype.datatype		\
	order by units,$station_datatype.datatype;"			|
sql.e '|'								|
$datatype_unique_process						|
$exclude_conductivity_if_salinity_process				|
$ignore_volts_process							|
$ignore_watts_process							|
cat

exit 0
