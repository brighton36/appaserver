:
echo "$0 $*" 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application project_title" 1>&2
	exit 1
fi

application=$1
project_title=$2

export DATABASE=$application

project_title_escaped=`echo $project_title | escape_character.e "'"`

select="award_end_date"
table=project
where="project_title = '$project_title_escaped'"

award_end_date=`echo "select $select from $table where $where;"	|
		sql.e`

if [ "$award_end_date" = "" ]
then
	exit 0
fi

if [ "$award_end_date" = "" ]
then
	years_remaining="0"
else
	years_remaining=$(years_between.sh `now.sh ymd` $award_end_date)
fi

if [ "$years_remaining" -lt 0 ]
then
	years_remaining="0"
fi

key="project_title"

echo "$project_title^years_remaining^$years_remaining"			|
update_statement table=$table key=$key carrot=y				|
sql.e

exit 0
