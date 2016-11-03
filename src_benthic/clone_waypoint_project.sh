:
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 source_project destination_project location" 1>&2
	exit 1
fi

source_project=$1
destination_project=$2
location=$3

where="project = '$source_project' and location = '$location'"

select="location,'$destination_project',site_number,latitude,longitude,waypoint_number,inaccessible_yn"
field="location,project,site_number,latitude,longitude,waypoint_number,inaccessible_yn"

echo "select $select from waypoint where $where;"	|
sql							|
insert_statement.e table=waypoint field=$field del='^'	|
cat

exit 0
