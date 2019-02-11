:
# src_capitolpops/membership_status_list.sh
# -----------------------------------------

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 ignored process_name output_medium" 1>&2
	exit 1
fi

process_name=$2

output_medium=$3
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"		# Defaults to table
fi

# Variables
# ---------
document_root=$(get_document_root.e)
output_file="$document_root/appaserver/$application/data/${process_name}_$$.csv"
prompt_file="/appaserver/$application/data/${process_name}_$$.csv"
process_title=`echo "$process_name" | format_initial_capital.e`

# Process
# -------
statement="select	
			band_member.status,
			band_member.full_name,
			primary_instrument,
			email_address,
			home_phone,
			cell_phone
from band_member,musician,instrument,status
where band_member.full_name = musician.full_name
  and musician.primary_instrument = instrument.instrument
  and band_member.status = status.status
order by status.sort_order,musician.full_name;"

heading="Status,Full Name,Instrument,Email,Home,Cell"
justification="left"

echo "$heading" > $output_file
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

process_table_name=`get_table_name $application process`

echo "	update $process_table_name
	set execution_count =
		if(execution_count,execution_count+1,1)
	where process = '$process_name';"		|
sql.e

exit 0
