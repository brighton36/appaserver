:
# src_capitolpops/reset_membership_status.sh
# ------------------------------------------

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

echo "<html><head><link rel=stylesheet type=text/css href=/$application/style.css></head>"
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
