:
# src_capitolpops/membership_status_list.sh
# -----------------------------------------

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 application process_name year aggregate output_medium" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)
database=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
else
	export DATABASE=$application
fi

process_name=$2
year=$3

aggregate=$4
if [ "$aggregate" = "" -o "$aggregate" = "aggregate" ]
then
	output_medium="detail"		# Defaults to detail
fi

output_medium=$5
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"		# Defaults to table
fi

# Variables
# ---------
document_root=$(get_document_root.e)
output_file="$document_root/$application/appaserver/${process_name}_$$.csv"
prompt_file="/$application/appaserver/${process_name}_$$.csv"
process_title=`echo "$process_name" | format_initial_capital.e`

# Process
# -------
if [ "$aggregate" = "detail" ]
then
	statement="select	full_name,
				donation_date,
				account,
				donation_amount
	from donation_program
	where donation_date like '${year}%'
	order by full_name,donation_date;"

	heading="Full Name,Date,Account,Amount"
	justification="left,left,left,right"
else
	statement="select	full_name,
				sum(donation_amount)
	from donation_program
	where donation_date like '${year}%'
	group by full_name
	order by sum(donation_amount) desc;"

	heading="Full Name,Amount"
	justification="left,right"
fi

echo "$heading" > $output_file
echo "$statement"		|
sql.e ',' >> $output_file

# Output
# ------
if [ "$output_medium" != "stdout" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/$application/style.css></head>"
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
