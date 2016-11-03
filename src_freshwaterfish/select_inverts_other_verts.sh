:
# src_freshwaterfish/select_inverts_other_verts.sh
# ------------------------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application begin_date end_date" 1>&2
	exit 1
fi

application=$1
begin_date=$2
end_date=$3

measurement=`get_table_name $application measurement`
species=`get_table_name $application species`
site_visit=`get_table_name $application site_visit`

species_category="species_category in ('other_vertebrate','other_invertebrate')"

echo "	select $site_visit.visit_date,$site_visit.site,$site_visit.plot,throw_number,$measurement.scientific_name,count(*)
	from $measurement,$species,$site_visit
	where	$measurement.scientific_name =
		$species.scientific_name
	  and	$measurement.site = $site_visit.site
	  and	$measurement.plot = $site_visit.plot
	  and	$measurement.region = $site_visit.region
	  and	$measurement.visit_date = $site_visit.visit_date
	  and	$site_visit.agency = 'national_park_service'
	  and	$species_category
	  and	$site_visit.visit_date between '$begin_date' and '$end_date'
	group by $site_visit.visit_date,$site_visit.site,$site_visit.plot,throw_number,$measurement.scientific_name;" |
sql.e

exit 0
