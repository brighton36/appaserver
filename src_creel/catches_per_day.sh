:
# src_creel/catches_per_day.sh
# -------------------------------------------

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 8 ]
then
	echo "Usage: $0 application process_name begin_date end_date fishing_purpose output_medium fishing_area interview_location" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database
process_name=$2                       	# Assumed letters_and_underbars

begin_date=$3
if [ "$begin_date" = "" -o "$begin_date" = "begin_date" ]
then
	begin_date="1901-01-01"
fi

end_date=$4
if [ "$end_date" = "" -o "$end_date" = "end_date" ]
then
	end_date="2099-12-31"
fi

fishing_purpose=$5
if [ "$fishing_purpose" = "" -o "$fishing_purpose" = "fishing_purpose" ]
then
	fishing_purpose="sport"		# Defaults to sport
fi

output_medium=$6
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"		# Defaults to table
fi

fishing_area=$7
interview_location=$8

# Variables
# ---------
fishing_trips=`get_table_name $application fishing_trips`
catches=`get_table_name $application catches`
process_title=$(echo $fishing_purpose $process_name | format_initial_capital.e)

if [ "$fishing_area" = "" -o "$fishing_area" = "fishing_area" ]
then
	fishing_area_in="1 = 1"
else
	in=`echo "$fishing_area" | tr ',' '\n' | in_clause.e`
	fishing_area_in="fishing_area $in"
fi

if [	"$interview_location" = "" -o			\
	"$interview_location" = "interview_location" 	]
then
	interview_location_where="1 = 1"
else
	interview_location_where="$fishing_trips.interview_location = '$interview_location'"
fi

# Constants
# ---------
document_root=$(get_document_root.e)
output_file="$document_root/appaserver/$application/data/${process_name}_$$.csv"
prompt_file="/appaserver/$application/data/${process_name}_$$.csv"

# Process
# -------
statement="select $fishing_trips.census_date, sum( kept_count + released_count )
from $catches, $fishing_trips
where $fishing_trips.census_date between '$begin_date' and '$end_date'
and $fishing_trips.fishing_purpose = $catches.fishing_purpose
and $fishing_trips.census_date = $catches.census_date
and $fishing_trips.interview_location = $catches.interview_location
and $fishing_trips.interview_number = $catches.interview_number
and $fishing_trips.fishing_purpose = '$fishing_purpose'
and $fishing_area_in
and $interview_location_where
group by $fishing_trips.census_date;"

echo "$statement" 1>&2

heading="Date,Caught"
justification="left,right"

echo "$heading" > $output_file
echo "$statement" | sql.e ',' >> $output_file

# Output
# ------
if [ "$output_medium" != "stdout" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
	echo "<body><h1>$process_title</h1>"

	if [ "$fishing_area_in" != "1 = 1" ]
	then
		echo "<h2>`echo $fishing_area_in | format_initial_capital.e`</h2>"
	fi

	if [ "$interview_location_where" != "1 = 1" ]
	then
		echo "<h2>`echo $interview_location_where | format_initial_capital.e`</h2>"
	fi

fi

if [ "$output_medium" = "text_file" -o "$output_medium" = "spreadsheet" ]
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

