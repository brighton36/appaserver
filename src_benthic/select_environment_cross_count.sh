:
if [ "$#" != 7 ]
then
	echo "Usage: $0 application begin_date end_date datatype family genus species" 1>&2
	exit 1
fi

application=$1
begin_date=$2
end_date=$3
datatype=$4
family_name=$5
genus_name=$6
species_name=$7

species_count=$(get_table_name $application species_count)

execute ()
{
	select="$species_count.anchor_date,$species_count.anchor_time,value,sum(count_per_square_meter)"
	where="$species_count.anchor_date between '$begin_date' and '$end_date' and datatype = '$datatype' and family_name = '$family_name' and genus_name = '$genus_name' and species_name = '$species_name' and $species_count.anchor_date = $abiotic_measurement.anchor_date and $species_count.anchor_time = $abiotic_measurement.anchor_time"
	group="$species_count.anchor_date,$species_count.anchor_time"
	order="value"

	echo "select $select from $abiotic_measurement,$species_count where $where group by $group order by $order;"		|
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
