:
# -------------------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/import_predictivebooks_process.sh
# -------------------------------------------------------------------------

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

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 4 ]
then
	echo "Usage: $0 ignored process_name module execute_yn" 1>&2
	exit 1
fi

process_name=$2
module=$3
execute_yn=$4

# Variables
# ---------
if [ "$module" = "enterprise" ]
then
	process="predictivebooks_enterprise.sh"
elif [ "$module" = "nonprofit" ]
then
	process="predictivebooks_nonprofit.sh"
elif [ "$module" = "personal" ]
then
	process="predictivebooks_personal.sh"
fi

process_title=`echo "$process_name" | format_initial_capital.e`

# Process
# -------
content_type_cgi.sh

echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
echo "<body><h1>$process_title</h1>"

if [ "$process" = "" ]
then
	echo "<h3>Error: please choose a module.</h3>"
	echo "</body></html>"
	exit 0
fi

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
		where process = '$process_name';" |
	sql.e

	echo "<h3>Process complete.</h3>"
else
	echo "<h3>Will execute: `which $process`</h3>"
fi

echo "</body></html>"

exit 0
