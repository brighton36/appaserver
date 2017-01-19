:
# src_creel/fishing_trips_per_year.sh
# -----------------------------------

if [ "$#" -ne 7 ]
then
	echo "Usage: $0 application process begin_year end_year fishing_purpose boating_purpose output_medium" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database
process=$2
begin_year=$3
end_year=$4
fishing_purpose=$5
boating_purpose=$5
output_medium=$7

fishing_trips=`get_table_name $application fishing_trips`
catches=`get_table_name $application catches`
output_file="/appaserver/creel/fishing_trips_per_year_$$.csv"
prompt_file="/creel/fishing_trips_per_year_$$.csv"

if [ "$begin_year" = "" -o "$begin_year" = "begin_year" ]
then
	begin_year=1958
fi

if [ "$end_year" = "" -o "$end_year" = "end_year" ]
then
	end_year=`date.e 0 | piece.e '-' 0`
fi

	census_date_where="census_date between '${begin_year}-01-01' and '${end_year}-12-31'"

if [ "$boating_purpose" = "fishing" ]
then
	hours_fishing_where="hours_fishing > 0"
elif [ "$boating_purpose" = "pleasure" ]
then
	hours_fishing_where="ifnull( hours_fishing, 0 ) = 0"
else
	hours_fishing_where="1 = 1"
fi

heading="Year,TotalTrips,MinHoursFishing,MaxHoursFishing,AvgHoursFishing"

echo "$heading"	> $output_file

echo "select substr( census_date, 1, 4 ) year, count(*), min( hours_fishing ), max( hours_fishing), avg( hours_fishing ) from $fishing_trips where fishing_purpose = '$fishing_purpose' and $hours_fishing_where and $census_date_where group by year;"				|
sql.e ',' >> $output_file

if [ "$output_medium" != "stdout" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/creel/style.css></head>"
	echo "<body><h1>Fishing Trips Per Year</h1>"
fi

if [ "$output_medium" = "stdout" ]
then
	cat $output_file
elif [ "$output_medium" = "text_file" ]
then
	echo "<a href=$prompt_file>Press to transmit file.</a>"
else
	cat $output_file				|
	grep -v "$heading"				|
	html_table.e '' "$heading" ',' 'right,right,right'
fi

if [ "$output_medium" != "stdout" ]
then
	echo "</body></html>"
fi

exit 0

