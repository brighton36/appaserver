:
# src_hydrology/calculate_station_utm.sh
# -------------------------------------------

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 4 ]
then
	echo "Usage: $0 application process_name station output_medium" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database
process_name=$2                       	# Assumed letters_and_underbars

if [ "$#" -ge 3 -a "$3" != "station" ]
then
	station_where="station in (`single_quotes_around.e $3`)"
else
	station_where="1 = 1"
fi

output_medium=$4
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"
fi

# Constants
# ---------
document_root=$(get_document_root.e)
output_file="$document_root/$application/appaserver/${process_name}_$$.csv"
prompt_file="/$application/appaserver/${process_name}_$$.csv"

# Variables
# ---------
station_table=`get_table_name $application station`
park_table=`get_table_name $application park`
process_title=$(echo $process_name | format_initial_capital.e)

# Process
# -------
statement="	select	station, 
			long_nad83,
			lat_nad83,
			UTM_zone
			from $station_table left join $park_table
			  on $station_table.park = $park_table.park
			where $station_where
			order by station;"
heading="Station,Longitude,Latitude,UTM Zone,UTM Easting, UTM Northing"
justification="left,right"

if [ "$output_medium" = "spreadsheet" ]
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
echo "$statement" | sql.e ',' | calculate_station_utm_cs2cs.sh >> $output_file

# Output
# ------
if [ "$output_medium" != "stdout" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/$application/style.css></head>"
	echo "<body><h1>$process_title</h1>"
fi

if [ "$output_medium" = "spreadsheet" ]
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

