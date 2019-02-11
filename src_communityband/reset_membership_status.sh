:
# src_communityband/reset_membership_status.sh
# --------------------------------------------

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
	echo "Usage: $0 ignored process_name execute_yn" 1>&2
	exit 1
fi

process_name=$2

execute_yn=$3

# Constants
# ---------

# Variables
# ---------
process_title=`echo "$process_name" | format_initial_capital.e`

# Process
# -------
where="status is null or status = 'Active'"

statement="select count(*)
from band_member
where ${where};"

content_type_cgi.sh

echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
echo "<body><h1>$process_title</h1>"

echo "$statement" | sql.e | html_table.e '' 'Count' ''

if [ "$execute_yn" = 'y' ]
then
	statement="	update band_member
			set status = 'Not Paid'
			where ${where};"
	echo "$statement" | sql.e
	echo "Process complete." | html_paragraph_wrapper
else
	echo "Process not executed." | html_paragraph_wrapper
fi

echo "</body></html>"

process_table_name=`get_table_name $application process`

echo "	update $process_table_name
	set execution_count =
		if(execution_count,execution_count+1,1)
	where process = '$process_name';"		|
sql.e

exit 0
