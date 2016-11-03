:
# src_creel/total_angler_count.sh
# -------------------------------------------

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 7 ]
then
	echo "Usage: $0 application process_name begin_year_month end_year_month fishing_purpose aggregate_level output_medium" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database
process_name=$2                       	# Assumed letters_and_underbars

begin_year_month=$3
if [ "$begin_year_month" = "" -o "$begin_year_month" = "begin_year_month" ]
then
	begin_year_month="1958-01"
fi

end_year_month=$4
if [ "$end_year_month" = "" -o "$end_year_month" = "end_year_month" ]
then
	end_year_month=`date.e 0 | piece_inverse.e 2 '-'`
fi

fishing_purpose=$5
if [ "$fishing_purpose" = "" -o "$fishing_purpose" = "fishing_purpose" ]
then
	fishing_purpose="sport"
fi

aggregate_level=$6
if [ "$aggregate_level" = "" -o "$aggregate_level" = "aggregate_level" ]
then
	aggregate_level="annually"
fi

output_medium=$7
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"
fi

# Constants
# ---------
appaserver_home=$(get_appaserver_home.e)
output_file="/$appaserver_home/$application/${process_name}_$$.csv"
prompt_file="/$application/${process_name}_$$.csv"

# Variables
# ---------
fishing_trips=`get_table_name $application fishing_trips`
process_title=$(echo $fishing_purpose $process_name from ${begin_year_month} to ${end_year_month} | format_initial_capital.e)

# Process
# -------
if [ "$aggregate_level" = "annually" ]
then
	statement="	select	substr( census_date, 1, 4 ) year,
				sum( number_of_people_fishing ),
				min( number_of_people_fishing),
				avg( number_of_people_fishing ),
				max( number_of_people_fishing ),
				count(*)
			from $fishing_trips
			where hours_fishing > 0
			and fishing_purpose = '$fishing_purpose'
			and census_date between
				'${begin_year_month}-01' and
				'${end_year_month}-31'
			group by year;"
	heading="Year,Total,Minimum,Average,Maximum,RecordCount"
elif [ "$aggregate_level" = "year_month" ]
then
	statement="	select	substr( census_date, 1, 7 ) yearmonth,
				sum( number_of_people_fishing ),
				min( number_of_people_fishing),
				avg( number_of_people_fishing ),
				max( number_of_people_fishing ),
				count(*)
			from $fishing_trips
			where hours_fishing > 0
			and fishing_purpose = '$fishing_purpose'
			and census_date between
				'${begin_year_month}-01' and
				'${end_year_month}-31'
			group by yearmonth;"
	heading="Year_Month,Total,Minimum,Average,Maximum,RecordCount"
elif [ "$aggregate_level" = "monthly" ]
then
	statement="	select	substr( census_date, 6, 2 ) month,
				sum( number_of_people_fishing ),
				min( number_of_people_fishing),
				avg( number_of_people_fishing ),
				max( number_of_people_fishing ),
				count(*)
			from $fishing_trips
			where hours_fishing > 0
			and fishing_purpose = '$fishing_purpose'
			and census_date between
				'${begin_year_month}-01' and
				'${end_year_month}-31'
			group by month;"
	heading="Month,Total,Minimum,Average,Maximum,RecordCount"
fi

justification="left,right,right,right,right,right"

if [ "$output_medium" = "text_file" ]
then
	title_maybe=$process_title
	heading_maybe=$heading
elif [ "$output_medium" = "table" ]
then
	title_maybe=""
	heading_maybe=""
elif [ "$output_medium" = "stdout" ]
then
	title_maybe="#$process_title"
	heading_maybe="#$heading"
fi

echo "$title_maybe" > $output_file
echo "$heading_maybe" >> $output_file
echo "$statement" | sql.e ',' >> $output_file

# Output
# ------
if [ "$output_medium" != "stdout" ]
then
	echo "Content-type: text/html"
	echo ""

	echo "<html><head><link rel=stylesheet type=text/css href=/$application/style.css></head>"
	echo "<body><h1>$process_title</h1>"
fi

if [ "$output_medium" = "text_file" ]
then
	echo "<a href=$prompt_file>Press to transmit file.</a>"
elif [ "$output_medium" = "table" ]
then
	cat $output_file				|
	grep -v "$heading"				|
	html_table.e "" "$heading" "," "$justification"
elif [ "$output_medium" = "stdout" ]
then
	cat $output_file
fi

if [ "$output_medium" != "stdout" ]
then
	echo "</body></html>"
fi

exit 0

