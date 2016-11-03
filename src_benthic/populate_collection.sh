:
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application project" 1>&2
	exit 1
fi

application=$1
project=$2

if [ "$project" = "" -o "$project" = "project" ]
then
	parameter_where="1 = 1"
else
	parameter_where="project.project = '$project'"
fi

table="collection,project"
select="collection.collection_name,project.project"
order="project.sort_order,collection.collection_number desc"

join_where="collection.project = project.project"
where="$parameter_where and $join_where"

echo "select $select from $table where $where order by $order;" | sql.e

exit 0
