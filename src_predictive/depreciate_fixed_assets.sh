:
# ----------------------------------------------------------------
# $APPASERVER_HOME/src_accountancymodel/depreciate_fixed_assets.sh
# ----------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 ignored process_name execute_yn" 1>&2
	exit 1
fi

process_name=$2
execute_yn=$3

# Constants
# ---------

# Variables
# ---------
heading="full_name,street_address,purchase_date_time,asset_name,serial_number"
field="$heading,depreciation_date"
today=`now.sh ymd`
process_title=`echo "$process_name" | format_initial_capital.e`

# Process
# -------
content_type_cgi.sh

echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
echo "<body><h1>$process_title</h1>"

if [ "$execute_yn" = 'y' ]
then
	depreciate_select.sh						|
	sed "s/$/^$today/"						|
	insert_statement.e table=depreciation field="$field" del="^"	|
	sql.e 2>&1							|
	html_paragraph_wrapper.e

	depreciate_select.sh						|
	sed "s/$/^$today/"						|
	post_change_depreciation $application "stdin"			|
	cat

	process_table_name=`get_table_name $application process`

	echo "	update $process_table_name
		set execution_count =
			if(execution_count,execution_count+1,1)
		where process = '$process_name';"		|
	sql.e

	echo "<h3>Process complete.</h3"
else
	depreciate_select.sh			|
	html_table.e '' "$heading" '^'
fi

echo "</body></html>"

exit 0
