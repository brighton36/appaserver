:
# src_benthic/scubadive_time_report.sh
# ------------------------------------

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 application process_name researcher year output_medium" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database
process_name=$2                       	# Assumed letters_and_underbars

process_title=`echo "$process_name" | format_initial_capital.e`

researcher=$3
year=$4

output_medium=$5
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"		# Defaults to table
fi

if [ "$output_medium" != "stdout" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/$application/style.css></head>"
	echo "<body><h1>$process_title</h1>"
fi

if [ "$researcher" = "" -o "$researcher" = "researcher" ]
then
	echo "<h3>Please choose a researcher.</h3>"
	echo "</body></html>"
	exit 0
fi

if [ "$year" = "" -o "$year" = "year" ]
then
	echo "<h3>Please enter in a year.</h3>"
	echo "</body></html>"
	exit 0
fi

# Constants
# ---------
document_root=$(get_document_root.e)
collection_file="$document_root/$application/appaserver/${process_name}_collection_$$.txt"
collection_diver_minutes_file="$document_root/$application/appaserver/${process_name}_collection_minutes_between_$$.txt"
collection_master_file="$document_root/$application/appaserver/${process_name}_collection_master_$$.txt"
water_depth_collection_file="$document_root/$application/appaserver/${process_name}_water_depth_collection_$$.txt"
habitat_file="$document_root/$application/appaserver/${process_name}_habitat_$$.txt"
habitat_dive_minutes_file="$document_root/$application/appaserver/${process_name}_habitat_minutes_between_$$.txt"
habitat_master_file="$document_root/$application/appaserver/${process_name}_habitat_master_$$.txt"
water_depth_habitat_file="$document_root/$application/appaserver/${process_name}_water_depth_habitat_$$.txt"
sort_file="$document_root/$application/appaserver/${process_name}_sort_$$.txt"
output_file="$document_root/$application/appaserver/${process_name}_$$.csv"
prompt_file="/$application/appaserver/${process_name}_$$.csv"

# Tables
# ------
sampling_point=`get_table_name $application sampling_point`
throwtrap_abiotic_measurement=`get_table_name $application throwtrap_abiotic_measurement`

# Collection diver minutes
# ------------------------
statement="
select concat( anchor_date, ':', begin_collection_dive_time ), concat( anchor_date, ':', end_collection_dive_time )
from $sampling_point
where ( collection_diver1 = '$researcher' or collection_diver2 = '$researcher' )
  and begin_collection_dive_time is not null
  and anchor_date like '$year-%'
order by anchor_date, begin_collection_dive_time;"

echo "$statement"			|
sql.e ' '				|
minutes_between.e			|
cat > $collection_diver_minutes_file

# Collection diver master
# -----------------------
diver1_statement="
select collection_diver1, concat( anchor_date, ':', begin_collection_dive_time ), concat( anchor_date, ':', end_collection_dive_time )
from $sampling_point
where collection_diver1 = '$researcher'
  and begin_collection_dive_time is not null
  and anchor_date like '$year-%'
order by anchor_date, begin_collection_dive_time;"

diver2_statement="
select collection_diver2, concat( anchor_date, ':', begin_collection_dive_time ), concat( anchor_date, ':', end_collection_dive_time )
from $sampling_point
where collection_diver2 = '$researcher'
  and begin_collection_dive_time is not null
  and anchor_date like '$year-%'
order by anchor_date, begin_collection_dive_time;"

(
echo "$diver1_statement" | sql.e ',';
echo "$diver2_statement" | sql.e ','
) | sort > $collection_master_file

# Water depth collection
# ----------------------
statement="
select avg( value )
from $sampling_point
left join $throwtrap_abiotic_measurement
   on $sampling_point.anchor_date = $throwtrap_abiotic_measurement.anchor_date
  and $sampling_point.anchor_time = $throwtrap_abiotic_measurement.anchor_time
  and $sampling_point.location = $throwtrap_abiotic_measurement.location
  and $sampling_point.site_number = $throwtrap_abiotic_measurement.site_number
  and datatype = 'water_depth_cm'
where ( collection_diver1 = '$researcher'
   or collection_diver2 = '$researcher' )
  and begin_collection_dive_time is not null
  and $sampling_point.anchor_date like '$year-%'
group by $sampling_point.anchor_date,begin_collection_dive_time"

echo "$statement"				|
sql.e ','					|
awk '{ printf( "%.1f\n", $1 * 0.03281 ) }'	|
cat > $water_depth_collection_file

# Habitat diver minutes
# ---------------------
statement="
select concat( anchor_date, ':', begin_habitat_dive_time ), concat( anchor_date, ':', end_habitat_dive_time )
from $sampling_point
where habitat_diver = '$researcher'
  and begin_habitat_dive_time is not null
  and anchor_date like '$year-%'
order by anchor_date, begin_habitat_dive_time;"

echo "$statement"			|
sql.e ' '				|
minutes_between.e			|
cat > $habitat_dive_minutes_file

# Habitat diver master
# --------------------
statement="
select habitat_diver, concat( anchor_date, ':', begin_habitat_dive_time ), concat( anchor_date, ':', end_habitat_dive_time )
from $sampling_point
where habitat_diver = '$researcher'
  and begin_habitat_dive_time is not null
  and anchor_date like '$year-%'
order by anchor_date, begin_habitat_dive_time;"

echo "$statement" | sql.e ',' > $habitat_master_file

# Water depth habitat
# -------------------
statement="
select avg( value )
from $sampling_point
left join $throwtrap_abiotic_measurement
   on $sampling_point.anchor_date = $throwtrap_abiotic_measurement.anchor_date
  and $sampling_point.anchor_time = $throwtrap_abiotic_measurement.anchor_time
  and $sampling_point.location = $throwtrap_abiotic_measurement.location
  and $sampling_point.site_number = $throwtrap_abiotic_measurement.site_number
  and datatype = 'water_depth_cm'
where habitat_diver = '$researcher'
  and begin_habitat_dive_time is not null
  and $sampling_point.anchor_date like '$year-%'
group by $sampling_point.anchor_date,begin_habitat_dive_time;"

echo "$statement"				|
sql.e ','					|
awk '{ printf( "%.1f\n", $1 * 0.03281 ) }'	|
cat > $water_depth_habitat_file

# Combine process
# ---------------
heading="Diver,BeginDive,EndDive,Depth (FT),Minutes"
justification="left,right,right,right"

joinfiles.e $collection_master_file $water_depth_collection_file ','	|
cat > $collection_file

joinfiles.e $collection_file $collection_diver_minutes_file ','		|
cat > $sort_file

joinfiles.e $habitat_master_file $water_depth_habitat_file ','		|
cat > $habitat_file

joinfiles.e $habitat_file $habitat_dive_minutes_file ','		|
cat >> $sort_file

echo "$heading" > $output_file

sort $sort_file						|
piece_sum.e ',' 4 n					|
cat >> $output_file

# Output
# ------
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
