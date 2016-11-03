:
application=creel

output_filename=species_count.csv
species_table_name=`get_table_name $application species`
catches_table_name=`get_table_name $application catches`

#sys_string="echo \"	select family,genus,species
#			from $species_table_name
#			where family like 'A%';\" |
#	    sql.e"

sys_string="echo \"	select family,genus,species
			from $species_table_name;\" |
	    sql.e"

(
for record in `eval $sys_string`
do

	/bin/echo -n "$record: "
	family=`echo $record | piece.e '^' 0`
	genus=`echo $record | piece.e '^' 1`
	species=`echo $record | piece.e '^' 2`
	echo "	select count(family)
		from $catches_table_name
		where family = '$family'
		  and genus = '$genus'
		  and species = '$species';" | sql.e
done
) |
sort -t\: -rn --key=2		|
sed 's/\^/,/g'			|
sed 's/:/,/g'			|
tee $output_filename		|
cat

echo "Created $output_filename"

exit 0
