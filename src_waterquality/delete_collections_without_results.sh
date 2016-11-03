:
# src_waterquality/delete_collections_without_result.sh
# -----------------------------------------------------------

# Input
# -----

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database

subquery="
collection.station = results.station and
collection.collection_date = results.collection_date and
collection.collection_time = results.collection_time"

statement="
select station,collection_date,collection_time
from collection
where not exists ( select 1 from results where $subquery );"

table="collection"
field="station,collection_date,collection_time"

echo "$statement"					|
sql_quick.e '^'						|
delete_statement table=$table field=$field del='^'	|
cat

exit 0
