:
# src_hydrology/mapping.sh
# -----------------------------------
# This is a wrapper around mapping.py
# -----------------------------------

echo "$0" "$*" 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 process begin_date end_date station_list omit_html_yn" 1>&2
	exit 1
fi

process=$1
begin_date=$2
end_date=$3
station_list=$4
omit_html_yn=$5

process_title=`echo $process | format_initial_capital.e`

if [ "$omit_html_yn" = "y" ]
then
	tee_process="tee /dev/tty"
else
	tee_process="cat"
fi


#begin_date="2017-06-01"
#end_date="2017-06-05"
#station_list="'BK','MK','JK','LR','PK','WB'"

document_root=$(get_document_root.e)
output_directory="$document_root/appaserver/$DATABASE/data"
output_file="$output_directory/mapping_${begin_date}_${end_date}"

select="station.station,datatype,lat_nad83,long_nad83,avg(measurement_value)"
group_by="station.station,datatype,lat_nad83,long_nad83"
heading="Station,Datatype,Latitude,Longitude,Value"

(
echo "$heading"
echo "	select $select
	from station, measurement
	where station.station = measurement.station
	  and station.station `in_clause.e $station_list`
	  and datatype = 'salinity'
	  and measurement_date between '${begin_date}' and '${end_date}'
	GROUP BY $group_by;"						|
$tee_process								|
sql.e ','								|
cat
)									|
cat > $output_file

return_file=`mapping.py $output_file`

prompt_file=$(echo "$return_file" | sed "s|`get_document_root.e`||")

if [ "$omit_html_yn" != "y" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$DATABASE/style.css></head>"
	echo "<body><h1>$process_title</h1>"
	echo "<a href=$prompt_file target=_new>Press to view file.</a>"
	echo "</body></html>"
else
	echo "Created: $return_file"
fi

exit 0
