:

output_file=measurement.txt.gz

select_column_list=station,datatype,measurement_date,measurement_time,measurement_date,measurement_time,measurement_value,last_validation_date,last_person_validating,measurement_update_method,last_validation_process,reason_value_missing,delta_time

#echo "	select $select_column_list			\
#	from measurement				\
#	where station = '1AAA'				\
#	  and datatype = 'stage'			\
#	  and measurement_date = '1999-06-01';"		|
echo "	select $select_column_list			\
	from measurement;"				|
sql_quick.e '|'						|
grep -v '0000-00-00'					|
grep -v '^null'						|
mysql_date2ora_date.e 2 '|'				|
mysql_date2ora_date.e 4 '|'				|
mysql_date2ora_date.e 7 '|'				|
sed 's/|/:/5'						|
sed 's/:null/:0000/'					|
gzip > $output_file

echo Created ${output_file}

exit 0
