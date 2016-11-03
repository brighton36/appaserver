:
if [ "$#" != 8 ]
then
	echo "Usage: $0 application begin_date end_date vegetation_name family genus species total_indicator" 1>&2
	exit 1
fi

application=$1
begin_date=$2
end_date=$3
vegetation_name=$4
family_name=$5
genus_name=$6
species_name=$7
total_indicator=$8

quad_replicate=$(get_table_name $application quad_replicate)

if [ "$total_indicator" = "total" ]
then
	quad_vegetation=$(get_table_name $application quad_vegetation_total)
else
	quad_vegetation=$(get_table_name $application quad_vegetation)
fi

species_measurement=$(get_table_name $application species_measurement)
abundance_rating=$(get_table_name $application abundance_rating)

select="$species_measurement.anchor_date,$species_measurement.anchor_time,avg(abundance_rating_code),avg(length_millimeters)"

where="$species_measurement.anchor_date between '$begin_date' and '$end_date' and vegetation_name = '$vegetation_name' and family_name = '$family_name' and genus_name = '$genus_name' and species_name = '$species_name' and $species_measurement.anchor_date = $quad_vegetation.anchor_date and $species_measurement.anchor_time = $quad_vegetation.anchor_time and $quad_vegetation.anchor_date = $quad_replicate.anchor_date and $quad_vegetation.anchor_time = $quad_replicate.anchor_time and $quad_vegetation.location = $quad_replicate.location and $quad_vegetation.site_number = $quad_replicate.site_number and $quad_vegetation.quad = $quad_replicate.quad and $quad_vegetation.abundance_rating = $abundance_rating.abundance_rating and closest_to_trap_yn = 'y' and $quad_vegetation.abundance_rating is not null"

group="$species_measurement.anchor_date,$species_measurement.anchor_time"
#order="abundance_rating_code"

echo "select $select from $quad_replicate,$quad_vegetation,$species_measurement,$abundance_rating where $where group by $group;"		|
sql.e

exit 0
