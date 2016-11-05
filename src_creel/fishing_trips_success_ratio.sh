:
# src_creel/fishing_trips_success_ratio.sh
# ----------------------------------------

if [ "$#" -ne 6 ]
then
	echo "Usage: $0 application process begin_year end_year fishing_purpose output_medium" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database
process_name=$2                       	# Assumed letters_and_underbars

begin_year=$3
if [ "$begin_year" = "" -o "$begin_year" = "begin_year" ]
then
	begin_year="1901"
fi

end_year=$4
if [ "$end_year" = "" -o "$end_year" = "end_year" ]
then
	end_year=`date.e 0 | piece.e '-' 0`
fi

fishing_purpose=$5
if [ "$fishing_purpose" = "" -o "$fishing_purpose" = "fishing_purpose" ]
then
	fishing_purpose="sport"
fi

output_medium=$6

process_title=`echo "$fishing_purpose $process_name" | format_initial_capital.e`

fishing_trips=`get_table_name $application fishing_trips`
catches=`get_table_name $application catches`
file1="/tmp/creel_join1_$$"
file2="/tmp/creel_join2_$$"
output_file="/appaserver/creel/${process_name}_${fishing_purpose}_$$.csv"
prompt_file="/creel/${process_name}_${fishing_purpose}_$$.csv"

# Select catches
# --------------
echo "	select substr( census_date, 1, 4 ) year, count(*)
	from $fishing_trips
	where exists (	select * from
			$catches
			where $fishing_trips.fishing_purpose =
				$catches.fishing_purpose
			and $fishing_trips.census_date = $catches.census_date
			and $fishing_trips.interview_location =
				$catches.interview_location
			and $fishing_trips.interview_number =
				$catches.interview_number )
	and fishing_purpose = '$fishing_purpose'
	and hours_fishing > 0
	and census_date between '${begin_year}-01-01' and '${end_year}-12-31'
	group by year;"			|
sql.e > $file1

# Select no-catches
# -----------------
echo "	select substr( census_date, 1, 4 ) year, count(*)
	from $fishing_trips
	where not exists (
			select *
			from $catches
			where $fishing_trips.fishing_purpose =
				$catches.fishing_purpose
			and $fishing_trips.census_date =
				$catches.census_date
			and $fishing_trips.interview_location =
				$catches.interview_location
			and $fishing_trips.interview_number =
				$catches.interview_number )
	and fishing_purpose = '$fishing_purpose'
	and hours_fishing > 0
	group by year;"			|
sql.e > $file2

(
echo "Year,Successes,Failures,Total,Success %"
join -t'^' -a1 $file1 $file2			|
awk 'BEGIN {
	FS = "^"
}
{
	year = $1
	success_count = $2
	failure_count = $3
	denominator = success_count + failure_count
	results = ( success_count / denominator ) * 100
	printf ( "%s^%s^%s^%d^%.1f\n",
		year,
		success_count,
		failure_count,
		denominator,
		results )
}'						|
tr '^' ','
) | cat > $output_file

if [ "$output_medium" != "stdout" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/creel/style.css></head>"
	echo "<body><h1>$process_title</h1>"
fi

if [ "$output_medium" = "stdout" ]
then
	cat $output_file
elif [ "$output_medium" = "text_file" ]
then
	echo "<a href=$prompt_file>Press to transmit file.</a>"
else
	cat $output_file				|
	html_table.e '' '' ',' 'right,right,right,right'
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

