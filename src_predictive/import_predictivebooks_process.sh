:
# -------------------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/import_predictivebooks_process.sh
# -------------------------------------------------------------------------

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application process_name execute_yn" 1>&2
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
execute_yn=$3

# Constants
# ---------
process="import_predictivebooks.sh"

# Variables
# ---------
process_title=`echo "$process_name" | format_initial_capital.e`

# Process
# -------
content_type_cgi.sh

echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
echo "<body><h1>$process_title</h1>"

results=`echo "show tables;" | sql.e | grep "^transaction$"`

if [ "$results" = "transaction" ]
then
	echo "<h3>Warning: PredictiveBooks is already loaded.</h3>"
	echo "<h3>Existing data will be deleted.</h3>"
fi

if [ "$execute_yn" = 'y' ]
then
	$process $application

	echo "	delete from role_process
		where process = '$process';" |
	sql.e

	echo "<h3>Process complete.</h3"
	echo "<h3>Press the &lt;Recount&gt; button to refresh the menu counts.</h3>"
else
	echo "<h3>Will execute:</h3"
	cat `which $process` | html_paragraph_wrapper.e
fi

echo "</body></html>"

exit 0
