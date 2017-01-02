:
# src_creel/total_trailer_count.sh
# --------------------------------

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 7 ]
then
	echo "Usage: $0 application process_name begin_year_month end_year_month parking_lot aggregate_level output_medium" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database
process_name=$2                       	# Assumed letters_and_underbars

begin_year_month=$3
if [ "$begin_year_month" = "" -o "$begin_year_month" = "begin_year_month" ]
then
	begin_year_month="1972-01"
fi

end_year_month=$4
if [ "$end_year_month" = "" -o "$end_year_month" = "end_year_month" ]
then
	end_year_month=`date.e 0 | piece_inverse.e 2 '-'`
fi

parking_lot=$5
if [ "$parking_lot" = "" -o "$parking_lot" = "parking_lot" ]
then
	parking_lot_where="1 = 1"
	parking_lot_title="ParkWide"
else
	parking_lot_title="$parking_lot"
	parking_lot_where=`or_sequence.e				\
			"parking_lot"  					\
			"${parking_lot}"`
fi

aggregate_level=$6
if [ "$aggregate_level" = "" -o "$aggregate_level" = "aggregate_level" ]
then
	aggregate_title="detail_trailer_count"
else
	aggregate_title=$process_name
fi

output_medium=$7
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"
fi

# Constants
# ---------
document_root=$(get_document_root.e)
output_file="$document_root/appaserver/$application/data/${process_name}_$$.csv"
prompt_file="/appaserver/$application/data/${process_name}_$$.csv"

# Variables
# ---------
trailer_count=`get_table_name $application trailer_count`
process_title=$(echo $parking_lot_title $aggregate_title from ${begin_year_month} to ${end_year_month} | format_initial_capital.e)

# Process
# -------
if [ "$aggregate_level" = "" -o "$aggregate_level" = "aggregate_level" ]
then
	statement="	select	date,
				parking_lot,
				trailer_count
			from $trailer_count
			where $parking_lot_where
			and date between
				'${begin_year_month}-01' and
				'${end_year_month}-31'
			order by date;"
	heading="Date,ParkingLot,TrailerCount"
	justification="left,left,right"
elif [ "$aggregate_level" = "annually" ]
then
	statement="	select	substr( date, 1, 4 ) year,
				sum( trailer_count),
				min( trailer_count),
				avg( trailer_count ),
				max( trailer_count ),
				count(*)
			from $trailer_count
			where $parking_lot_where
			and date between
				'${begin_year_month}-01' and
				'${end_year_month}-31'
			group by year;"
	heading="Year,TotalTrailers,Minimum,Average,Maximum,RecordCount"
	justification="left,right,right,right,right,right"
elif [ "$aggregate_level" = "year_month" ]
then
	statement="	select	substr( date, 1, 7 ) yearmonth,
				sum( trailer_count ),
				min( trailer_count),
				avg( trailer_count ),
				max( trailer_count ),
				count(*)
			from $trailer_count
			where $parking_lot_where
			and date between
				'${begin_year_month}-01' and
				'${end_year_month}-31'
			group by yearmonth;"
	heading="Year_Month,TotalTrailers,Minimum,Average,Maximum,RecordCount"
	justification="left,right,right,right,right,right"
elif [ "$aggregate_level" = "monthly" ]
then
	statement="	select	substr( date, 6, 2 ) month,
				sum( trailer_count ),
				min( trailer_count),
				avg( trailer_count ),
				max( trailer_count ),
				count(*)
			from $trailer_count
			where $parking_lot_where
			and date between
				'${begin_year_month}-01' and
				'${end_year_month}-31'
			group by month;"
	heading="Month,TotalTrailers,Minimum,Average,Maximum,RecordCount"
	justification="left,right,right,right,right,right"
fi


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
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
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

