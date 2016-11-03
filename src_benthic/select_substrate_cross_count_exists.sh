:
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
species_count=$(get_table_name $application species_count)

select="$species_count.anchor_date,$species_count.anchor_time,'1',sum(count_per_square_meter)"

where="$species_count.anchor_date between '$begin_date' and '$end_date' and substrate = '$substrate' and family_name = '$family_name' and genus_name = '$genus_name' and species_name = '$species_name' and $species_count.anchor_date = $quad_substrate.anchor_date and $species_count.anchor_time = $quad_substrate.anchor_time and $quad_substrate.anchor_date = $quad_replicate.anchor_date and $quad_substrate.anchor_time = $quad_replicate.anchor_time and $quad_substrate.location = $quad_replicate.location and $quad_substrate.site_number = $quad_replicate.site_number and $quad_substrate.quad = $quad_replicate.quad and closest_to_trap_yn = 'y'"

group="$species_count.anchor_date,$species_count.anchor_time"
order="$species_count.anchor_date,$species_count.anchor_time"

echo "select $select from $quad_replicate,$quad_substrate,$species_count where $where group by $group order by $order;"		|
sql.e

exit 0
