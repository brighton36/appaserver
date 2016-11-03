:
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application project where" 1>&2
	exit 1
fi

application=$1
project=$2
parameter_where=$3

if [ "$parameter_where" = "" -o "$parameter_where" = "$where" ]
then
	parameter_where="1 = 1"
fi

if [ "$project" = "" -o "$project" = "project" ]
then
	where="$parameter_where"
else
	where="$parameter_where and project = '$project'"
fi

table=$(get_table_name $application site)
select="location,project,site_number"
order="project,location,site_number"

echo "select $select from $table where $where order by $order;" | sql.e

exit 0
