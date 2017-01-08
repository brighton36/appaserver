:
# $APPASERVER_HOME/src_capitolpops/donor_list.sh
# ----------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------


# Input
# -----

# echo "$0" "$*" 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 application process_name year aggregate output_medium" 1>&2
	echo "Note: aggregate could be full_name" 1>&2
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
	aggregate="detail"		# Defaults to detail
fi

output_medium=$5
if [ "$output_medium" = "" -o "$output_medium" = "output_medium" ]
then
	output_medium="table"		# Defaults to table
fi

if [ "$output_medium" = "receipt" ]
then
	donor_receipt $application $process_name $year
	exit 0
fi

# Variables
# ---------
document_root=$(get_document_root.e)
output_file="$document_root/appaserver/$application/data/${process_name}_$$.dat"
csv_file="$document_root/appaserver/$application/data/${process_name}_$$.csv"
prompt_file="/appaserver/$application/data/${process_name}_$$.csv"
process_title=`echo "$process_name" | format_initial_capital.e`

where="donation_date like '${year}%' and donation_account.street_address <> 'null' and donation_account.street_address <> 'unknown'"

join="donation_account.full_name = entity.full_name and donation_account.street_address = entity.street_address"

# Process
# -------
if [ "$aggregate" = "detail" ]
then
	statement="select	donation_account.full_name,
				donation_date,
				account,
				donation_amount,
				donation_account.street_address,
				city,
				state_code,
				zip_code
	from donation_account,entity
	where $where and $join
	order by donation_account.full_name,donation_date;"

	heading="Full Name,Date,Account,Amount,Street Address,City,State,Zip code"
	justification="left,left,left,right,left,left,left,left"
elif [ "$aggregate" = "donor" ]
then
	statement="select distinct full_name
	from donation_account
	where $where
	order by full_name,donation_date;"

	heading="Full Name"
	justification="left"
elif [ "$aggregate" = "summary" ]
then
	statement="select	full_name,
				sum(donation_amount)
	from donation_account
	where $where
	group by full_name
	order by sum(donation_amount) desc;"

	heading="Full Name,Amount"
	justification="left,right"
else
# ----------------
# Individual donor
# ----------------
	statement="select	donation_date,
				account,
				donation_amount
	from donation_account
	where $where and full_name = '$aggregate'
	order by donation_date;"

	heading="Date,Account,Amount"
	justification="left,left,right"
fi

echo "$heading"				|
tee $output_file > $csv_file

echo "$statement"			|
sql.e '^'				|
tee -a $output_file			|
double_quote_comma_delimited '^'	|
cat >> $csv_file

# Output
# ------
if [ "$output_medium" != "stdout" ]
then
	content_type_cgi.sh

	echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
	echo "<body><h1>$process_title</h1>"
fi

if [ "$output_medium" = "spreadsheet" ]
then
	echo "<a href=$prompt_file>Press to transmit file.</a>"
elif [ "$output_medium" = "table" ]
then
	cat $output_file				|
	grep -v "$heading"				|
	html_table.e "" "$heading" "^" "$justification"
elif [ "$output_medium" = "stdout" ]
then
	cat $output_file
fi

if [ "$output_medium" != "stdout" ]
then
	echo "</body></html>"
fi

#process_table_name=`get_table_name $application process`
#
#echo "	update $process_table_name
#	set execution_count =
#		if(execution_count,execution_count+1,1)
#	where process = '$process_name';"		|
#sql.e

exit 0
