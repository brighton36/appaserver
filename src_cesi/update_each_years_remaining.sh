:
echo "$0 $*" 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

export DATABASE=$application

select="project_title"
table=project
where="award_end_date >= '`now.sh ymd -365`'"

echo "select $select from $table where $where;"	|
sql.e						|
while read project_title
do
	post_change_project.sh $application "$project_title"
done

exit 0
