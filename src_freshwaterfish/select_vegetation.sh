:
# src_freshwaterfish/select_vegetation.sh
# ---------------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application begin_date end_date" 1>&2
	exit 1
fi

application=$1
begin_date=$2
end_date=$3

surface_vegetation=`get_table_name $application surface_vegetation`
submergent_vegetation=`get_table_name $application submergent_vegetation`
species=`get_table_name $application species`
site_visit=`get_table_name $application site_visit`

species_category="species_category in ('surface_vegetation')"
echo "	select $site_visit.visit_date,$site_visit.site,$site_visit.plot,throw_number,$surface_vegetation.scientific_name,sum(density_alive_count)
	from $surface_vegetation,$species,$site_visit
	where	$surface_vegetation.scientific_name =
		$species.scientific_name
	  and	$surface_vegetation.site = $site_visit.site
	  and	$surface_vegetation.plot = $site_visit.plot
	  and	$surface_vegetation.region = $site_visit.region
	  and	$site_visit.agency = 'national_park_service'
	  and	$species_category
	  and	$site_visit.visit_date between '$begin_date' and '$end_date'
	group by $site_visit.visit_date,$site_visit.site,$site_visit.plot,throw_number,$surface_vegetation.scientific_name;" |
sql.e

species_category="species_category in ('submergent_vegetation')"
echo "	select $site_visit.visit_date,$site_visit.site,$site_visit.plot,throw_number,$submergent_vegetation.scientific_name,sum(density_percent)
	from $submergent_vegetation,$species,$site_visit
	where	$submergent_vegetation.scientific_name =
		$species.scientific_name
	  and	$submergent_vegetation.site = $site_visit.site
	  and	$submergent_vegetation.plot = $site_visit.plot
	  and	$submergent_vegetation.region = $site_visit.region
	  and	$site_visit.agency = 'national_park_service'
	  and	$species_category
	  and	$site_visit.visit_date between '$begin_date' and '$end_date'
	group by $site_visit.visit_date,$site_visit.site,$site_visit.plot,throw_number,$submergent_vegetation.scientific_name;" |
sql.e

exit 0
