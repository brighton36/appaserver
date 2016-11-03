:
application=creel

species_table_name=`get_table_name $application species`
genera_table_name=`get_table_name $application genera`
families_table_name=`get_table_name $application families`

species_select="$species_table_name.common_name,$species_table_name.taxonomic_serial_number,$species_table_name.measurement_termination,$species_table_name.florida_state_code"
genera_select="$genera_table_name.common_name"
families_select="$families_table_name.common_name"

(
/bin/echo "family,genus,species,catch_record_count,$species_select,$genera_select,$families_select"
while read record
do

	/bin/echo -n "$record"
	family=`echo $record | piece.e ',' 0`
	genus=`echo $record | piece.e ',' 1`
	species=`echo $record | piece.e ',' 2`
	results=`echo "	select $species_select
			from $species_table_name
			where family = '$family'
		  	and genus = '$genus'
		  	and species = '$species';" | sql.e`
	/bin/echo -n ",$results"
	results=`echo "	select $genera_select
			from $genera_table_name
			where family = '$family'
		  	and genus = '$genus';" | sql.e`
	/bin/echo -n ",$results"
	results=`echo "	select $families_select
			from $families_table_name
			where family = '$family';" | sql.e`
	/bin/echo -n ",$results"
	/bin/echo
done < species_count.dat
) 				|
tr '^' ','			|
tee species_count_meta.csv	|
cat

exit 0
