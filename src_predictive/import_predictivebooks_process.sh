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

if [ "$#" -ne 7 ]
then
	echo "Usage: $0 login_name process_name module cash_opening_balance cash_account_name equity_account_name execute_yn" 1>&2
	exit 1
fi

login_name=$1
process_name=$2
module=$3
cash_opening_balance=$4
cash_account_name=$5
equity_account_name=$6
execute_yn=$7

# Variables
# ---------
if [ "$module" = "enterprise" ]
then
	process="predictivebooks_enterprise.sh"
elif [ "$module" = "community_band" ]
then
	process="predictivebooks_communityband.sh"
elif [ "$module" = "autorepair" ]
then
	process="predictivebooks_autorepair.sh"
elif [ "$module" = "rentalproperty" ]
then
	process="predictivebooks_rentalproperty.sh"
elif [ "$module" = "nonprofit" ]
then
	process="predictivebooks_nonprofit.sh"
elif [ "$module" = "home" ]
then
	process="predictivebooks_home.sh"
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
	export login_name=${login_name}
	export cash_opening_balance=${cash_opening_balance}
	export cash_account_name=${cash_account_name}
	export equity_acocunt_name=${equity_acocunt_name}

	$process

	echo "	delete from role_process
		where process = '$process_name';" |
	sql.e

	echo "<h3>Process complete.</h3>"
else
	echo "<h3>Will execute: \$APPASERVER_HOME/${process}`</h3>"
fi

echo "</body></html>"

exit 0
