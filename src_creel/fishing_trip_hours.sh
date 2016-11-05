:
# src_creel/fishing_trip_hours.sh
# -------------------------------

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 8 ]
then
	echo "Usage: $0 application process_name begin_date end_date fishing_purpose boating_purpose aggregate_level output_medium" 1>&2
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
	end_date=`date.e 0 | piece.e ':' 0`
fi

fishing_purpose=$5
if [ "$fishing_purpose" = "" -o "$fishing_purpose" = "fishing_purpose" ]
then
	fishing_purpose="sport"
fi

boating_purpose=$6
if [ "$boating_purpose" = "" -o "$boating_purpose" = "boating_purpose" ]
then
	boating_purpose="fishing"
fi

if [ "$boating_purpose" = "fishing" ]
then
	hours_fishing_where="hours_fishing > 0"
	aggregate_select=",min(hours_fishing),avg(hours_fishing),max(hours_fishing)"
elif [ "$boating_purpose" = "pleasure" ]
then
	hours_fishing_where="ifnull( hours_fishing, 0 ) = 0"
	aggregate_select=""
fi

aggregate_level=$7

output_medium=$8
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"		# Defaults to table
fi

# Constants
# ---------
document_root=$(get_document_root.e)
output_file="$document_root/$application/appaserver/${process_name}_$$.csv"
prompt_file="/$application/appaserver/${process_name}_$$.csv"

# Variables
# ---------
fishing_trips=`get_table_name $application fishing_trips`
catches=`get_table_name $application catches`
process_title=`echo "$fishing_purpose $process_name (${boating_purpose})" | format_initial_capital.e`


# Process
# -------
if [ "$aggregate_level" = "" -o "$aggregate_level" = "aggregate_level" ]
then

statement="select census_date,fishing_purpose,interview_number,hours_fishing
from $fishing_trips
where census_date between '$begin_date' and '$end_date'
and fishing_purpose = '$fishing_purpose'
and $hours_fishing_where
order by census_date;"

heading="Date,Purpose,Interview Number,Hours Fishing"
justification="left,left,right,right"

elif [ "$aggregate_level" = "daily" ]
then

statement="select census_date,fishing_purpose,count(*)${aggregate_select}
from $fishing_trips
where census_date between '$begin_date' and '$end_date'
and fishing_purpose = '$fishing_purpose'
and $hours_fishing_where
group by census_date;"

heading="Date,Purpose,Trip Count,Min. Hours,Avg. Hours,Max. Hours"
justification="left,left,right,right,right,right"

elif [ "$aggregate_level" = "monthly" ]
then

statement="select substr( census_date, 1, 7),fishing_purpose,count(*)${aggregate_select}
from $fishing_trips
where census_date between '$begin_date' and '$end_date'
and fishing_purpose = '$fishing_purpose'
and $hours_fishing_where
group by substr( census_date, 1, 7);"

heading="Month,Purpose,Trip Count,Min. Hours,Avg. Hours,Max. Hours"
justification="left,left,right,right,right,right"

elif [ "$aggregate_level" = "annually" ]
then

statement="select substr( census_date, 1, 4),fishing_purpose,count(*)${aggregate_select}
from $fishing_trips
where census_date between '$begin_date' and '$end_date'
and fishing_purpose = '$fishing_purpose'
and $hours_fishing_where
group by substr( census_date, 1, 4);"

heading="Year,Purpose,Trip Count,Min. Hours,Avg. Hours,Max. Hours"
justification="left,left,right,right,right,right"

fi

echo "$heading" > $output_file
echo "$statement" | sql.e ',' >> $output_file

# Output
# ------
if [ "$output_medium" != "stdout" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/$application/style.css></head>"
	echo "<body><h1>$process_title from $begin_date to $end_date</h1>"
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

process_table_name=`get_table_name $application process`

echo "	update $process_table_name
	set execution_count =
		if(execution_count,execution_count+1,1)
	where process = '$process_name';"		|
sql.e

exit 0
