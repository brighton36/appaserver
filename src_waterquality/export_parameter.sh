:
application=waterquality
table_name=$(get_table_name $application parameter)
select="parameter,units,lab_test_code,lab_storet_code"
output_file=/tmp/waterquality_parameter_unit.dat

echo "#${select}" > $output_file
echo "select $select from $table_name;" | sql.e >> $output_file
echo "Created $output_file"

exit 0
