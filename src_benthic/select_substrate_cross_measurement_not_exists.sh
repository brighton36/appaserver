:
echo "$0 $*" 1>&2

if [ "$#" != 7 ]
then
	echo "Usage: $0 application begin_date end_date substrate family genus species" 1>&2
	exit 1
fi

application=$1
begin_date=$2
end_date=$3
substrate=$4
family_name=$5
genus_name=$6
species_name=$7

quad_replicate=$(get_table_name $application quad_replicate)
quad_substrate=$(get_table_name $application quad_substrate)
species_measurement=$(get_table_name $application species_measurement)

select="$species_measurement.anchor_date,$species_measurement.anchor_time,'0',length_millimeters"

inner_query_inside_join="$quad_replicate.anchor_date = $quad_substrate.anchor_date and $quad_replicate.anchor_time = $quad_substrate.anchor_time and $quad_replicate.location = $quad_substrate.location and $quad_replicate.site_number = $quad_substrate.site_number and $quad_replicate.project = $quad_substrate.project and $quad_replicate.quad = $quad_substrate.quad"

inner_query_outside_join="$quad_replicate.anchor_date = $species_measurement.anchor_date and $quad_replicate.anchor_time = $species_measurement.anchor_time and $quad_replicate.location = $species_measurement.location and $quad_replicate.site_number = $species_measurement.site_number and $quad_replicate.project = $species_measurement.project"

inner_query_where="$inner_query_inside_join and closest_to_trap_yn = 'y' and $quad_substrate.substrate = '$substrate' and $inner_query_outside_join"
where="$species_measurement.anchor_date between '$begin_date' and '$end_date' and substrate = '$substrate' and family_name = '$family_name' and genus_name = '$genus_name' and species_name = '$species_name' and $species_measurement.anchor_date = $quad_substrate.anchor_date and $species_measurement.anchor_time = $quad_substrate.anchor_time and $quad_substrate.anchor_date = $quad_replicate.anchor_date and $quad_substrate.anchor_time = $quad_replicate.anchor_time and $quad_substrate.location = $quad_replicate.location and $quad_substrate.site_number = $quad_replicate.site_number and $quad_substrate.quad = $quad_replicate.quad and closest_to_trap_yn = 'y'"

echo "select $select from $species_measurement where not exists (select * from $quad_replicate, $quad_substrate where $inner_query_where) and family_name = '$family_name' and genus_name = '$genus_name' and species_name = '$species_name' and anchor_date between '$begin_date' and '$end_date';"|
sql.e

exit 0
