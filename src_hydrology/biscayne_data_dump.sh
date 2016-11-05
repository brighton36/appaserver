:
# src_hydrology/biscayne_data_dump.sh
# -----------------------------------

if [ "$#" -ne 4 ]
then
	echo "Usage: $0 application begin_date end_date output_medium" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)
datatype=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
fi

begin_date=$2
end_date=$3
output_medium=$4

queue_number=500

if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="spreadsheet"
fi

measurement=`get_table_name $application measurement`
station=`get_table_name $application station`
output_file="${APPASERVER_HOME}/hydrology/biscayne_data_dump_$$.csv"
prompt_file="/hydrology/biscayne_data_dump_$$.csv"

select="$measurement.station,measurement_date,measurement_time,datatype,measurement_value"

where_agency="$station.agency = 'NPS_BISC'"
station_list=`echo "select station from $station where $where_agency;" | sql | joinlines.e ',' | single_quotes_around.e`

where="station in (${station_list}) and datatype in ('conductivity','salinity','surface_temperature','depth') and measurement_date between '$begin_date' and '$end_date'"

datatype_comma_list="conductivity,salinity,surface_temperature,depth"
heading="station,measurement_date,measurement_time,$datatype_comma_list"

(
echo ".heading$heading"							;
echo "select $select from $measurement where $where;"			|
sql_quick.e ','								|
grep -v '^BISCBB,'							|
sort
)									|
pivot_table.e $datatype_comma_list ','					|	
cat > $output_file

if [ "$output_medium" != "stdout" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/hydrology/style.css></head>"
	echo "<body><h1>Biscayne Data Dump</h1>"
fi

if [ "$output_medium" = "stdout" ]
then
	cat $output_file
elif [ "$output_medium" = "spreadsheet" ]
then
	echo "<a href=$prompt_file>Press to transmit file.</a>"
else
	cat $output_file				|
	grep -v '^station'				|
	queue_top_bottom_lines.e $queue_number		|
	html_table.e '' "$heading" ',' ''
fi

if [ "$output_medium" != "stdout" ]
then
	echo "</body></html>"
fi

exit 0

