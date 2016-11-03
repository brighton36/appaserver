:
# src_hydrology/salinity2oxygen_solubility.sh
# -------------------------------------------

# Constants
# ---------
OXYGEN_SOLUBILITY_DATATYPE="oxygen_solubility"

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 application process_name station where_clause really_yn" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)
datatype=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
fi

process_name=$2                       	# Assumed letters_and_underbars
station=$3
where=$4
really_yn=$5

# Constants
# ---------
appaserver_home=$(get_appaserver_home.e)

# Variables
# ---------
process_title=$(echo $process_name for ${station} | format_initial_capital.e)

# Process
# -------

# Output
# ------
echo "Content-type: text/html"
echo ""

echo "<html><head><link rel=stylesheet type=text/css href=/$application/style.css></head>"
echo "<body><h1>$process_title</h1>"

heading="Station,Datatype,Date,Time,Bottom Temperature,Salinity,Oxygen Solubility"
justification="left,left,left,left,right,right,right"

if [ "$really_yn" = "y" ]
then
	delete_where="$where and datatype = '$OXYGEN_SOLUBILITY_DATATYPE'"

	echo "delete from measurement where $delete_where;"		 |
	sql.e

	calculate_oxygen_solubility $application $station "$where" sql	 |
	sql.e

	echo "<p>Process complete."
else
	calculate_oxygen_solubility $application $station "$where" nosql |
	tr ' ' '^'							 |
	queue_top_bottom_lines.e 100					 |
	html_table.e '' "$heading" '^' "$justification"			 |
	cat
fi


echo "</body></html>"

exit 0

