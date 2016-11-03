:
if [ "$#" -eq 0 ]
then
	echo "Usage: $0 application [project]" 1>&2
	exit 1
fi

application=$1

if [ $# -eq 2 ]
then
	project=$2
fi

if [ "$project" = "" -o "$project" = "project" ]
then
	parameter_where="1 = 1"
else
	parameter_where="project.project = '$project'"
fi

table="location_project,project"
select="location_project.location,location_project.project"
order="project.sort_order,location_project.location"

join_where="location_project.project = project.project"
where="$parameter_where and $join_where"

echo "select $select from $table where $where order by $order;" | sql.e

exit 0
