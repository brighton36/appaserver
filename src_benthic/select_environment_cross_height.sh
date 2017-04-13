:
echo "$0 $*" 1>&2

if [ "$#" != 5 ]
then
	echo "Usage: $0 application begin_date end_date datatype vegetation_name" 1>&2
	exit 1
fi

application=$1
begin_date=$2
end_date=$3
datatype=$4
vegetation_name=$5

quad_vegetation=$(get_table_name $application quad_vegetation)

execute ()
{
	select="$quad_vegetation.anchor_date,$quad_vegetation.anchor_time,value,avg(vegetation_height_cm)"
	where="$quad_vegetation.anchor_date between '$begin_date' and '$end_date' and datatype = '$datatype' and vegetation_name = '$vegetation_name' and $quad_vegetation.anchor_date = $abiotic_measurement.anchor_date and $quad_vegetation.anchor_time = $abiotic_measurement.anchor_time"
#	order="$quad_vegetation.anchor_date,$quad_vegetation.anchor_time"
	group="$quad_vegetation.anchor_date,$quad_vegetation.anchor_time"

	echo "select $select from $abiotic_measurement,$quad_vegetation where $where group by $group;"			|
	sql.e
}

exists_sampling_point_abiotic=`echo "	select count(*)
					from sampling_point_datatype
					where datatype = '$datatype';" |
				sql.e`

if [ "$exists_sampling_point_abiotic" -eq 1 ]
then
	abiotic_measurement=sampling_point_abiotic_measurement
else
	abiotic_measurement=throwtrap_abiotic_measurement
fi

execute

exit 0
