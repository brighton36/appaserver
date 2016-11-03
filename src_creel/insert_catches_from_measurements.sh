:
application=creel
catch_measurements=`get_table_name $application catch_measurements`
catches=`get_table_name $application catches`

select="fishing_purpose,census_date,interview_location,interview_number,family,genus,species,count(*)"
group="fishing_purpose,census_date,interview_location,interview_number,family,genus,species"
field="fishing_purpose,census_date,interview_location,interview_number,family,genus,species,kept_count"
#where="census_date < '1980-01-01'"
where="1 = 1"

echo "select $select from $catch_measurements where $where group by $group;" |
sql_quick.e								     |
insert_statement.e table=$catches field=$field del='^'

exit 0
