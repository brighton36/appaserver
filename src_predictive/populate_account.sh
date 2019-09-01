:
# src_predictive/populate_account.sh
# ----------------------------------

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

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 [where]" 1>&2
	exit 1
fi

if [ "$#" -eq 1 -a "$1" != "where" -a "$1" != "\$where" ]
then
	parameter_where=$1
else
	parameter_where="1=1"
fi

where="$parameter_where"

table=$(get_table_name $application account)
select="chart_account_number,concat( '---', account,'|', account )"
order="chart_account_number"

echo "select $select from $table where $where order by $order;"	|
sql.e

exit 0
