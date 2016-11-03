:
output_filename=/tmp/fishing_area.txt
table_name=`get_table_name creel fishing_area`

echo "select concat( location_area,location_zone ) from $table_name;"	|
sql.e									|
sed 's/ent.*$//'							|
tee /dev/tty								|
cat > $output_filename

echo "created $output_filename"
exit 0
