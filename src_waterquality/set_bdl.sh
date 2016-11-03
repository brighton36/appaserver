:
application=waterquality
table=`get_table_name $application results`

primary_key="station,collection_date,collection_time,parameter,units"


select="$primary_key,'concentration',''"
#select="$primary_key,'concentration',minimum_detection_limit / 2"

where="exception = 'below_detection_limit'" 

echo "select $select from $table where $where;"			|
sql								|
update_statement.e table=$table key=$primary_key carrot=y	|
cat

exit 0
