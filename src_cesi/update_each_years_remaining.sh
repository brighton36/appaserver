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

echo "select $select from $table;"	|
sql.e					|
while read project_title
do
	post_change_project.sh $application "$project_title"
done

exit 0
