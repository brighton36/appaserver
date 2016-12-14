:
# src_sparrow/site_visit_sparrow_observations.sh
# ----------------------------------------------

# Input
# -----
if [ "$#" -ne 4 ]
then
	echo "Usage: $0 application process_name survey_year output_medium" 1>&2
	exit 1
fi

echo $0 $* 1>&2

application=$(echo $1 | piece.e ':' 0)	# May have appended database
process_name=$2                       	# Assumed letters_and_underbars
survey_year=$3

output_medium=$4
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"		# Defaults to table
fi

# System variables
# ----------------
document_root_directory=`get_document_root.e`
appaserver_data_directory=`get_appaserver_data_directory.e`
observation_site_output_file="$appaserver_data_directory"/${application}_${process_name}_observation_site_$$.dat
site_visit_output_file="$appaserver_data_directory"/${application}_${process_name}_site_visit_$$.dat
output_file="$document_root_directory/$application/appaserver/${process_name}_$$.csv"
prompt_file="/$application/appaserver/${process_name}_$$.csv"
exit_flag=0

# User variables
# --------------
process_title=$(echo $process_name | format_initial_capital.e)

justification="left,left,left,left,left,right,right,right"
heading="quad_sheet,site_number,visit_date,visit_time,latitude,longitude,distance_from_observer_m,azimuth_degrees,bird_count"
select_observation_site="site_visit.quad_sheet,site_visit.site_number,site_visit.visit_date,site_visit.visit_time,observation_site.latitude,observation_site.longitude,sparrow_observation.distance_from_observer_m,sparrow_observation.azimuth_degrees,sparrow_observation.bird_count"
select_site_visit="site_visit.quad_sheet,site_visit.site_number,site_visit.visit_date,site_visit.visit_time,site_visit.latitude,site_visit.longitude,sparrow_observation.distance_from_observer_m,sparrow_observation.azimuth_degrees,sparrow_observation.bird_count"
where_join="site_visit.quad_sheet = sparrow_observation.quad_sheet and site_visit.site_number = sparrow_observation.site_number and site_visit.visit_date = sparrow_observation.visit_date and site_visit.quad_sheet = observation_site.quad_sheet and site_visit.site_number = observation_site.site_number"
where_observation_site="$where_join and survey_year = $survey_year and site_visit.latitude is null"
where_site_visit="$where_join and survey_year = $survey_year and site_visit.latitude is not null"

# Process
# -------
echo "$heading" > $output_file

echo "select $select_observation_site
from site_visit,sparrow_observation,observation_site
where $where_observation_site;"					|
sql.e ',' > $observation_site_output_file

echo "select $select_site_visit
from site_visit,sparrow_observation,observation_site
where $where_site_visit;"					|
sql.e ',' > $site_visit_output_file

cat $observation_site_output_file $site_visit_output_file	|
sort >> $output_file

# Output
# ------
if [ "$output_medium" != "stdout" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/$application/style.css></head>"
	echo "<body><h1>$process_title</h1>"
fi

if [ "$output_medium" = "text_file" -o "$output_medium" = "spreadsheet" ]
then
	echo "<a href=$prompt_file>Press to transmit file.</a>"
elif [ "$output_medium" = "table" ]
then
	cat $output_file				|
	html_table.e "" "$heading" "," "$justification"
elif [ "$output_medium" = "stdout" ]
then
	cat $output_file
else
	echo "Error: invalid output medium of $output_medium" 1>&2
	exit_flag=1
fi

if [ "$output_medium" != "stdout" ]
then
	echo "</body></html>"
fi

exit $exit_flag

