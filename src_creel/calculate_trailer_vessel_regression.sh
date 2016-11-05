:
# src_creel/calculate_trailer_vessel_regression.sh
# ------------------------------------------------
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 parking_lot begin_date end_date" 1>&2
	exit 1
fi

application=creel

echo "$0 $*" 1>&2

parking_lot=$1

begin_date=$2
if [ "$begin_date" = "begin_date" -o "$begin_date" = "" ]
then
	begin_date="2000-01-01"
fi

end_date=$3
if [ "$end_date" = "end_date" -o "$end_date" = "" ]
then
	end_date="2099-12-31"
fi

document_root=$(get_document_root.e)
process_name=calculate_trailer_vessel_regression
output_file="$document_root/$application/appaserver/${process_name}_$$.csv"
prompt_file="/$application/appaserver/${process_name}_$$.csv"

content_type_cgi.sh

echo "<html><head><link rel=stylesheet type=text/css href=/creel/style.css></head>"
echo "<body><h1>Trailer/Vessel Regression</h1>"
echo "<h2>"
echo "$parking_lot $begin_date $end_date" | format_initial_capital.e
echo "</h2>"

aerial_vessel_count=`get_table_name $application aerial_vessel_count`
trailer_count=`get_table_name $application trailer_count`

heading="slope,y-intercept,standard_error_estimate (%),determination (R^2)"
justify="right,right,right,right,right"

echo "date,trailers,vessels" > $output_file

echo "	select $aerial_vessel_count.date,trailer_count,vessel_count
	from $aerial_vessel_count, $trailer_count
	where $aerial_vessel_count.date = $trailer_count.date
	  and $aerial_vessel_count.parking_lot = $trailer_count.parking_lot
	  and $trailer_count.parking_lot = '$parking_lot'
	  and $trailer_count.date between '$begin_date' and '$end_date';"     |
sql.e ' '								      |
tee_process.e "tr ' ' ',' >> $output_file"				      |
column.e 1,2								      |
regression.e								      |
html_table.e "Best-Fit Equation" "$heading" ' ' "$justify"

#heading="Dependent,covariance,count,average,standard_deviation,correlation"
#justify="left,right,right,right,right"
#
#(
#echo "	select date, 'trailer', trailer_count
#	from $trailer_count
#	where parking_lot = '$parking_lot'
#	  and date between '$begin_date' and '$end_date';"	|
#sql.e '^'							;
#echo "	select date, 'vessel', vessel_count
#	from $aerial_vessel_count
#	where parking_lot = '$parking_lot'
#	  and date between '$begin_date' and '$end_date';"	|
#sql.e '^'
#) 								|
#covariance.e trailer  0  1 2 '^' 2>/dev/null			|
#grep '^vessel'							|
#html_table.e '' "$heading" ':' "$justify"

echo "<br><a href=$prompt_file>Press to transmit the trailer/vessel pairs.</a>"

echo "</body></html>"

exit 0
