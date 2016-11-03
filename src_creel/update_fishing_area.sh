:
table_name=`get_table_name creel fishing_trips`
echo "select fishing_purpose,census_date,interview_location,interview_number,concat( location_area,upper(location_zone)) from $table_name;"		|
sql.e									|
sed 's/ENT.*$//'							|
piece_insert.e 4 '^' fishing_area					|
sed 's/fishing_area\^$/fishing_area^0/'					|
update_statement.e table=$table_name key=fishing_purpose,census_date,interview_location,interview_number carrot=y 					|
cat
exit 0
