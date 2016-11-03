:
# src_waterquality/collections_without_results.sh
# -----------------------------------------------

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application process_name output_medium" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database
process_name=$2                       	# Assumed letters_and_underbars

process_title=`echo "$process_name" | format_initial_capital.e`

output_medium=$3

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

# Constants
# ---------
appaserver_home=$(get_appaserver_home.e)
output_file="$appaserver_home/$application/${process_name}_$$.csv"
prompt_file="/$application/${process_name}_$$.csv"
heading="Station,Collection Date,Collection Time"
justification="left"

subquery="
collection.station = results.station and
collection.collection_date = results.collection_date and
collection.collection_time = results.collection_time"

statement="
select station,collection_date,collection_time
from collection
where not exists ( select 1 from results where $subquery );"

echo "$heading" > $output_file

echo "$statement"			|
sql.e '^'				|
tr ',' '/'				|
tr '^' ','				|
sort 					|
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

	linecount=`cat $output_file			|
	grep -v "$heading"				|
	wc -l`
	echo "Line count: $linecount"			|
	html_paragraph_wrapper.e
elif [ "$output_medium" = "stdout" ]
then
	cat $output_file
fi

if [ "$output_medium" != "stdout" ]
then
	echo "</body></html>"
fi

exit 0
